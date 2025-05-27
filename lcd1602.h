#ifndef LCD1602_H
#define LCD1602_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_Devices 127

int scan_I2C_devices(byte devices[10]) {
  Wire.begin();

  byte error, address;
  int n_devices = 0;

  for (address = 1; address < I2C_Devices; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      devices[n_devices++] = address;
    }
  }

  return n_devices;
}


void init_lcd(LiquidCrystal_I2C *lcd) {
  lcd->init();
  lcd->backlight();
  lcd->clear();
}


void display_message(LiquidCrystal_I2C *lcd, const char* message, int col = 0, int row = 0, bool clear_before = true) {
  if (clear_before) {
    lcd->clear();
  }
  lcd->setCursor(col, row);
  lcd->print(message);
}

#endif