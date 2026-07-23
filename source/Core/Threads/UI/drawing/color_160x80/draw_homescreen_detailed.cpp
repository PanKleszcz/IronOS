#include "ui_drawing.hpp"
#include <OperatingModes.h>
#ifdef LCD_160x80

extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_detailed(TemperatureType_t tipTemp) {
  // Print power source
  Display::setCursor(132, 0);
  Display::print(PowerSourceNames[getPowerSourceNumber()], FontStyle::SMALL, 2);
  // Print voltage
  Display::setCursor(96, 16);
  printVoltage();
  Display::print(SmallSymbolVolts, FontStyle::SMALL);
  if (isTipDisconnected()) {
    // Draw in missing tip symbol
    Display::drawArea(52, 32, 56, 32, disconnectedTip);
  } else {
    // Print tip temp
    Display::setCursor(4, 24);
    if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (tipTemp > 55) && (xTaskGetTickCount() % 1000 < 300))) {
      // Blink temp if setting enable and temp < 55°
      // 1000 tick/sec
      // OFF 300ms ON 700ms
      ui_draw_tip_temperature(true, FontStyle::LARGE); // draw in the temp
    }
    // Print set temp
    Display::setCursor(40, 0);
    Display::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::SMALL);
    Display::printSymbolDeg(FontStyle::EXTRAS);
  }
}
#endif
