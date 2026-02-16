#include "ui_drawing.hpp"

#ifdef OLED_128x32

extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_simplified(TemperatureType_t tipTemp) {
  bool tempOnDisplay          = false;
  bool tipDisconnectedDisplay = false;
  if (Display::getRotation()) {
    Display::drawArea(68, 0, 56, 32, buttonAF);
    Display::drawArea(12, 0, 56, 32, buttonBF);
    Display::setCursor(0, 0);
    ui_draw_power_source_icon();
  } else {
    Display::drawArea(0, 0, 56, 32, buttonA);  // Needs to be flipped so button ends up
    Display::drawArea(58, 0, 56, 32, buttonB); // on right side of screen
    Display::setCursor(116, 0);
    ui_draw_power_source_icon();
  }
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
    // draw temp over the start soldering button
    // Location changes on screen rotation
    if (Display::getRotation()) {
      // in right handed mode we want to draw over the first part
      Display::fillArea(68, 0, 56, 32, 0); // clear the area for the temp
      Display::setCursor(56, 0);
    } else {
      Display::fillArea(0, 0, 56, 32, 0); // clear the area
      Display::setCursor(0, 0);
    }
    // If we have a tip connected draw the temp, if not we leave it blank
    if (!tipDisconnectedDisplay) {
      // draw in the temp
      if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (xTaskGetTickCount() % 1000 < 300))) {
        ui_draw_tip_temperature(false, FontStyle::LARGE); // draw in the temp
      }
    } else {
      // Draw in missing tip symbol
      if (Display::getRotation()) {
        // in right handed mode we want to draw over the first part
        Display::drawArea(54, 0, 56, 32, disconnectedTipF);
      } else {
        Display::drawArea(0, 0, 56, 32, disconnectedTip);
      }
    }
  }
}

#endif
