#include "ui_drawing.hpp"
#ifdef OLED_96x16

void ui_draw_soldering_profile_advanced(TemperatureType_t tipTemp, TemperatureType_t profileCurrentTargetTemp, uint32_t phaseElapsedSeconds, uint32_t phase, const uint32_t phaseTimeGoal) {
  // print temperature
  if (Display::getRotation()) {
    Display::setCursor(48, 0);
  } else {
    Display::setCursor(0, 0);
  }

  Display::printNumber(tipTemp, 3, FontStyle::SMALL);
  Display::print(SmallSymbolSlash, FontStyle::SMALL);
  Display::printNumber(profileCurrentTargetTemp, 3, FontStyle::SMALL);

  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    Display::print(SmallSymbolDegF, FontStyle::SMALL);
  } else {
    Display::print(SmallSymbolDegC, FontStyle::SMALL);
  }

  // print phase
  if (phase > 0 && phase <= getSettingValue(SettingsOptions::ProfilePhases)) {
    if (Display::getRotation()) {
      Display::setCursor(36, 0);
    } else {
      Display::setCursor(55, 0);
    }
    Display::printNumber(phase, 1, FontStyle::SMALL);
  }

  // print time progress / preheat / cooldown
  if (Display::getRotation()) {
    Display::setCursor(42, 8);
  } else {
    Display::setCursor(0, 8);
  }

  if (phase == 0) {
    Display::print(translatedString(Tr->ProfilePreheatString), FontStyle::SMALL);
  } else if (phase > getSettingValue(SettingsOptions::ProfilePhases)) {
    Display::print(translatedString(Tr->ProfileCooldownString), FontStyle::SMALL);
  } else {
    Display::printNumber(phaseElapsedSeconds / 60, 1, FontStyle::SMALL);
    Display::print(SmallSymbolColon, FontStyle::SMALL);
    Display::printNumber(phaseElapsedSeconds % 60, 2, FontStyle::SMALL, false);

    Display::print(SmallSymbolSlash, FontStyle::SMALL);

    // blink if we can't keep up with the time goal
    if (phaseElapsedSeconds < phaseTimeGoal + 2 || (xTaskGetTickCount() / TICKS_SECOND) % 2 == 0) {
      Display::printNumber(phaseTimeGoal / 60, 1, FontStyle::SMALL);
      Display::print(SmallSymbolColon, FontStyle::SMALL);
      Display::printNumber(phaseTimeGoal % 60, 2, FontStyle::SMALL, false);
    }
  }
}

#endif
