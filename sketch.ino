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
  BEGINNING, START, PICK_ROW,
  PICK_COLUMN, MOVE, END
} STATES;


uint8_t current_player;
bool move_made;
uint8_t state;

// uint32_t last_debounce_white = 0;
// uint32_t last_debounce_black = 0;

uint32_t player_time_white = 600;
uint32_t player_time_black = 600;

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

// interrupt when white presses button
// void IRAM_ATTR press_button_white() {
//   if ((millis() - last_debounce_white) < DEBOUNCE_DELAY) return;
//   last_debounce_white = millis();

//   switch (state) {
//     case BEGINNING:
//       Serial.println("Please put all pieces on the board before starting.");
//       break;

//     case READY:
//       if (current_player == WHITE) {
//         state = MOVE;
//         // to start timer for white
//       }
//       break;
//     case PICK_ROW:
//       break;
//     case PICK_COLUMN:
//       break;
//     case MOVE:
//         if (current_player == WHITE) {
//             current_player = BLACK;
//             state = PICK_ROW;
//         }
//         break;

//     default:
//       break;
//   }
// }

// interrupt when black presses button
// void IRAM_ATTR press_button_black() {
//   if ((millis() - last_debounce_black) < DEBOUNCE_DELAY) return;
//   last_debounce_black = millis();

//   switch (state) {
//     case BEGINNING:
//       display_message(lcd"Please put all pieces on the board before starting.");
//       break;
//     case READY:
//       if (current_player == BLACK) {
//         state = MOVE;
//         Serial.println("Game started. Black's turn.");
//         // To start timer for black
//       }
//       break;
//     case PICK_ROW:
//       break;
//     case PICK_COLUMN:
//         if (current_player == BLACK) {
//             current_player = WHITE;
//             state = PICK_ROW;
//         }
//         break;

//     default:
//       break;
//   }
// }


void run_action() {
    switch (state)
    {
    case BEGINNING:
        Serial.println("Waiting for all pieces to be placed on the board.");
        break;
    case START:
        Serial.println("All pieces on board. Game ready to start.");
        break;
    case PICK_ROW:
        break;
    case PICK_COLUMN:
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
    if (!digitalRead(SDO))
      Key_State = Num;
      digitalWrite(SCL, HIGH);
  } 
  return Key_State;
}


void setup() {
  current_player = WHITE;
  move_made = false;
  state = BEGINNING;

  // pinii pentru butoane
  pinMode(WHITE_BUTTON, INPUT);
  pinMode(BLACK_BUTTON, INPUT);

  pinMode(SCL, OUTPUT); 
  pinMode(SDO, INPUT);

  // attachInterrupt(digitalPinToInterrupt(WHITE_BUTTON), press_button_white, FALLING);
  // attachInterrupt(digitalPinToInterrupt(BLACK_BUTTON), press_button_black, FALLING);

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
}


void loop() {

  delay(1000);
}
