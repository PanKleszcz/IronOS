#include "ui_drawing.hpp"
#ifdef OLED_96x16

void ui_draw_temperature_change(void) {

  Display::setCursor(0, 0);
  if (Display::getRotation()) {
    Display::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
  } else {
    Display::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);
  }

  Display::print(LargeSymbolSpace, FontStyle::LARGE);
  Display::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::LARGE);
  Display::printSymbolDeg(FontStyle::EXTRAS);
  Display::print(LargeSymbolSpace, FontStyle::LARGE);
  if (Display::getRotation()) {
    Display::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);
  } else {
    Display::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
  }
}
#endif
