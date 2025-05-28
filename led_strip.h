#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Adafruit_NeoPixel.h>

void setup_WS2812B(Adafruit_NeoPixel &ws2812b) {
  ws2812b.begin();
  ws2812b.show();
}

void stop_all_pixels_WS2812B(Adafruit_NeoPixel &ws2812b) {
  ws2812b.clear();
  ws2812b.show();
}

int __get_pixel_index(int x, int y) {
  return (!(x % 2)) ? (x * 8 + y) : (x * 8 - y + 7);
}

// Default color is red
void turn_on_pixels_WS2812B(Adafruit_NeoPixel &ws2812b, uint8_t n, uint8_t positions[64][2], uint8_t r=255, uint8_t g=0, uint8_t b=0, bool one_by_one=false) {
  uint32_t color = ws2812b.Color(r, g, b);
  for (uint8_t i = 0; i < n; i ++) {
    int pixel_idx = __get_pixel_index(positions[i][0], positions[i][1]);
    ws2812b.setPixelColor(pixel_idx, color);
    if (one_by_one)
      ws2812b.show();
      delay(50);
  }
  if (!one_by_one)
    ws2812b.show();
}

void turn_all(Adafruit_NeoPixel &ws2812b, uint8_t r=255, uint8_t g=0, uint8_t b=0) {
  uint32_t color = ws2812b.Color(r, g, b);
  for (uint8_t i = 0; i < 64; i ++) {
    ws2812b.setPixelColor(i, color);
  }
    ws2812b.show();
}

#endif