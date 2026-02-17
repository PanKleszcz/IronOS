#include "power.hpp"
#include "ui_drawing.hpp"
#include <OperatingModes.h>
#ifdef LCD_160x80

void ui_draw_soldering_power_status(bool boost_mode_on) {
  if (Display::getRotation()) {
    Display::setCursor(50, 0);
  } else {
    Display::setCursor(-1, 0);
  }

  ui_draw_tip_temperature(true, FontStyle::LARGE);

  if (boost_mode_on) { // Boost mode is on
    if (Display::getRotation()) {
      Display::setCursor(34, 0);
    } else {
      Display::setCursor(50, 0);
    }
    Display::print(LargeSymbolPlus, FontStyle::LARGE);
  } else {
#ifndef NO_SLEEP_MODE
    if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {
      if (Display::getRotation()) {
        Display::setCursor(32, 0);
      } else {
        Display::setCursor(47, 0);
      }
      printCountdownUntilSleep(getSleepTimeout());
    }
#endif
    if (Display::getRotation()) {
      Display::setCursor(32, 8);
    } else {
      Display::setCursor(47, 8);
    }
    Display::print(PowerSourceNames[getPowerSourceNumber()], FontStyle::SMALL, 2);
  }

  if (Display::getRotation()) {
    Display::setCursor(0, 0);
  } else {
    Display::setCursor(67, 0);
  }
  // Print wattage
  {
    uint32_t x10Watt = x10WattHistory.average();
    if (x10Watt > 999) {
      // If we exceed 99.9W we drop the decimal place to keep it all fitting
      Display::print(SmallSymbolSpace, FontStyle::SMALL);
      Display::printNumber(x10WattHistory.average() / 10, 3, FontStyle::SMALL);
    } else {
      Display::printNumber(x10WattHistory.average() / 10, 2, FontStyle::SMALL);
      Display::print(SmallSymbolDot, FontStyle::SMALL);
      Display::printNumber(x10WattHistory.average() % 10, 1, FontStyle::SMALL);
    }
    Display::print(SmallSymbolWatts, FontStyle::SMALL);
  }

  if (Display::getRotation()) {
    Display::setCursor(0, 8);
  } else {
    Display::setCursor(67, 8);
  }
  printVoltage();
  Display::print(SmallSymbolVolts, FontStyle::SMALL);
}
#endif
