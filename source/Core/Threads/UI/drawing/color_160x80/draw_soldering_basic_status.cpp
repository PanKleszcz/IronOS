#include "power.hpp"
#include "ui_drawing.hpp"
#ifdef LCD_160x80

void ui_draw_soldering_basic_status(bool boostModeOn) {
  // Draw heating/cooling symbols
  Display::setCursor(4, 4);
  // We draw boost arrow if boosting
  Display::setCursor(16, 32);
  if (boostModeOn) {
    Display::drawSymbol(2);
  } else {
    Display::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
  }
  // Draw current tip temp
  Display::setCursor(44, 24);
  ui_draw_tip_temperature(true, FontStyle::LARGE);

  Display::setCursor(138, 15);
  ui_draw_power_source_icon();
}

#endif
