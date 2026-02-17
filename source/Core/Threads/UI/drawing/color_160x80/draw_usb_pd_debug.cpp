#include "ui_drawing.hpp"
#ifdef LCD_160x80

void ui_draw_usb_pd_debug_state(const uint16_t vbus_sense_state, const uint8_t stateNumber) {
  Display::setCursor(0, 0);                             // Position the cursor at the 0,0 (top left)
  Display::print(SmallSymbolPDDebug, FontStyle::SMALL); // Print Title
  Display::setCursor(0, 8);                             // second line
  // Print the PD state machine
  Display::print(SmallSymbolState, FontStyle::SMALL);
  Display::print(SmallSymbolSpace, FontStyle::SMALL);
  Display::printNumber(stateNumber, 2, FontStyle::SMALL, true);
  Display::print(SmallSymbolSpace, FontStyle::SMALL);

  if (vbus_sense_state == 2) {
    Display::print(SmallSymbolNoVBus, FontStyle::SMALL);
  } else if (vbus_sense_state == 1) {
    Display::print(SmallSymbolVBus, FontStyle::SMALL);
  }
}

void ui_draw_usb_pd_debug_pdo(const uint8_t entry_num, const uint16_t min_voltage, const uint16_t max_voltage, const uint16_t current_a_x100, const uint16_t wattage) {

  Display::setCursor(0, 0);                                   // Position the cursor at the 0,0 (top left)
  Display::print(SmallSymbolPDDebug, FontStyle::SMALL);       // Print Title
  Display::setCursor(0, 8);                                   // second line
  Display::printNumber(entry_num, 2, FontStyle::SMALL, true); // print the entry number
  Display::print(SmallSymbolSpace, FontStyle::SMALL);
  if (min_voltage > 0) {
    Display::printNumber(min_voltage, 2, FontStyle::SMALL, true); // print the voltage
    Display::print(SmallSymbolMinus, FontStyle::SMALL);
  }
  Display::printNumber(max_voltage, 2, FontStyle::SMALL, true); // print the voltage
  Display::print(SmallSymbolVolts, FontStyle::SMALL);
  Display::print(SmallSymbolSpace, FontStyle::SMALL);
  if (wattage) {
    Display::printNumber(wattage, 3, FontStyle::SMALL, true); // print the current in 0.1A res
    Display::print(SmallSymbolWatts, FontStyle::SMALL);
  } else {
    Display::printNumber(current_a_x100 / 100, 2, FontStyle::SMALL, true); // print the current in 0.1A res
    Display::print(SmallSymbolDot, FontStyle::SMALL);
    Display::printNumber(current_a_x100 % 100, 2, FontStyle::SMALL, false); // print the current in 0.1A res
    Display::print(SmallSymbolAmps, FontStyle::SMALL);
  }
}
#endif
