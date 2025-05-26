#include "led_band.h"
#include "moves.h"

// Placehorder values for now
#define DS 35
#define STcp 32
#define SHcp 34

#define WHITE_BUTTON 12
#define BLACK_BUTTON 13

typedef enum player{
  WHITE, BLACK
} PLAYER;

typedef enum pieces {
  EMPTY, PAWN, ROOK, KNIGHT,
  BISHOP, KING, QUEEN
} PIECE;

typedef enum states {
  BEGINNING, READY,
  MOVE, CAPTURE, END
} STATES;

uint8_t current_player;
bool move_made;
uint8_t state;

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

uint8_t column_pins[8] = {};

void read_table() {
  for (int row = 0; row < 8; row ++) {
    sendToShiftRegister(1 << row);
    delay(1);
    board[row] = 0;
    for (int col = 0; col < 8; col++) {
      board[row] |= (digitalRead(column_pins[col]) == LOW) << col;
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
}

// interrupt when black presses button
void IRAM_ATTR press_button_black() {

}


void run_action() {
  if (state == BEGINNING) {
    if (count_pieces_on_table(board) == 32){
      state = READY;
    }
  } else if (state == READY) {

  }
}

void setup() {
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
}


void loop() {
  read_table();
  run_action();
  delay(1000);
}


void sendToShiftRegister(byte data) {
  digitalWrite(STcp, LOW);
  shiftOut(DS, SHcp, MSBFIRST, data); 
  digitalWrite(STcp, HIGH);
}

