#include "OperatingModes.h"
#include "TipThermoModel.h"
#include "power.hpp"
#include "ui_drawing.hpp"
#ifdef LCD_160x80

namespace {
// Palette indices, matching LCD::palette2bpp (index 0 must stay the background).
constexpr uint8_t kBg    = 0;
constexpr uint8_t kInk   = 1;
constexpr uint8_t kEmber = 2;
constexpr uint8_t kCool  = 3;

constexpr float kPi        = 3.14159265f;
constexpr float kRingStart = kPi * 0.72f; // Leaves a gap at the bottom of the ring, same as the mockup.
constexpr float kRingEnd   = kPi * 2.28f;

constexpr uint8_t kGaugeCx = 40, kGaugeCy = 44, kGaugeR = 28, kGaugeThickness = 6;
constexpr uint8_t kStatX = 96; // Right-hand stats column, matching the x=96 convention already
                                // used across the rest of this codebase's soldering/idle screens.

// Draws the temperature ring + big current-temp number + "SET nnn" line shared by idle/soldering.
void drawGaugeCore(TemperatureType_t current, TemperatureType_t target, uint8_t ringColor) {
  float frac = (target > 0) ? (float)current / (float)target : 0.0f;
  if (frac > 1.0f) {
    frac = 1.0f;
  } else if (frac < 0.0f) {
    frac = 0.0f;
  }
  Display::drawRingColor(kGaugeCx, kGaugeCy, kGaugeR, kGaugeThickness, ringColor, kRingStart, kRingStart + (kRingEnd - kRingStart) * frac);
  Display::drawTickColor(kGaugeCx, kGaugeCy, kRingEnd, kGaugeR - 5, kGaugeR + 5, kInk);

  Display::printNumberColor(current, 3, kGaugeCx - (3 * FONT_LARGE_WIDTH) / 2, kGaugeCy - FONT_LARGE_HEIGHT / 2, FontStyle::LARGE, kInk);

  uint8_t x = 6;
  Display::printColor(SmallSymbolSetPoint, x, 60, FontStyle::SMALL, kInk);
  x += 3 * FONT_SMALL_WIDTH + 4;
  Display::printNumberColor(target, 3, x, 60, FontStyle::SMALL, kInk);
}

// Draws "NN.NV" starting at (x,y), advancing a running cursor -- avoids hand-computed offsets.
void drawVoltageColor(uint8_t x, uint8_t y, uint8_t color) {
  uint32_t volt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
  Display::printNumberColor(volt / 10, 2, x, y, FontStyle::SMALL, color);
  x += 2 * FONT_SMALL_WIDTH;
  Display::printColor(SmallSymbolDot, x, y, FontStyle::SMALL, color);
  x += FONT_SMALL_WIDTH;
  Display::printNumberColor(volt % 10, 1, x, y, FontStyle::SMALL, color);
  x += FONT_SMALL_WIDTH;
  Display::printColor(SmallSymbolVolts, x, y, FontStyle::SMALL, color);
}
} // namespace

void ui_draw_home_gauge_idle(TemperatureType_t tipTemp) {
  if (isTipDisconnected()) {
    return; // Background is already cleared for this frame; leave the gauge area blank.
  }
  const TemperatureType_t target = getSettingValue(SettingsOptions::SolderingTemp);
  drawGaugeCore(tipTemp, target, kCool);

  Display::printColor(PowerSourceNames[getPowerSourceNumber()], kStatX, 2, FontStyle::SMALL, kInk, 4);
  drawVoltageColor(kStatX, 20, kInk);
}

void ui_draw_home_gauge_soldering(bool boostModeOn) {
  const TemperatureType_t current = TipThermoModel::getTipInC();
  const TemperatureType_t target  = getSettingValue(boostModeOn ? SettingsOptions::BoostTemp : SettingsOptions::SolderingTemp);
  drawGaugeCore(current, target, kEmber);

  if (boostModeOn) {
    Display::printColor(SmallSymbolBoostActive, 6, 72, FontStyle::SMALL, kEmber);
  }

  Display::printColor(PowerSourceNames[getPowerSourceNumber()], kStatX, 2, FontStyle::SMALL, kInk, 4);
  drawVoltageColor(kStatX, 20, kInk);

  const uint32_t x10Watt = x10WattHistory.average();
  uint8_t        wx      = kStatX;
  Display::printNumberColor(x10Watt / 10, 2, wx, 38, FontStyle::SMALL, kInk);
  wx += 2 * FONT_SMALL_WIDTH;
  Display::printColor(SmallSymbolDot, wx, 38, FontStyle::SMALL, kInk);
  wx += FONT_SMALL_WIDTH;
  Display::printNumberColor(x10Watt % 10, 1, wx, 38, FontStyle::SMALL, kInk);
  wx += FONT_SMALL_WIDTH;
  Display::printColor(SmallSymbolWatts, wx, 38, FontStyle::SMALL, kInk);

  const float wattFrac = (x10Watt > (uint32_t)HARDWARE_MAX_WATTAGE_X10) ? 1.0f : (float)x10Watt / (float)HARDWARE_MAX_WATTAGE_X10;
  Display::fillRectColor(kStatX, 70, (uint8_t)(58.0f * wattFrac), 6, kEmber);

#ifndef NO_SLEEP_MODE
  if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {
    // Mirrors printCountdownUntilSleep()'s math (printSleepCountdown.cpp), blitting to an explicit
    // (x,y) in colour instead of via the shared cursor / 1bpp path.
    TickType_t lastEventTime = lastButtonTime < lastMovementTime ? lastMovementTime : lastButtonTime;
    TickType_t downCount     = getSleepTimeout() - xTaskGetTickCount() + lastEventTime;
    uint8_t    cx            = kStatX;
    if (downCount > (99 * TICKS_SECOND)) {
      Display::printNumberColor(downCount / 60000 + 1, 2, cx, 56, FontStyle::SMALL, kInk);
      cx += 2 * FONT_SMALL_WIDTH;
      Display::printColor(SmallSymbolMinutes, cx, 56, FontStyle::SMALL, kInk);
    } else {
      Display::printNumberColor(downCount / 1000 + 1, 2, cx, 56, FontStyle::SMALL, kInk);
      cx += 2 * FONT_SMALL_WIDTH;
      Display::printColor(SmallSymbolSeconds, cx, 56, FontStyle::SMALL, kInk);
    }
  }
#endif
}

void ui_draw_home_gauge_sleep(TemperatureType_t tipTemp) {
  Display::printColor(LargeSymbolSleep, 32, 2, FontStyle::LARGE, kCool);
  Display::printNumberColor(tipTemp, 3, 44, 36, FontStyle::LARGE, kInk);
  drawVoltageColor(4, 70, kInk);
}

#endif
