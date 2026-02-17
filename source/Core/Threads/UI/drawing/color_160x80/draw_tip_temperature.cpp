#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#include "SolderingCommon.h"
#include "TipThermoModel.h"
#ifdef LCD_160x80

void ui_draw_tip_temperature(bool symbol, const FontStyle font) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  TemperatureType_t Temp = getTipTemp();

  Display::printNumber(Temp, 3, font); // Draw the tip temp out
  if (symbol) {
    // For big font, can draw nice symbols, otherwise fall back to chars
    Display::printSymbolDeg(font == FontStyle::LARGE ? FontStyle::EXTRAS : font);
  }
}
#endif
