#include "BootLogo.h"
#include "BSP.h"
#include "Buttons.hpp"
#include "Display.hpp"
#include "Settings.h"
#include "cmsis_os.h"

#define LOGO_PAGE_LENGTH 1024

void delay() {
  if (getSettingValue(SettingsOptions::LOGOTime) >= logoMode_t::ONETIME) {
    waitForButtonPress();
  } else {
    waitForButtonPressOrTimeout(TICKS_SECOND * getSettingValue(SettingsOptions::LOGOTime));
  }
}

void BootLogo::handleShowingLogo(const uint8_t *ptrLogoArea) {
  Display::clearScreen();
  // Read the first few bytes and figure out what format we are looking at
  if (OLD_LOGO_HEADER_VALUE == *(reinterpret_cast<const uint32_t *>(ptrLogoArea))) {
    showOldFormat(ptrLogoArea);
  } else if (ptrLogoArea[0] == 0xAA) {
    showNewFormat(ptrLogoArea + 1);
  }

  Display::clearScreen();
}

void BootLogo::showOldFormat(const uint8_t *ptrLogoArea) {
#ifdef OLED_128x32
  // Draw in middle
  Display::drawAreaSwapped(16, 8, 96, 16, (uint8_t *)(ptrLogoArea + 4));

#else
  Display::drawAreaSwapped(0, 0, 96, 16, (uint8_t *)(ptrLogoArea + 4));

#endif
  Display::refresh();
  // Delay here with static logo until a button is pressed or its been the amount of seconds set by the user
  delay();
}

void BootLogo::showNewFormat(const uint8_t *ptrLogoArea) {
  if (getSettingValue(SettingsOptions::LOGOTime) == logoMode_t::SKIP) {
    return;
  }

  // New logo format (a) fixes long standing byte swap quirk and (b) supports animation
  uint8_t interFrameDelay = ptrLogoArea[0];
  Display::clearScreen();

  // Now draw in the frames
  int position = 1;
  while (getButtonState() == BUTTON_NONE) {
    int len = (showNewFrame(ptrLogoArea + position));
    Display::refresh();
    position += len;

    if (interFrameDelay) {
      osDelay(interFrameDelay * 4);
    }

    // 1024 less the header type byte and the inter-frame-delay
    if (getSettingValue(SettingsOptions::LOGOTime) && (position >= 1022 || len == 0)) {
      // Animated logo stops here ...
      if (getSettingValue(SettingsOptions::LOGOTime) == logoMode_t::INFINITY) {
        // ... but if it's infinite logo setting then keep it rolling over again until a button is pressed
        osDelay(4 * TICKS_100MS);
        Display::clearScreen();
        position = 1;
        continue;
      }
    } else {
      // Animation in progress so jumping to the next frame
      continue;
    }

    // Static logo case ends up right here, so delay until a button is pressed or its been the amount of seconds set by the user
    delay();
    return;
  }
}

int BootLogo::showNewFrame(const uint8_t *ptrLogoArea) {
  uint8_t length = ptrLogoArea[0];
  switch (length) {
  case 0:
    // End
    return 0;
    break;
  case 0xFE:
    return 1;
    break;
  case 0xFF:
// Full frame update
#ifdef OLED_128x32
    Display::drawArea(16, 8, 96, 16, ptrLogoArea + 1);
#else
    Display::drawArea(0, 0, 96, 16, ptrLogoArea + 1);
#endif
    length = 96;
    break;
  default:
    length /= 2;
    // Draw length patches
    for (int p = 0; p < length; p++) {
      uint8_t index = ptrLogoArea[1 + (p * 2)];
      uint8_t value = ptrLogoArea[2 + (p * 2)];
#ifdef OLED_128x32
      Display::drawArea(16 + (index % 96), index >= 96 ? 16 : 8, 1, 8, &value);
#else
      Display::drawArea(index % 96, index >= 96 ? 8 : 0, 1, 8, &value);
#endif
    }
  }

  Display::refresh();
  return (length * 2) + 1;
}
