/*
 * OLED.cpp
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "OLED.hpp"

#include "Settings.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// rendering to the buffer
uint8_t *OLED::stripPointers[4]; // Pointers to the strips to allow for buffer having extra content

// offsets in GRAM)
alignas(uint32_t) uint8_t OLED::screenBuffer[16 + (OLED_WIDTH * (OLED_HEIGHT / 8)) + 10]; // The data buffer
alignas(uint32_t) uint8_t OLED::secondFrameBuffer[16 + (OLED_WIDTH * (OLED_HEIGHT / 8)) + 10];
uint32_t OLED::displayChecksum;
uint8_t  OLED::loopCounter;
/*
 * Setup params for the OLED screen
 * http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf
 * All commands are prefixed with 0x80
 * Data packets are prefixed with 0x40
 */
I2C_CLASS::I2C_REG OLED_Setup_Array[] = {
    /**/
    {0x80,         OLED_OFF, 0}, /* Display off */
    {0x80,     OLED_DIVIDER, 0}, /* Set display clock divide ratio / osc freq */
    {0x80,             0x52, 0}, /* Divide ratios */
    {0x80,             0xA8, 0}, /* Set Multiplex Ratio */
    {0x80,  OLED_HEIGHT - 1, 0}, /* Multiplex ratio adjusts how far down the matrix it scans */
    {0x80,             0xC0, 0}, /* Set COM Scan direction */
    {0x80,             0xD3, 0}, /* Set vertical Display offset */
    {0x80,             0x00, 0}, /* 0 Offset */
    {0x80,             0x40, 0}, /* Set Display start line to 0 */
#ifdef OLED_SEGMENT_MAP_REVERSED
    {0x80,             0xA1, 0}, /* Set Segment remap to normal */
#else
    {0x80, 0xA0, 0}, /* Set Segment remap to normal */
#endif
    {0x80,             0x8D, 0}, /* Charge Pump */
    {0x80,             0x14, 0}, /* Charge Pump settings */
    {0x80,             0xDA, 0}, /* Set VCOM Pins hardware config */
    {0x80, OLED_VCOM_LAYOUT, 0}, /* Combination 0x2 or 0x12 depending on OLED model */
    {0x80,             0x81, 0}, /* Brightness */
    {0x80,             0x00, 0}, /* ^0 */
    {0x80,             0xD9, 0}, /* Set pre-charge period */
    {0x80,             0xF1, 0}, /* Pre charge period */
    {0x80,             0xDB, 0}, /* Adjust VCOMH regulator ouput */
    {0x80,             0x30, 0}, /* VCOM level */
    {0x80,             0xA4, 0}, /* Enable the display GDDR */
    {0x80,             0xA6, 0}, /* Normal display */
    {0x80,             0x20, 0}, /* Memory Mode */
    {0x80,             0x00, 0}, /* Wrap memory */
    {0x80,          OLED_ON, 0}, /* Display on */
};
// Setup based on the SSD1307 and modified for the SSD1306

const uint8_t REFRESH_COMMANDS[17] = {
    // Set display ON:
    0x80,
    0xAF, // cmd

    // Set column address:
    //  A[6:0] - Column start address = 0x20
    //  B[6:0] - Column end address = 0x7F
    0x80,
    0x21, // cmd
    0x80,
    OLED_GRAM_START, // A
    0x80,
    OLED_GRAM_END, // B

    // Set COM output scan direction (normal mode, COM0 to COM[N-1])
    0x80,
    0xC0,

    // Set page address:
    //  A[2:0] - Page start address = 0
    //  B[2:0] - Page end address = 1
    0x80,
    0x22, // cmd
    0x80,
    0x00, // A
    0x80,
    (OLED_HEIGHT / 8) - 1, // B

    // Start of data
    0x40,
};

void OLED::initialize() {
#ifdef OLED_128x32
  stripPointers[0] = &screenBuffer[FRAMEBUFFER_START];
  stripPointers[1] = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];
  stripPointers[2] = &screenBuffer[FRAMEBUFFER_START + 2 * OLED_WIDTH];
  stripPointers[3] = &screenBuffer[FRAMEBUFFER_START + 3 * OLED_WIDTH];

#else
  stripPointers[0] = &screenBuffer[FRAMEBUFFER_START];
  stripPointers[1] = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];

#endif /* OLED_128x32 */
  memcpy(&screenBuffer[0], &REFRESH_COMMANDS[0], sizeof(REFRESH_COMMANDS));
  memcpy(&secondFrameBuffer[0], &REFRESH_COMMANDS[0], sizeof(REFRESH_COMMANDS));

  // Set the display to be ON once the settings block is sent and send the
  // initialisation data to the OLED.

  for (int tries = 0; tries < 10; tries++) {
    if (I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, OLED_Setup_Array, sizeof(OLED_Setup_Array) / sizeof(OLED_Setup_Array[0]))) {
      tries = 11;
    }
  }
}

void OLED::setFramebuffer(uint8_t *buffer) {
  stripPointers[0] = &buffer[FRAMEBUFFER_START];
  stripPointers[1] = &buffer[FRAMEBUFFER_START + OLED_WIDTH];

#ifdef OLED_128x32
  stripPointers[2] = &buffer[FRAMEBUFFER_START + (2 * OLED_WIDTH)];
  stripPointers[3] = &buffer[FRAMEBUFFER_START + (3 * OLED_WIDTH)];
#endif /* OLED_128x32 */
}

// NOT USED
/**
 * Masks (removes) the scrolling indicator, i.e. clears the rightmost column
 * on the screen. This operates directly on the OLED graphics RAM, as this
 * is intended to be used before calling `OLED::transitionScrollDown()`.
 */
// void OLED::maskScrollIndicatorOnOLED() {
//   // The right-most column depends on the screen rotation, so just take
//   // it from the screen buffer which is updated by `OLED::setRotation`.
//   uint8_t rightmostColumn = screenBuffer[7];
//   uint8_t maskCommands[]  = {
//       // Set column address:
//       //  A[6:0] - Column start address = rightmost column
//       //  B[6:0] - Column end address = rightmost column
//       0x80,
//       0x21, // cmd
//       0x80,
//       rightmostColumn, // A
//       0x80,
//       rightmostColumn, // B

//       // Start of data
//       0x40,
// #ifdef OLED_128x32
//       0x00,
//       0x00,
// #endif /* OLED_128x32 */
//       // Clears two 8px strips
//       0x00,
//       0x00,
//   };
//   I2C_CLASS::Transmit(DEVICEADDR_OLED, maskCommands, sizeof(maskCommands));
// }

/**
 * Plays a transition animation between two framebuffers.
 *
 * If forward is true, this displays a forward navigation to the second framebuffer contents.
 * Otherwise a rewinding navigation animation is shown to the second framebuffer contents.
 */
bool OLED::scrollHorizontal(const bool dirForward, uint16_t progress, uint8_t offset) {
  uint8_t *stripBackPointers[4];
  stripBackPointers[0] = &secondFrameBuffer[FRAMEBUFFER_START + 0];
  stripBackPointers[1] = &secondFrameBuffer[FRAMEBUFFER_START + OLED_WIDTH];

#ifdef OLED_128x32
  stripBackPointers[2] = &secondFrameBuffer[FRAMEBUFFER_START + (OLED_WIDTH * 2)];
  stripBackPointers[3] = &secondFrameBuffer[FRAMEBUFFER_START + (OLED_WIDTH * 3)];
#endif /* OLED_128x32 */

  // When forward, current contents move to the left out.
  // Otherwise the contents move to the right out.
  uint8_t oldStart    = dirForward ? 0 : progress;
  uint8_t oldPrevious = dirForward ? progress - offset : offset;

  // Content from the second framebuffer moves in from the right (forward)
  // or from the left (not forward).
  uint8_t newStart = dirForward ? OLED_WIDTH - progress : 0;
  uint8_t newEnd   = dirForward ? 0 : OLED_WIDTH - progress;

  offset = progress;

  memmove(&stripPointers[0][oldStart], &stripPointers[0][oldPrevious], OLED_WIDTH - progress);
  memmove(&stripPointers[1][oldStart], &stripPointers[1][oldPrevious], OLED_WIDTH - progress);

#ifdef OLED_128x32
  memmove(&stripPointers[2][oldStart], &stripPointers[2][oldPrevious], OLED_WIDTH - progress);
  memmove(&stripPointers[3][oldStart], &stripPointers[3][oldPrevious], OLED_WIDTH - progress);
#endif /* OLED_128x32 */

  memmove(&stripPointers[0][newStart], &stripBackPointers[0][newEnd], progress);
  memmove(&stripPointers[1][newStart], &stripBackPointers[1][newEnd], progress);

#ifdef OLED_128x32
  memmove(&stripPointers[2][newStart], &stripBackPointers[2][newEnd], progress);
  memmove(&stripPointers[3][newStart], &stripBackPointers[3][newEnd], progress);
#endif /* OLED_128x32 */

#ifdef OLED_128x32
  if (loopCounter++ % 2 == 0) {
    return true;
  }
  return false;
#else
  return true;
#endif
}

void OLED::useSecondaryFramebuffer(bool useSecondary) {
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
bool OLED::scrollDown(uint8_t pos) {

  // For each line, we shuffle all bits up a row
  for (uint8_t xPos = 0; xPos < OLED_WIDTH; xPos++) {
    const uint16_t firstStripPos  = FRAMEBUFFER_START + xPos;
    const uint16_t secondStripPos = firstStripPos + OLED_WIDTH;
#ifdef OLED_128x32
    // For 32 pixel high OLED's we have four strips to tailchain
    const uint16_t thirdStripPos  = secondStripPos + OLED_WIDTH;
    const uint16_t fourthStripPos = thirdStripPos + OLED_WIDTH;
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
#else
    // Move the LSB off the first strip, and pop MSB from second strip onto the first strip
    screenBuffer[firstStripPos] = (screenBuffer[firstStripPos] >> 1) | ((screenBuffer[secondStripPos] & 0x01) << 7);
    // Now shuffle off the second strip MSB, and replace it with the LSB of the secondary buffer
    screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] >> 1) | ((secondFrameBuffer[firstStripPos] & 0x01) << 7);
    // Finally, do the shuffle on the second frame buffer
    secondFrameBuffer[firstStripPos] = (secondFrameBuffer[firstStripPos] >> 1) | ((secondFrameBuffer[secondStripPos] & 0x01) << 7);
    // Finally on the bottom row; we shuffle it up ready
    secondFrameBuffer[secondStripPos] >>= 1;
#endif /* OLED_128x32 */
  }
#ifdef OLED_128x32
  if (loopCounter++ % 2 == 0) {
    return true;
  }
  return false;
#else
  return true;
#endif
}
/**
 * This assumes that the current display output buffer has the current on screen contents
 * Then the secondary buffer has the "new" contents to be slid down onto the screen
 * Sadly we cant use the hardware scroll as some devices with the 128x32 screens dont have the GRAM for holding both screens at once
 *
 * **This function blocks until the transition has completed or user presses button**
 */
bool OLED::scrollUp(uint8_t pos) {
  // For each line, we shuffle all bits down a row
  for (uint8_t xPos = 0; xPos < OLED_WIDTH; xPos++) {
    const uint16_t firstStripPos  = FRAMEBUFFER_START + xPos;
    const uint16_t secondStripPos = firstStripPos + OLED_WIDTH;
#ifdef OLED_128x32
    // For 32 pixel high OLED's we have four strips to tailchain
    const uint16_t thirdStripPos  = secondStripPos + OLED_WIDTH;
    const uint16_t fourthStripPos = thirdStripPos + OLED_WIDTH;
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
#else
    // We pop the LSB off the bottom row, and replace the MSB in that byte with the LSB of the row above
    screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] << 1) | ((screenBuffer[firstStripPos] & 0x80) >> 7);
    // Move the LSB off the first strip, and pop MSB from second strip onto the first strip
    screenBuffer[firstStripPos] = (screenBuffer[firstStripPos] << 1) | ((secondFrameBuffer[secondStripPos] & 0x80) >> 7);

    // Finally, do the shuffle on the second frame buffer
    secondFrameBuffer[secondStripPos] = (secondFrameBuffer[secondStripPos] << 1) | ((secondFrameBuffer[firstStripPos] & 0x80) >> 7);
    // Finally on the bottom row; we shuffle it up ready
    secondFrameBuffer[firstStripPos] <<= 1;
#endif /* OLED_128x32 */
  }

#ifdef OLED_128x32
  if (loopCounter++ % 2 == 0) {
    return true;
  }
  return false;
#else
  return true;
#endif
}

void OLED::setRotation(bool leftHanded) {
#ifdef OLED_SEGMENT_MAP_REVERSED
  if (!leftHanded) {
    OLED_Setup_Array[9].val = 0xA1;
  } else {
    OLED_Setup_Array[9].val = 0xA0;
  }
#else
  if (leftHanded) {
    OLED_Setup_Array[9].val = 0xA1;
  } else {
    OLED_Setup_Array[9].val = 0xA0;
  }
#endif /* OLED_SEGMENT_MAP_REVERSED */
  // send command struct again with changes
  if (leftHanded) {
    OLED_Setup_Array[5].val = 0xC8; // c1?
  } else {
    OLED_Setup_Array[5].val = 0xC0;
  }
  I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, OLED_Setup_Array, sizeof(OLED_Setup_Array) / sizeof(OLED_Setup_Array[0]));
  osDelay(TICKS_10MS);

  screenBuffer[5] = !leftHanded ? OLED_GRAM_START_FLIP : OLED_GRAM_START; // display is shifted by 32 in left handed
                                                                          // mode as driver ram is 128 wide
  screenBuffer[7] = !leftHanded ? OLED_GRAM_END_FLIP : OLED_GRAM_END;     // End address of the ram segment we are writing to (96 wide)
  screenBuffer[9] = !leftHanded ? 0xC8 : 0xC0;
  // Force a screen refresh
  const int len = FRAMEBUFFER_START + (OLED_WIDTH * (OLED_HEIGHT / 8));
  I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, len);
  osDelay(TICKS_10MS);
  checkDisplayBufferChecksum();
}

void OLED::setBrightness(uint8_t contrast) {
  if (OLED_Setup_Array[15].val != contrast) {
    OLED_Setup_Array[15].val = contrast;
    I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, &OLED_Setup_Array[14], 2);
  }
}

void OLED::setInverse(bool inverse) {
  uint8_t normalInverseCmd = inverse ? 0xA7 : 0xA6;
  if (OLED_Setup_Array[21].val != normalInverseCmd) {
    OLED_Setup_Array[21].val = normalInverseCmd;
    I2C_CLASS::I2C_RegisterWrite(DEVICEADDR_OLED, 0x80, normalInverseCmd);
  }
}

void OLED::flushSecondBuffer(void) { memcpy(screenBuffer + FRAMEBUFFER_START, secondFrameBuffer + FRAMEBUFFER_START, sizeof(screenBuffer) - FRAMEBUFFER_START); }

// Draw an area, but y must be aligned on 0/8 offset
void OLED::drawArea(int16_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+width in two strides
  if (x <= -width) {
    return; // cutoffleft
  }
  if (x > OLED_WIDTH) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = width;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + width > OLED_WIDTH) {
    visibleEnd = OLED_WIDTH - x;
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
void OLED::drawAreaSwapped(int16_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+width in two strides
  if (x <= -width) {
    return; // cutoffleft
  }
  if (x > OLED_WIDTH) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = width;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + width > OLED_WIDTH) {
    visibleEnd = OLED_WIDTH - x;
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

void OLED::fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide) {
    return; // cutoffleft
  }
  if (x > OLED_WIDTH) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > OLED_WIDTH) {
    visibleEnd = OLED_WIDTH - x;
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

void OLED::drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear) {
  // Ensure coordinates are within bounds
  if (x0 >= OLED_WIDTH || y0 >= OLED_HEIGHT || x1 >= OLED_WIDTH || y1 >= OLED_HEIGHT) {
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
