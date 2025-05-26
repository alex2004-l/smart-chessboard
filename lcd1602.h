#ifndef LCD1602_H
#define LCD1602_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA_PIN 21
#define SCL_PIN 22

LiquidCrystal_I2C lcd(0x27, 16, 2);

void init_lcd() {
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Hello, ESP32!");
  delay(2000);
  lcd.clear();
}

void display_message(const char* message, int col = 0, int row = 0, bool clear_before = true) {
  if (clear_before) {
    lcd.clear();
  }
  lcd.setCursor(col, row);
  lcd.print(message);
}

#endif