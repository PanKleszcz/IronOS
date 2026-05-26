/*
 * OLED.hpp
 *
 *  Created on: 20Jan.,2017
 *      Author: Ben V. Brown <Ralim>, MrTick
 *      Designed for the SSD1307
 *      Cleared for release for TS100 2017/08/20
 */

#pragma once
#include "configuration.h"
#ifdef LCD_160x80
#include "Font.h"
#include "cmsis_os.h"
#include <BSP.h>
#include <stdbool.h>
#include <string.h>

#include <SPI_Wrapper.hpp>
#define SPI_CLASS FRToSSPI

// TODO: use whole size of the display
#define LCD_WIDTH           128
#define LCD_HEIGHT          32


#define ST7735_XOFFSET 1
#define ST7735_YOFFSET 26

class LCD {
public:

  static void initialize(); // Startup the I2C coms (brings screen out of reset etc)
  // Draw the buffer out to the LCD if any content has changed.
  static void refresh() {

    if (checkDisplayBufferChecksum()) {
      const int len = (LCD_WIDTH * (LCD_HEIGHT / 8));

      // TODO: don't use strip buffers
      setDrawingWindow(16, 24, LCD_WIDTH, 8);
      SPI_CLASS::sendPixels(stripPointers[0], len/4);
      setDrawingWindow(16, 32, LCD_WIDTH, 8);
      SPI_CLASS::sendPixels(stripPointers[1], len/4);
      setDrawingWindow(16, 40, LCD_WIDTH, 8);
      SPI_CLASS::sendPixels(stripPointers[2], len/4);
      setDrawingWindow(16, 48, LCD_WIDTH, 8);
      SPI_CLASS::sendPixels(stripPointers[3], len/4);

    }
  }

  static void setDisplayState(bool state) {
    // TODO: implement
    osDelay(TICKS_10MS);
  }

  // Set the rotation for the screen
  static void setRotation(bool leftHanded);
  static void setBrightness(uint8_t brightness);
  static void setInverse(bool inverted);

  // Clears the buffer
  static void clearScreen() { memset(stripPointers[0], 0, LCD_WIDTH * (LCD_HEIGHT / 8)); }
  inline static void drawUnavailableIcon() { drawArea(LCD_WIDTH - LCD_HEIGHT - 2, 0, LCD_HEIGHT, LCD_HEIGHT, UnavailableIcon); }

  static void drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr);        // Draw an area, but y must be aligned on 0/8 offset
  static void drawAreaSwapped(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr); // Draw an area, but y must be aligned on 0/8 offset
  static void fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value);       // Fill an area, but y must be aligned on 0/8 offset
  static void drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear);
  static void drawHeatSymbol(uint8_t state);

  static void flushSecondBuffer(void);
  static void useSecondaryFramebuffer(bool useSecondary);

  static bool scrollDown(uint8_t pos);
  static bool scrollUp(uint8_t pos);
  static bool scrollHorizontal(const bool dirForward, uint16_t progress, uint8_t offset);
  static void setFramebuffer(uint8_t *buffer);

private:
  static bool checkDisplayBufferChecksum() {
static_assert(sizeof(screenBuffer) % 4 == 0, "screenBuffer size must be multiple of 4");
    uint32_t hash = 0;
    uint32_t len = sizeof(screenBuffer)/4;
    uint32_t *pBuffer = (uint32_t*)screenBuffer;
    while (len > 0) {
      hash += (len * (*pBuffer++));
len--;
    }

    bool result     = hash != displayChecksum;
    displayChecksum = hash;
    return result;
  }
  static void         drawChar(uint16_t charCode, FontStyle fontStyle, const uint8_t soft_x_limit); // Draw a character to the current cursor location
  static void         setDrawingWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

  static uint8_t     *stripPointers[4]; // Pointers to the strips to allow for buffer having extra content
  static uint32_t     displayChecksum;
  static uint8_t      screenBuffer[LCD_WIDTH * (LCD_HEIGHT / 8)]; // The data buffer
  static uint8_t      secondFrameBuffer[LCD_WIDTH * (LCD_HEIGHT / 8)];
  static uint8_t      loopCounter;
};

#endif // LCD_160x80
