#include "ui_drawing.hpp"
#ifdef OLED_128x32

void printVoltage(void) {
  uint32_t volt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
  Display::printNumber(volt / 10, 2, FontStyle::SMALL);
  Display::print(SmallSymbolDot, FontStyle::SMALL);
  Display::printNumber(volt % 10, 1, FontStyle::SMALL);
}
#endif
