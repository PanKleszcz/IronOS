#include "ui_drawing.hpp"
#ifdef LCD_160x80

void ui_draw_warning_undervoltage(void) {
  Display::clearScreen();
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
    Display::setCursor(0, 24);
    Display::print(translatedString(Tr->UndervoltageString), FontStyle::SMALL);
    Display::setCursor(0, 48);
    Display::print(translatedString(Tr->InputVoltageString), FontStyle::SMALL);
    Display::setCursor(96, 48);
    printVoltage();
    Display::print(SmallSymbolVolts, FontStyle::SMALL);
  } else {
    Display::setCursor(4, 24);
    Display::print(translatedString(Tr->UVLOWarningString), FontStyle::LARGE);
  }

  Display::refresh();
  GUIDelay();
  waitForButtonPress();
}
#endif
