#include "ui_drawing.hpp"
#ifdef OLED_96x16

void ui_draw_soldering_detailed_sleep(TemperatureType_t tipTemp) {

  Display::clearScreen();
  Display::setCursor(0, 0);
  Display::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
  Display::setCursor(0, 8);
  Display::print(translatedString(Tr->SleepingTipAdvancedString), FontStyle::SMALL);
  Display::printNumber(tipTemp, 3, FontStyle::SMALL);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    Display::print(SmallSymbolDegF, FontStyle::SMALL);
  } else {
    Display::print(SmallSymbolDegC, FontStyle::SMALL);
  }

  Display::print(SmallSymbolSpace, FontStyle::SMALL);
  printVoltage();
  Display::print(SmallSymbolVolts, FontStyle::SMALL);

  Display::refresh();
}

void ui_draw_soldering_basic_sleep(TemperatureType_t tipTemp) {

  Display::clearScreen();
  Display::setCursor(0, 0);

  Display::print(LargeSymbolSleep, FontStyle::LARGE);
  Display::printNumber(tipTemp, 3, FontStyle::LARGE);
  Display::printSymbolDeg(FontStyle::EXTRAS);

  Display::refresh();
}
#endif
