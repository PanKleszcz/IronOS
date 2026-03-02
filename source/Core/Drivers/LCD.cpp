/*
 * OLED.cpp
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "LCD.hpp"
#ifdef LCD_160x80

#include "Settings.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// rendering to the buffer
uint8_t *LCD::stripPointers[4]; // Pointers to the strips to allow for buffer having extra content

alignas(uint32_t) uint8_t LCD::screenBuffer[LCD_WIDTH * (LCD_HEIGHT / 8)]; // The data buffer
alignas(uint32_t) uint8_t LCD::secondFrameBuffer[LCD_WIDTH * (LCD_HEIGHT / 8)];
uint32_t LCD::displayChecksum;

// ST7735 Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

const FRToSSPI::SPI_CMD lcdInitCmds[] = {
    {ST7735_SWRESET, FRToSSPI::SPI_CMD_DELAY_MS, 150,                                                                                                        NULL},
    { ST7735_SLPOUT, FRToSSPI::SPI_CMD_DELAY_MS, 200,                                                                                                        NULL},

    {ST7735_FRMCTR1,  FRToSSPI::SPI_CMD_PAYLOAD,   3,                                                                               (uint8_t[]){0x05, 0x3A, 0x3A}},
    {ST7735_FRMCTR2,  FRToSSPI::SPI_CMD_PAYLOAD,   3,                                                                               (uint8_t[]){0x05, 0x3A, 0x3A}},
    {ST7735_FRMCTR3,  FRToSSPI::SPI_CMD_PAYLOAD,   6,                                                             (uint8_t[]){0x05, 0x3A, 0x3A, 0x05, 0x3A, 0x3A}},

    { ST7735_PWCTR1,  FRToSSPI::SPI_CMD_PAYLOAD,   3,                                                                               (uint8_t[]){0x62, 0x02, 0x04}},
    { ST7735_PWCTR2,  FRToSSPI::SPI_CMD_PAYLOAD,   1,                                                                                           (uint8_t[]){0xC0}},
    { ST7735_PWCTR3,  FRToSSPI::SPI_CMD_PAYLOAD,   2,                                                                                     (uint8_t[]){0x0D, 0x00}},
    { ST7735_PWCTR4,  FRToSSPI::SPI_CMD_PAYLOAD,   2,                                                                                     (uint8_t[]){0x8D, 0x6A}},
    { ST7735_PWCTR5,  FRToSSPI::SPI_CMD_PAYLOAD,   2,                                                                                     (uint8_t[]){0x8D, 0xEE}},

    {ST7735_GMCTRP1,  FRToSSPI::SPI_CMD_PAYLOAD,  16, (uint8_t[]){0x10, 0x0E, 0x02, 0x03, 0x0E, 0x07, 0x02, 0x07, 0x0A, 0x12, 0x27, 0x37, 0x00, 0x0D, 0x0E, 0x10}},
    {ST7735_GMCTRN1,  FRToSSPI::SPI_CMD_PAYLOAD,  16, (uint8_t[]){0x10, 0x0E, 0x03, 0x03, 0x0F, 0x06, 0x02, 0x08, 0x0A, 0x13, 0x26, 0x36, 0x00, 0x0D, 0x0E, 0x10}},

    { ST7735_INVCTR,  FRToSSPI::SPI_CMD_PAYLOAD,   1,                                                                                           (uint8_t[]){0x03}},
    {  ST7735_INVON,  FRToSSPI::SPI_CMD_PAYLOAD,   0,                                                                                                        NULL},
    { ST7735_VMCTR1,  FRToSSPI::SPI_CMD_PAYLOAD,   1,                                                                                           (uint8_t[]){0x0E}},
    { ST7735_MADCTL,  FRToSSPI::SPI_CMD_PAYLOAD,   1,                                                                                           (uint8_t[]){0x88}},
    { ST7735_COLMOD,  FRToSSPI::SPI_CMD_PAYLOAD,   1,                                                                                           (uint8_t[]){0x05}},

    {  ST7735_NORON, FRToSSPI::SPI_CMD_DELAY_MS,  10,                                                                                                        NULL},
    { ST7735_DISPON, FRToSSPI::SPI_CMD_DELAY_MS, 100,                                                                                                        NULL},
};

FRToSSPI::SPI_CMD lcdSetAreaCmds[] = {
    {ST7735_RASET, FRToSSPI::SPI_CMD_PAYLOAD, 4,  (uint8_t[]){0, ST7735_XOFFSET, 0, LCD_WIDTH + ST7735_XOFFSET - 1}},
    {ST7735_CASET, FRToSSPI::SPI_CMD_PAYLOAD, 4, (uint8_t[]){0, ST7735_YOFFSET, 0, LCD_HEIGHT + ST7735_YOFFSET - 1}},
    {ST7735_RAMWR, FRToSSPI::SPI_CMD_PAYLOAD, 0,                                                               NULL},
};

void LCD::setDrawingWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  lcdSetAreaCmds[0].data[1] = x + ST7735_XOFFSET;
  lcdSetAreaCmds[0].data[3] = x + w + ST7735_XOFFSET - 1;
  lcdSetAreaCmds[1].data[1] = y + ST7735_YOFFSET;
  lcdSetAreaCmds[1].data[3] = y + h + ST7735_YOFFSET - 1;

  FRToSSPI::sendCmdChain(lcdSetAreaCmds, sizeof(lcdSetAreaCmds) / sizeof(*lcdSetAreaCmds));
}

/*
 * Animation timing function that follows a bezier curve.
 * @param t A given percentage value [0..<100]
 * Returns a new percentage value with ease in and ease out.
 * Original floating point formula: t * t * (3.0f - 2.0f * t);
 */
static uint16_t easeInOutTiming(uint16_t t) { return t * t * (300 - 2 * t) / 10000; }

/*
 * Returns the value between a and b, using a percentage value t.
 * @param a The value associated with 0%
 * @param b The value associated with 100%
 * @param t The percentage [0..<100]
 */
static uint16_t lerp(uint16_t a, uint16_t b, uint16_t t) { return a + t * (b - a) / 100; }

void LCD::initialize() {
  stripPointers[0] = &screenBuffer[0 * LCD_WIDTH];
  stripPointers[1] = &screenBuffer[1 * LCD_WIDTH];
  stripPointers[2] = &screenBuffer[2 * LCD_WIDTH];
  stripPointers[3] = &screenBuffer[3 * LCD_WIDTH];

  FRToSSPI::init();
  FRToSSPI::sendCmdChain(lcdInitCmds, sizeof(lcdInitCmds) / sizeof(*lcdInitCmds));

  // Erase background
  setDrawingWindow(0, 0, 160, 80);
  FRToSSPI::sendByteMutiple(0x00, 2 * 160 * 80);

  // Draw a nice frame for the emulated OLED display
  setDrawingWindow(12, 20, 136, 40);
  FRToSSPI::sendByteMutiple(0xFF, 2 * 136 * 40);
  setDrawingWindow(14, 22, 132, 36);
  FRToSSPI::sendByteMutiple(0x00, 2 * 132 * 36);
}

void LCD::setFramebuffer(uint8_t *buffer) {
  stripPointers[0] = &buffer[0 * LCD_WIDTH];
  stripPointers[1] = &buffer[1 * LCD_WIDTH];
  stripPointers[2] = &buffer[2 * LCD_WIDTH];
  stripPointers[3] = &buffer[3 * LCD_WIDTH];
}

/**
 * Plays a transition animation between two framebuffers.
 *
 * If forward is true, this displays a forward navigation to the second framebuffer contents.
 * Otherwise a rewinding navigation animation is shown to the second framebuffer contents.
 */
bool LCD::scrollHorizontal(const bool dirForward, uint16_t progress, uint8_t offset) {
  uint8_t *stripBackPointers[4];
  stripBackPointers[0] = &secondFrameBuffer[0 * LCD_WIDTH];
  stripBackPointers[1] = &secondFrameBuffer[1 * LCD_WIDTH];
  stripBackPointers[2] = &secondFrameBuffer[2 * LCD_WIDTH];
  stripBackPointers[3] = &secondFrameBuffer[3 * LCD_WIDTH];

  // When forward, current contents move to the left out.
  // Otherwise the contents move to the right out.
  uint8_t oldStart    = dirForward ? 0 : progress;
  uint8_t oldPrevious = dirForward ? progress - offset : offset;

  // Content from the second framebuffer moves in from the right (forward)
  // or from the left (not forward).
  uint8_t newStart = dirForward ? LCD_WIDTH - progress : 0;
  uint8_t newEnd   = dirForward ? 0 : LCD_WIDTH - progress;

  offset = progress;

  memmove(&stripPointers[0][oldStart], &stripPointers[0][oldPrevious], LCD_WIDTH - progress);
  memmove(&stripPointers[1][oldStart], &stripPointers[1][oldPrevious], LCD_WIDTH - progress);
  memmove(&stripPointers[2][oldStart], &stripPointers[2][oldPrevious], LCD_WIDTH - progress);
  memmove(&stripPointers[3][oldStart], &stripPointers[3][oldPrevious], LCD_WIDTH - progress);

  memmove(&stripPointers[0][newStart], &stripBackPointers[0][newEnd], progress);
  memmove(&stripPointers[1][newStart], &stripBackPointers[1][newEnd], progress);
  memmove(&stripPointers[2][newStart], &stripBackPointers[2][newEnd], progress);
  memmove(&stripPointers[3][newStart], &stripBackPointers[3][newEnd], progress);

  return true;
}

void LCD::useSecondaryFramebuffer(bool useSecondary) {
  if (useSecondary) {
    setFramebuffer(secondFrameBuffer);
  } else {
    setFramebuffer(screenBuffer);
  }
}

/**
 * This assumes that the current display output buffer has the current on screen contents
 * Then the secondary buffer has the "new" contents to be slid up onto the screen
 * Sadly we cant use the hardware scroll as some devices with the 128x32 screens dont have the GRAM for holding both screens at once
 *
 * **This function blocks until the transition has completed or user presses button**
 */
bool LCD::scrollDown(uint8_t pos) {

  // For each line, we shuffle all bits up a row
  for (uint8_t xPos = 0; xPos < LCD_WIDTH; xPos++) {
    const uint16_t firstStripPos  = xPos;
    const uint16_t secondStripPos = firstStripPos + LCD_WIDTH;
    const uint16_t thirdStripPos  = secondStripPos + LCD_WIDTH;
    const uint16_t fourthStripPos = thirdStripPos + LCD_WIDTH;

    // Move the MSB off the first strip, and pop MSB from second strip onto the first strip
    screenBuffer[firstStripPos] = (screenBuffer[firstStripPos] >> 1) | ((screenBuffer[secondStripPos] & 0x01) << 7);
    // Now shuffle off the second strip
    screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] >> 1) | ((screenBuffer[thirdStripPos] & 0x01) << 7);
    // Now shuffle off the third strip
    screenBuffer[thirdStripPos] = (screenBuffer[thirdStripPos] >> 1) | ((screenBuffer[fourthStripPos] & 0x01) << 7);
    // Now forth strip gets the start of the new buffer
    screenBuffer[fourthStripPos] = (screenBuffer[fourthStripPos] >> 1) | ((secondFrameBuffer[firstStripPos] & 0x01) << 7);
    // Now cycle all the secondary buffers

    secondFrameBuffer[firstStripPos]  = (secondFrameBuffer[firstStripPos] >> 1) | ((secondFrameBuffer[secondStripPos] & 0x01) << 7);
    secondFrameBuffer[secondStripPos] = (secondFrameBuffer[secondStripPos] >> 1) | ((secondFrameBuffer[thirdStripPos] & 0x01) << 7);
    secondFrameBuffer[thirdStripPos]  = (secondFrameBuffer[thirdStripPos] >> 1) | ((secondFrameBuffer[fourthStripPos] & 0x01) << 7);
    // Finally on the bottom row; we shuffle it up ready
    secondFrameBuffer[fourthStripPos] >>= 1;
  }

  return true;
}
/**
 * This assumes that the current display output buffer has the current on screen contents
 * Then the secondary buffer has the "new" contents to be slid down onto the screen
 * Sadly we cant use the hardware scroll as some devices with the 128x32 screens dont have the GRAM for holding both screens at once
 *
 * **This function blocks until the transition has completed or user presses button**
 */
bool LCD::scrollUp(uint8_t pos) {
  // For each line, we shuffle all bits down a row
  for (uint8_t xPos = 0; xPos < LCD_WIDTH; xPos++) {
    const uint16_t firstStripPos  = xPos;
    const uint16_t secondStripPos = firstStripPos + LCD_WIDTH;
    const uint16_t thirdStripPos  = secondStripPos + LCD_WIDTH;
    const uint16_t fourthStripPos = thirdStripPos + LCD_WIDTH;

    // We are shffling LSB's off the end and pushing bits down
    screenBuffer[fourthStripPos] = (screenBuffer[fourthStripPos] << 1) | ((screenBuffer[thirdStripPos] & 0x80) >> 7);
    screenBuffer[thirdStripPos]  = (screenBuffer[thirdStripPos] << 1) | ((screenBuffer[secondStripPos] & 0x80) >> 7);
    screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] << 1) | ((screenBuffer[firstStripPos] & 0x80) >> 7);
    screenBuffer[firstStripPos]  = (screenBuffer[firstStripPos] << 1) | ((secondFrameBuffer[fourthStripPos] & 0x80) >> 7);

    secondFrameBuffer[fourthStripPos] = (secondFrameBuffer[fourthStripPos] << 1) | ((secondFrameBuffer[thirdStripPos] & 0x80) >> 7);
    secondFrameBuffer[thirdStripPos]  = (secondFrameBuffer[thirdStripPos] << 1) | ((secondFrameBuffer[secondStripPos] & 0x80) >> 7);
    secondFrameBuffer[secondStripPos] = (secondFrameBuffer[secondStripPos] << 1) | ((secondFrameBuffer[firstStripPos] & 0x80) >> 7);
    // Finally on the bottom row; we shuffle it up ready
    secondFrameBuffer[firstStripPos] <<= 1;
  }

  return true;
}

void LCD::setRotation(bool leftHanded) {
  // TODO implement
}

void LCD::setBrightness(uint8_t brightness) {
  // TODO implement
}

void LCD::setInverse(bool inverse) {
  // TODO implement
}

void LCD::flushSecondBuffer(void) { memcpy(screenBuffer, secondFrameBuffer, sizeof(screenBuffer)); }

// Draw an area, but y must be aligned on 0/8 offset
void LCD::drawArea(int16_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+width in two strides
  if (x <= -width) {
    return; // cutoffleft
  }
  if (x > LCD_WIDTH) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = width;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + width > LCD_WIDTH) {
    visibleEnd = LCD_WIDTH - x;
  }
  uint8_t rowsDrawn = 0;
  while (height > 0) {
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      stripPointers[(y / 8) + rowsDrawn][x + xx] = ptr[xx + (rowsDrawn * width)];
    }
    height -= 8;
    rowsDrawn++;
  }
}

// Draw an area, but y must be aligned on 0/8 offset
// For data which has octets swapped in a 16-bit word.
void LCD::drawAreaSwapped(int16_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+width in two strides
  if (x <= -width) {
    return; // cutoffleft
  }
  if (x > LCD_WIDTH) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = width;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + width > LCD_WIDTH) {
    visibleEnd = LCD_WIDTH - x;
  }

  uint8_t rowsDrawn = 0;
  while (height > 0) {
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx += 2) {
      stripPointers[(y / 8) + rowsDrawn][x + xx]     = ptr[xx + 1 + (rowsDrawn * width)];
      stripPointers[(y / 8) + rowsDrawn][x + xx + 1] = ptr[xx + (rowsDrawn * width)];
    }
    height -= 8;
    rowsDrawn++;
  }
}

void LCD::fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide) {
    return; // cutoffleft
  }
  if (x > LCD_WIDTH) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > LCD_WIDTH) {
    visibleEnd = LCD_WIDTH - x;
  }

  uint8_t rowsDrawn = 0;
  while (height > 0) {
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      stripPointers[(y / 8) + rowsDrawn][x + xx] = value;
    }
    height -= 8;
    rowsDrawn++;
  }
}

void LCD::drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear) {
  // Ensure coordinates are within bounds
  if (x0 >= LCD_WIDTH || y0 >= LCD_HEIGHT || x1 >= LCD_WIDTH || y1 >= LCD_HEIGHT) {
    return;
  }

  // Calculate the height in rows
  uint8_t startRow  = y0 / 8;
  uint8_t endRow    = y1 / 8;
  uint8_t startMask = 0xFF << (y0 % 8);
  uint8_t endMask   = 0xFF >> (7 - (y1 % 8));

  for (uint8_t row = startRow; row <= endRow; row++) {
    uint8_t mask = 0xFF;
    if (row == startRow) {
      mask &= startMask;
    }
    if (row == endRow) {
      mask &= endMask;
    }

    for (uint8_t x = x0; x <= x1; x++) {
      if (clear) {
        stripPointers[row][x] &= ~mask;
      } else {
        stripPointers[row][x] |= mask;
      }
    }
  }
}

#endif // LCD_160x80
