#include "power.hpp"
#include "ui_drawing.hpp"
#ifdef OLED_96x16

void ui_draw_soldering_basic_status(bool boostModeOn) {
  Display::setCursor(0, 0);
  // We switch the layout direction depending on the orientation of the oled
  if (Display::getRotation()) {
    // battery
    ui_draw_power_source_icon();
    // Space out gap between battery <-> temp
    Display::print(LargeSymbolSpace, FontStyle::LARGE);
    // Draw current tip temp
    ui_draw_tip_temperature(true, FontStyle::LARGE);

    // We draw boost arrow if boosting,
    // or else gap temp <-> heat indicator
    if (boostModeOn) {
      Display::drawSymbol(2);
    } else {
      Display::print(LargeSymbolSpace, FontStyle::LARGE);
    }

    // Draw heating/cooling symbols
    Display::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
  } else {
    // Draw heating/cooling symbols
    Display::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
    // We draw boost arrow if boosting,
    // or else gap temp <-> heat indicator
    if (boostModeOn) {
      Display::drawSymbol(2);
    } else {
      Display::print(LargeSymbolSpace, FontStyle::LARGE);
    }
    // Draw current tip temp
    ui_draw_tip_temperature(true, FontStyle::LARGE);
    // Space out gap between battery <-> temp
    Display::print(LargeSymbolSpace, FontStyle::LARGE);

    ui_draw_power_source_icon();
  }
}
#endif
