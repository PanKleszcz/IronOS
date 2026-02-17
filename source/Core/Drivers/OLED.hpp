/*
 * OLED.hpp
 *
 *  Created on: 20Jan.,2017
 *      Author: Ben V. Brown <Ralim>
 *      Designed for the SSD1307
 *      Cleared for release for TS100 2017/08/20
 */

#ifndef OLED_HPP_
#define OLED_HPP_
#include "Font.h"
#include "cmsis_os.h"
#include "configuration.h"
#include <BSP.h>
#include <stdbool.h>
#include <string.h>

#if defined(OLED_I2CBB2)
#include "I2CBB2.hpp"
#define I2C_CLASS I2CBB2
#elif defined(OLED_I2CBB1)
#include "I2CBB1.hpp"
#define I2C_CLASS I2CBB1
#else
#define I2C_CLASS FRToSI2C
#include "I2C_Wrapper.hpp"
#endif

#define DEVICEADDR_OLED (0x3c << 1)

#ifdef OLED_128x32

#define OLED_WIDTH           128
#define OLED_HEIGHT          32
#define OLED_GRAM_START      0x00 // Should be 0x00 when we have full width
#define OLED_GRAM_END        0x7F // Should be 0x7F when we have full width
#define OLED_GRAM_START_FLIP 0
#define OLED_GRAM_END_FLIP   0x7F

#define OLED_VCOM_LAYOUT 0x12
#define OLED_SEGMENT_MAP_REVERSED
#define OLED_DIVIDER 0xD3

#else

#define OLED_WIDTH           96
#define OLED_HEIGHT          16
#define OLED_GRAM_START      0x20
#define OLED_GRAM_END        0x7F
#define OLED_GRAM_START_FLIP 0
#define OLED_GRAM_END_FLIP   95

#define OLED_VCOM_LAYOUT 0x02
#define OLED_SEGMENT_MAP 0xA0
#define OLED_DIVIDER     0xD5

#endif /* OLED_128x32 */

#define OLED_ON  0xAF
#define OLED_OFF 0xAE

#define FRAMEBUFFER_START 17

class OLED {
public:

  static void initialize(); // Startup the I2C coms (brings screen out of reset etc)
  // Draw the buffer out to the LCD if any content has changed.
  static void refresh() {
    if (checkDisplayBufferChecksum()) {
      const int len = FRAMEBUFFER_START + (OLED_WIDTH * (OLED_HEIGHT / 8));
      I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, len);
      // DMA tx time is ~ 20mS Ensure after calling this you delay for at least 25ms
      // or we need to goto double buffering
    }
  }

  static void setDisplayState(bool state) {
    screenBuffer[1] = (state) ? OLED_ON : OLED_OFF;
    // Dump the screen state change out _now_
    I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, FRAMEBUFFER_START - 1);
    osDelay(TICKS_10MS);
  }

  // Set the rotation for the screen
  static void setRotation(bool leftHanded);
  static void setBrightness(uint8_t contrast);
  static void setInverse(bool inverted);

  // Clears the buffer
  static void clearScreen() { memset(stripPointers[0], 0, OLED_WIDTH * (OLED_HEIGHT / 8)); }
  // Draws a checkbox
  inline static void drawUnavailableIcon() { drawArea(OLED_WIDTH - OLED_HEIGHT - 2, 0, OLED_HEIGHT, OLED_HEIGHT, UnavailableIcon); }

  static void drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr);        // Draw an area, but y must be aligned on 0/8 offset
  static void drawAreaSwapped(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr); // Draw an area, but y must be aligned on 0/8 offset
  static void fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value);       // Fill an area, but y must be aligned on 0/8 offset
  static void drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear);

  static void flushSecondBuffer(void);
  static void useSecondaryFramebuffer(bool useSecondary);

  static bool scrollDown(uint8_t pos);
  static bool scrollUp(uint8_t pos);
  static bool scrollHorizontal(const bool dirForward, uint16_t progress, uint8_t offset);
  static void setFramebuffer(uint8_t *buffer);

private:
  static bool checkDisplayBufferChecksum() {
    uint32_t  hash = 0;
    const int len  = sizeof(screenBuffer);
    for (int i = 0; i < len; i++) {
      hash += (i * screenBuffer[i]);
    }

    bool result     = hash != displayChecksum;
    displayChecksum = hash;
    return result;
  }
  static uint8_t     *stripPointers[4]; // Pointers to the strips to allow for buffer having extra content
  static uint32_t     displayChecksum;
  static uint8_t      screenBuffer[16 + (OLED_WIDTH * (OLED_HEIGHT / 8)) + 10]; // The data buffer
  static uint8_t      secondFrameBuffer[16 + OLED_WIDTH * (OLED_HEIGHT / 8) + 10];
  static uint8_t      loopCounter;
};

#endif /* OLED_HPP_ */
