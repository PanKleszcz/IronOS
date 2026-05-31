#include "ui_drawing.hpp"

#ifdef LCD_160x80
void ui_draw_soldering_detailed_sleep(TemperatureType_t tipTemp) {

  Display::clearScreen();
  Display::setCursor(0, 8);
  Display::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
  Display::setCursor(0, 32);
  Display::print(translatedString(Tr->SleepingTipAdvancedString), FontStyle::SMALL);
  Display::printNumber(tipTemp, 3, FontStyle::SMALL);
  Display::printSymbolDeg(FontStyle::SMALL);

  Display::setCursor(0, 48);
  Display::print(translatedString(Tr->InputVoltageString), FontStyle::SMALL);
  Display::setCursor(96, 48);
  printVoltage();
  Display::print(SmallSymbolVolts, FontStyle::SMALL);

  Display::refresh();
}

void ui_draw_soldering_basic_sleep(TemperatureType_t tipTemp) {

  Display::clearScreen();
  Display::setCursor(44, 0);
  Display::print(LargeSymbolSleep, FontStyle::LARGE);
  Display::setCursor(44, 40);
  Display::printNumber(tipTemp, 3, FontStyle::LARGE);
  Display::printSymbolDeg(FontStyle::EXTRAS);

  Display::refresh();
}
#endif
