#ifndef LED_BAND_H
#define LED_BAND_H

#include <Adafruit_NeoPixel.h>

#define PIN_WS2812B 12 
#define NUM_PIXELS 64

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

void setup_WS2812B() {
  ws2812b.begin();
}

void stop_all_pixels_WS2812B() {
  ws2812b.clear();
  ws2812b.show();
}

int __get_pixel_index(int x, int y) {
  return (!(x % 2)) ? (x * 8 + y) : (x * 8 - y + 8);
}

void turn_on_pixels_WS2812B(uint8_t n, uint8_t positions[64][2], uint8_t red, uint8_t green, uint8_t blue, bool one_by_one=false) {
  uint32_t color = ws2812b.Color(red, green, blue);
  for (uint8_t i = 0; i < n; i ++) {
    int pixel_idx = __get_pixel_index(positions[i][0], positions[i][1]);
    ws2812b.setPixelColor(pixel_idx, color);
    if (one_by_one)
      ws2812b.show();
  }
  if (!one_by_one)
    ws2812b.show();
}

#endif
