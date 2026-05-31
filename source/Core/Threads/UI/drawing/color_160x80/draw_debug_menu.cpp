#include "OperatingModes.h"
#include "TipThermoModel.h"
#include "main.hpp"
#include "ui_drawing.hpp"

#ifdef LCD_160x80
extern osThreadId GUITaskHandle;
extern osThreadId MOVTaskHandle;
extern osThreadId PIDTaskHandle;

void ui_draw_debug_menu(const uint8_t item_number) {
  Display::setCursor(0, 0);                                   // Position the cursor at the 0,0 (top left)
  Display::print(SmallSymbolVersionNumber, FontStyle::SMALL); // Print version number
  Display::setCursor(0, 24);                                  // second line
  Display::print(DebugMenu[item_number], FontStyle::SMALL);
  switch (item_number) {
  case 0: // Build Date
    break;
  case 1: // Device ID
  {
    uint64_t id = getDeviceID();
#ifdef DEVICE_HAS_VALIDATION_CODE
    // If device has validation code; then we want to take over both lines of the screen
    Display::clearScreen();   // Ensure the buffer starts clean
    Display::setCursor(0, 0); // Position the cursor at the 0,0 (top left)
    Display::print(DebugMenu[item_number], FontStyle::SMALL);
    Display::drawHex(getDeviceValidation(), FontStyle::SMALL, 8);
    Display::setCursor(0, 16); // second line
#endif
    Display::drawHex((uint32_t)(id >> 32), FontStyle::SMALL, 8);
    Display::setCursor(36, 40);
    Display::drawHex((uint32_t)(id & 0xFFFFFFFF), FontStyle::SMALL, 8);
  } break;
  case 2: // ACC Type
    Display::print(AccelTypeNames[(int)DetectedAccelerometerVersion], FontStyle::SMALL);
    break;
  case 3: // Power Negotiation Status
    Display::print(PowerSourceNames[getPowerSourceNumber()], FontStyle::SMALL);
    break;
  case 4: // Input Voltage
    printVoltage();
    break;
  case 5: // Temp in °C
    Display::printNumber(TipThermoModel::getTipInC(), 6, FontStyle::SMALL);
    break;
  case 6: // Handle Temp in °C
    Display::printNumber(getHandleTemperature(0) / 10, 6, FontStyle::SMALL);
    Display::print(SmallSymbolDot, FontStyle::SMALL);
    Display::printNumber(getHandleTemperature(0) % 10, 1, FontStyle::SMALL);
    break;
  case 7: // Max Temp Limit in °C
    Display::printNumber(TipThermoModel::getTipMaxInC(), 6, FontStyle::SMALL);
    break;
  case 8: // System Uptime
    Display::printNumber(xTaskGetTickCount() / TICKS_100MS, 8, FontStyle::SMALL);
    break;
  case 9: // Movement Timestamp
    Display::printNumber(lastMovementTime / TICKS_100MS, 8, FontStyle::SMALL);
    break;
  case 10:                                                                 // Tip Resistance in Ω
    Display::printNumber(getTipResistanceX10() / 10, 6, FontStyle::SMALL); // large to pad over so that we cover ID left overs
    Display::print(SmallSymbolDot, FontStyle::SMALL);
    Display::printNumber(getTipResistanceX10() % 10, 1, FontStyle::SMALL);
    break;
  case 11: // Raw Tip in µV
    Display::printNumber(TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true), 8, FontStyle::SMALL);
    break;
  case 12: // Tip Cold Junction Compensation Offset in µV
    Display::printNumber(getSettingValue(SettingsOptions::CalibrationOffset), 8, FontStyle::SMALL);
    break;
  case 13: // High Water Mark for GUI
    Display::printNumber(uxTaskGetStackHighWaterMark(GUITaskHandle), 8, FontStyle::SMALL);
    break;
  case 14: // High Water Mark for Movement Task
    Display::printNumber(uxTaskGetStackHighWaterMark(MOVTaskHandle), 8, FontStyle::SMALL);
    break;
  case 15: // High Water Mark for PID Task
    Display::printNumber(uxTaskGetStackHighWaterMark(PIDTaskHandle), 8, FontStyle::SMALL);
    break;
    break;
#ifdef HALL_SENSOR
  case 16: // Raw Hall Effect Value
  {
    int16_t hallEffectStrength = getRawHallEffect();
    if (hallEffectStrength < 0) {
      hallEffectStrength = -hallEffectStrength;
    }
    Display::printNumber(hallEffectStrength, 6, FontStyle::SMALL);
  } break;
#endif

  default:
    break;
  }
}
#endif
