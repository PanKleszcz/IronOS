#include "ui_drawing.hpp"
#ifdef OLED_128x32

extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_detailed(TemperatureType_t tipTemp) {
  if (isTipDisconnected()) {
    if (Display::getRotation()) {
      // in right handed mode we want to draw over the first part
      Display::drawArea(54, 0, 56, 32, disconnectedTipF);
    } else {
      Display::drawArea(0, 0, 56, 32, disconnectedTip);
    }
    if (Display::getRotation()) {
      Display::setCursor(-1, 0);
    } else {
      Display::setCursor(56, 0);
    }
    uint32_t Vlt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    Display::printNumber(Vlt / 10, 2, FontStyle::LARGE);
    Display::print(LargeSymbolDot, FontStyle::LARGE);
    Display::printNumber(Vlt % 10, 1, FontStyle::LARGE);
    if (Display::getRotation()) {
      Display::setCursor(48, 8);
    } else {
      Display::setCursor(91, 8);
    }
    Display::print(SmallSymbolVolts, FontStyle::SMALL);
  } else {
    if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (tipTemp > 55) && (xTaskGetTickCount() % 1000 < 300))) {
      // Blink temp if setting enable and temp < 55°
      // 1000 tick/sec
      // OFF 300ms ON 700ms
      ui_draw_tip_temperature(true, FontStyle::LARGE); // draw in the temp
    }
    if (Display::getRotation()) {
      Display::setCursor(6, 0);
    } else {
      Display::setCursor(73, 0); // top right
    }
    // draw set temp
    Display::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::SMALL);

    Display::printSymbolDeg(FontStyle::SMALL);

    if (Display::getRotation()) {
      Display::setCursor(0, 8);
    } else {
      Display::setCursor(67, 8); // bottom right
    }
    printVoltage(); // draw voltage then symbol (v)
    Display::print(SmallSymbolVolts, FontStyle::SMALL);
  }
}
#endif
