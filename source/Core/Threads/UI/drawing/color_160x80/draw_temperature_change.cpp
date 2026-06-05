#include "ui_drawing.hpp"

#ifdef LCD_160x80
void ui_draw_temperature_change(void) {
  Display::setCursor(2, 24);
  Display::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);

  Display::print(SmallSymbolSpace, FontStyle::SMALL);
  Display::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::LARGE);
  Display::printSymbolDeg(FontStyle::EXTRAS);
  Display::print(SmallSymbolSpace, FontStyle::SMALL);
  Display::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
}
#endif
