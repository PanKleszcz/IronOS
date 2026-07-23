#include "ui_drawing.hpp"

#ifdef OLED_96x16
void ui_draw_cjc_sampling(const uint8_t num_dots) {
  Display::setCursor(0, 0);
  Display::print(translatedString(Tr->CJCCalibrating), FontStyle::SMALL);
  Display::setCursor(0, 8);
  Display::print(SmallSymbolDot, FontStyle::SMALL);
  for (uint8_t x = 0; x < num_dots; x++) {
    Display::print(SmallSymbolDot, FontStyle::SMALL);
  }
}
#endif
