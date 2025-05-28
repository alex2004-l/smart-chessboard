#include "led_strip.h"
#include "moves.h"
#include "lcd1602.h"

// Buttons
#define WHITE_BUTTON 2
#define BLACK_BUTTON 3

// Led band
#define PIN_WS2812B 6
#define NUM_PIXELS 64

// Capacitive senzor plate
#define SCL 8
#define SDO 9

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C* lcd = nullptr;

typedef enum player{
  WHITE, BLACK
} PLAYER;

typedef enum states {
  BEGINNING, START, PICK_PIECE_TO_MOVE,
  PICK_NEW_POSITION, MOVE, END
} STATES;


uint8_t current_player;
bool move_made;
uint8_t state;

int8_t selected_row    = -1;
int8_t selected_column = -1;
int8_t new_row         = -1;
int8_t new_column     = -1;

uint32_t last_debounce_white = 0;
uint32_t last_debounce_black = 0;

uint32_t player_time_white = 600;
uint32_t player_time_black = 600;

volatile bool white_pressed = false;
volatile bool black_pressed = false;

byte key = 0;

int8_t board_configuration[8][8] = {
  { ROOK,  KNIGHT,  BISHOP,  QUEEN,  KING,   BISHOP,  KNIGHT, ROOK},
  { PAWN,  PAWN,    PAWN,    PAWN,   PAWN,   PAWN,    PAWN,   PAWN},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  {-PAWN,  -PAWN,   -PAWN,   -PAWN,  -PAWN,  -PAWN,   -PAWN,   -PAWN},
  {-ROOK,  -KNIGHT, -BISHOP, -QUEEN, -KING,  -BISHOP, -KNIGHT, -ROOK}
};

int no_moves = 0;
uint8_t possible_moves[64][2];

void beginning(void) {
  display_message(lcd, "Smart chessboard");
  display_message(lcd, "Press to start", 0, 1, false);
  if (white_pressed || black_pressed) {
    state = START;
    white_pressed = false;
    black_pressed = false;
  }
}

void start(void) {
  display_message(lcd, "3");
  delay(1000);
  display_message(lcd, "2");
  delay(1000);
  display_message(lcd, "1");
  delay(1000);
  display_message(lcd, "Start");
  state = PICK_PIECE_TO_MOVE;
  if (white_pressed || black_pressed) {
    Serial.println("Invalid pressing of buttons during initialization");
    white_pressed = false;
    black_pressed = false;
  }
}


void read_position(uint8_t *row, uint8_t *column){
  key = Read_TTP229_Keypad();
  if (key) {
    Serial.println(key);
    if (key <= 8) {
      if (*row == -1) {
        *row = key - 1;
      } else {
        display_message(lcd, "Row already selected");
        delay(1000);
        return;
      }
    } else {
      if (*column == -1) {
        *column = key - 9;
      } else {
        display_message(lcd, "Column already selected");
        delay(1000);
        return;
      }
    }
  }
}


void pick_piece_to_move(void) {
  display_message(lcd, "Select piece");
  if (current_player == WHITE) {
    display_message(lcd, "WHITE", 0, 1, false);
  } else {
    display_message(lcd, "BLACK", 0, 1, false);
  }
  delay(1000);
  read_position(&selected_row, &selected_column);

  if (is_valid(selected_row, selected_column)) {

    int8_t piece = board_configuration[selected_row][selected_column];
    if ((current_player == WHITE && piece < 0)
          || (current_player == BLACK && piece > 0)) {
      display_message(lcd, "Not your piece");
    } else {
      piece = (piece < 0) ? -piece : piece;
      state = PICK_NEW_POSITION;
      switch(piece) {
      case EMPTY:
        display_message(lcd, "No piece");
        state = PICK_PIECE_TO_MOVE;
        break;
      case PAWN:
        display_message(lcd, "Pawn");
        no_moves = get_pawn_moves(selected_row, selected_column, board_configuration, possible_moves, current_player == WHITE);
        break;
      case BISHOP:
        display_message(lcd, "BISHOP");
        no_moves = get_bishop_moves(selected_row, selected_column, board_configuration, possible_moves);
        break;
      case ROOK:
        display_message(lcd, "ROOK");
        no_moves = get_rook_moves(selected_row, selected_column, board_configuration, possible_moves);
        break;
      case KNIGHT:
        display_message(lcd, "KNIGHT");
        no_moves = get_knight_moves(selected_row, selected_column, board_configuration, possible_moves);
        break;
      case KING:
        display_message(lcd, "KING");
        no_moves = get_king_moves(selected_row, selected_column, board_configuration, possible_moves);
        break;
      case QUEEN:
        display_message(lcd, "QUEEN");
        no_moves = get_queen_moves(selected_row, selected_column, board_configuration, possible_moves);
        break;
      default:
        display_message(lcd, "Something fishy");
        break;
      };
    }
    if (!no_moves) {
      state = PICK_PIECE_TO_MOVE;
      display_message(lcd, "No available moves");
      selected_row = -1;
      selected_column = -1;
    }
    if (state == PICK_NEW_POSITION) {
      turn_on_pixels_WS2812B(ws2812b, no_moves, possible_moves);
    }
    delay(1000);
  }
}

void pick_new_position(void) {
  display_message(lcd, "Select new pos");
  if (current_player == WHITE) {
    display_message(lcd, "WHITE", 0, 1, false);
  } else {
    display_message(lcd, "BLACK", 0, 1, false);
  }
  delay(100);
  read_position(&new_row, &new_column);

  if (is_valid(new_row, new_column)) {
    for (int k = 0; k < no_moves; k++) {
      if (possible_moves[k][0] == new_row && possible_moves[k][1] == new_column) {
        stop_all_pixels_WS2812B(ws2812b);
        state = MOVE;
        display_message(lcd, "Move piece");
        board_configuration[new_row][new_column] = board_configuration[selected_row][selected_column];
        board_configuration[selected_row][selected_column] = EMPTY;
      }
    }

    if (state == PICK_NEW_POSITION) {
      display_message(lcd, "Illegal move");
      delay(1000);
    }
  }
}

void run_action() {
    switch (state)
    {
    case BEGINNING:
        beginning();
        break;
    case START:
        start();
        break;
    case PICK_PIECE_TO_MOVE:
        pick_piece_to_move();
        break;
    case PICK_NEW_POSITION:
        pick_new_position();
        break;
    case MOVE:
        if ((current_player == WHITE && white_pressed)|| (current_player==BLACK && black_pressed)) {
          state = PICK_PIECE_TO_MOVE;
          current_player = (current_player == WHITE) ? BLACK : WHITE;
          display_message(lcd, "Player's turn:");
          display_message(lcd, (current_player == WHITE) ? "WHITE" : "BLACK");
          white_pressed = false;
          black_pressed = false;
        } else if ((current_player == WHITE && black_pressed)|| (current_player==BLACK && white_pressed)) {
          display_message(lcd, "NOT YOUR TURN");
          white_pressed = false;
          black_pressed = false;
        }
        break;
    case END:
        break;
    default:
        break;
    }
}

byte Read_TTP229_Keypad(void) {
  byte Num;
  byte Key_State = 0;
  for(Num = 1; Num <= 16; Num++)
  {
    digitalWrite(SCL, LOW);
    delayMicroseconds(2);
    if (!digitalRead(SDO))
      Key_State = Num;
    digitalWrite(SCL, HIGH);
    delayMicroseconds(2);
  } 
  return Key_State;
}


void setup() {
  current_player = WHITE;
  move_made = false;
  state = BEGINNING;

  // pinii pentru butoane
  pinMode(WHITE_BUTTON, INPUT_PULLUP);
  pinMode(BLACK_BUTTON, INPUT_PULLUP);

  pinMode(SCL, OUTPUT); 
  pinMode(SDO, INPUT);

  Serial.begin(115200);

  Serial.println("Scanning I2C devices...");

  byte i2c_devices[10];
  int n_devices = scan_I2C_devices(i2c_devices);

  if (n_devices > 0) {
    byte lcd_address = i2c_devices[0];
    lcd = new LiquidCrystal_I2C(lcd_address, 16, 2);
    init_lcd(lcd);
  } else {
    Serial.println("No I2C devices found!");
  }

  // Activare intreruperi pinii de butoane
  cli();
  EICRA |= (1 << ISC01) | (1 << ISC11);
  EICRA &= ~((1 << ISC00) | (1 << ISC10));
  EIMSK |= (1 << INT0) | (1 << INT1);
  sei();

  setup_WS2812B(ws2812b);
  // turn_all(ws2812b);

  display_message(lcd, "Smart chessboard");
}


void loop() {
  run_action();
  // turn_all(ws2812b);
  delay(1000);
}

ISR(INT0_vect) {
  white_pressed = true;
}

ISR(INT1_vect) {
  black_pressed = true;
}

