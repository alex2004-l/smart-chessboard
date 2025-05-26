#include "led_band.h"
#include "moves.h"
#include "lcd1602.h"

#define DS 17
#define STcp 19
#define SHcp 18

#define WHITE_BUTTON 13
#define BLACK_BUTTON 16

#define DEBOUNCE_DELAY 100

typedef enum player{
  WHITE, BLACK
} PLAYER;


typedef enum pieces {
  EMPTY, PAWN, ROOK, KNIGHT,
  BISHOP, KING, QUEEN
} PIECE;


typedef enum states {
  BEGINNING, READY, PICK,
  MOVE, CAPTURE, END
} STATES;


uint8_t current_player;
bool move_made;
uint8_t state;

uint32_t last_debounce_white = 0;
uint32_t last_debounce_black = 0;

uint32_t player_time_white = 600;
uint32_t player_time_black = 600;

uint8_t board_configuration[8][8] = {
  {ROOK,  KNIGHT, BISHOP, QUEEN, KING,  BISHOP, KNIGHT, ROOK},
  {PAWN,  PAWN,   PAWN,   PAWN,  PAWN,  PAWN,   PAWN,   PAWN},
  {EMPTY, EMPTY,  EMPTY,  EMPTY, EMPTY, EMPTY,  EMPTY, EMPTY},
  {EMPTY, EMPTY,  EMPTY,  EMPTY, EMPTY, EMPTY,  EMPTY, EMPTY},
  {EMPTY, EMPTY,  EMPTY,  EMPTY, EMPTY, EMPTY,  EMPTY, EMPTY},
  {EMPTY, EMPTY,  EMPTY,  EMPTY, EMPTY, EMPTY,  EMPTY, EMPTY},
  {PAWN,  PAWN,   PAWN,   PAWN,  PAWN,  PAWN,   PAWN,   PAWN},
  {ROOK,  KNIGHT, BISHOP, QUEEN, KING,  BISHOP, KNIGHT, ROOK}
};

uint8_t previous_board[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t board[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// Possible to need to change 14
uint8_t column_pins[8] = {34, 35, 32, 33, 25, 26, 27, 14};


void read_board(uint8_t board[8]) {
  for (int row = 0; row < 8; row ++) {
    sendToShiftRegister(1 << row);
    delay(1);
    board[row] = 0;
    for (int col = 0; col < 8; col++) {
      board[row] |= (digitalRead(column_pins[col]) == HIGH) << col;
    }
  }
}


uint8_t count_pieces_on_table(uint8_t b[8]) {
  uint8_t cnt_pieces = 0;
  for (int row = 0; row < 8; row ++) {
    for (int col = 0; col < 8; col++) {
      if (b[row] & (1<< col))
        cnt_pieces ++;
    }
  }
  return cnt_pieces;
}

// interrupt when white presses button
void IRAM_ATTR press_button_white() {
  if ((millis() - last_debounce_white) < DEBOUNCE_DELAY) return;
  last_debounce_white = millis();

  switch (state) {
    case BEGINNING:
      Serial.println("Please put all pieces on the board before starting.");
      break;

    case READY:
      if (current_player == WHITE) {
        state = MOVE;
        // to start timer for white
      }
      break;
    case PICK:
      break;
    case MOVE:
        if (current_player == WHITE) {
            current_player = BLACK;
            state = PICK;
        }
        break;

    default:
      break;
  }
}

// interrupt when black presses button
void IRAM_ATTR press_button_black() {
    if ((millis() - last_debounce_black) < DEBOUNCE_DELAY) return;
  last_debounce_black = millis();

  switch (state) {
    case BEGINNING:
      Serial.println("Please put all pieces on the board before starting.");
      break;
    case READY:
      if (current_player == BLACK) {
        state = MOVE;
        Serial.println("Game started. Black's turn.");
        // To start timer for black
      }
      break;
    case PICK:
      break;
    case MOVE:
        if (current_player == BLACK) {
            current_player = WHITE;
            state = PICK;
        }
        break;

    default:
      break;
  }

}


void run_action() {
    switch (state)
    {
    case BEGINNING:
        if (count_pieces_on_table(board) == 32) {
          state = READY;
        } else {
            Serial.println("Waiting for all pieces to be placed on the board.");
        }
        break;
    case READY:
        Serial.println("All pieces on board. Game ready to start.");
        break;
    case PICK:
        break;
    case MOVE:
        break;
    case CAPTURE:
        break;
    case END:
        break;
    default:
        break;
    }
}


void sendToShiftRegister(byte data) {
  digitalWrite(STcp, LOW);
  shiftOut(DS, SHcp, MSBFIRST, data); 
  digitalWrite(STcp, HIGH);
}


void setup() {
  // Inițializare variabile
  current_player = WHITE;
  move_made = false;
  state = BEGINNING;

  // Pinii pentru registru
  pinMode(DS, OUTPUT);
  pinMode(STcp, OUTPUT);
  pinMode(SHcp, OUTPUT);

  // pinii pentru butoane
  pinMode(WHITE_BUTTON, INPUT_PULLUP);
  pinMode(BLACK_BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(WHITE_BUTTON), press_button_white, FALLING);
  attachInterrupt(digitalPinToInterrupt(BLACK_BUTTON), press_button_black, FALLING);

  for (int i = 0; i < 8; i++) {
    pinMode(column_pins[i], INPUT);
  }

  Serial.begin(115200);
  // make sure serial2 is disabled and gpios 16 and 17 can be safely used
  Serial2.end();

  delay(10000);

  init_lcd();

    Serial.println("Scanning I2C devices...");
  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found");
  else Serial.println("Done.");

}

void loop() {
  read_board(board);
  // run_action();

  display_message("Bunaa, iub!!");

  // memcpy(previous_board, board, sizeof(previous_board));
  delay(1000);
}
