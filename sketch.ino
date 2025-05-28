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

int8_t selected_row   = -1;
int8_t selected_column = -1;

uint32_t last_debounce_white = 0;
uint32_t last_debounce_black = 0;

uint32_t player_time_white = 600;
uint32_t player_time_black = 600;

volatile bool white_pressed = false;
volatile bool black_pressed = false;

byte key = 0;

int8_t board_configuration[8][8] = {
  {-ROOK,  -KNIGHT, -BISHOP, -QUEEN, -KING,  -BISHOP, -KNIGHT, -ROOK},
  {-PAWN,  -PAWN,   -PAWN,   -PAWN,  -PAWN,  -PAWN,   -PAWN,   -PAWN},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY,  EMPTY,   EMPTY,  EMPTY},
  { PAWN,  PAWN,    PAWN,    PAWN,   PAWN,   PAWN,    PAWN,   PAWN},
  { ROOK,  KNIGHT,  BISHOP,  QUEEN,  KING,   BISHOP,  KNIGHT, ROOK}
};

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

void pick_piece_to_move(void) {
  display_message(lcd, "Select piece");
  if (current_player == WHITE) {
    display_message(lcd, "WHITE", 0, 1, false);
  } else {
    display_message(lcd, "BLACK", 0, 1, false);
  }
  delay(500);
  key = Read_TTP229_Keypad();
  if (key) {
    Serial.println(key);
    if (key <= 8) {
      if (selected_row == -1) {
        selected_row = key - 1;
      } else {
        display_message(lcd, "Row already selected");
      }
    } else {
      if (selected_column == -1) {
        selected_column = key - 9;
      } else {
        display_message(lcd, "Column already selected");
      }
    }
  }
  if (selected_row != -1 && selected_column != -1) {

    int8_t piece = board_configuration[selected_row][selected_column];
    if (current_player == WHITE && piece < 0) {
      display_message(lcd, "Not your piece");
    } else if (current_player == BLACK && piece > 0) {
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
        break;
      default:
        display_message(lcd, "Something fishy");
        break;
      };
    }

    selected_row = -1;
    selected_column = -1;
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
        display_message(lcd, "Selected new position");
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

  display_message(lcd, "Smart chessboard");
}


void loop() {
  run_action();
  delay(1000);
}

ISR(INT0_vect) {
  white_pressed = true;
}

ISR(INT1_vect) {
  black_pressed = true;
}

