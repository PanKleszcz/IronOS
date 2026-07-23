#include "ui_drawing.hpp"

#ifdef LCD_160x80

extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_simplified(TemperatureType_t tipTemp) {
  bool tempOnDisplay          = false;
  bool tipDisconnectedDisplay = false;
  Display::drawArea(24, 8, 56, 32, buttonA);
  Display::drawArea(80, 8, 56, 32, buttonB);
  Display::setCursor(138, 15);
  ui_draw_power_source_icon();
  tipDisconnectedDisplay = false;
  if (tipTemp > 55) {
    tempOnDisplay = true;
  } else if (tipTemp < 45) {
    tempOnDisplay = false;
  }
  if (isTipDisconnected()) {
    tempOnDisplay          = false;
    tipDisconnectedDisplay = true;
  }
  if (tempOnDisplay || tipDisconnectedDisplay) {
    // If we have a tip connected draw the temp, if not we leave it blank
    if (!tipDisconnectedDisplay) {
      // draw in the temp
      if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (xTaskGetTickCount() % 1000 < 300))) {
        Display::setCursor(42, 48);
        ui_draw_tip_temperature(true, FontStyle::LARGE); // draw in the temp
      }
    } else {
      // Draw in missing tip symbol
      Display::drawArea(52, 48, 56, 32, disconnectedTip);
    }
  }
}

#endif
