#include "ui_drawing.hpp"
#ifdef OLED_96x16

void ui_draw_warning_undervoltage(void) {
  Display::clearScreen();
  Display::setCursor(0, 0);
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
    Display::print(translatedString(Tr->UndervoltageString), FontStyle::SMALL);
    Display::setCursor(0, 8);
    Display::print(translatedString(Tr->InputVoltageString), FontStyle::SMALL);
    printVoltage();
    Display::print(SmallSymbolVolts, FontStyle::SMALL);
  } else {
    Display::print(translatedString(Tr->UVLOWarningString), FontStyle::LARGE);
  }

  Display::refresh();
  GUIDelay();
  waitForButtonPress();
}
#endif
