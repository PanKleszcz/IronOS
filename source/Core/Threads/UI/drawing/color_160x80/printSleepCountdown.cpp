#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
#ifdef LCD_160x80
extern TickType_t lastMovementTime;
#ifndef NO_SLEEP_MODE
void printCountdownUntilSleep(int sleepThres) {
  /*
   * Print seconds or minutes (if > 99 seconds) until sleep
   * mode is triggered.
   */
  TickType_t lastEventTime = lastButtonTime < lastMovementTime ? lastMovementTime : lastButtonTime;
  TickType_t downCount     = sleepThres - xTaskGetTickCount() + lastEventTime;
  if (downCount > (99 * TICKS_SECOND)) {
    Display::printNumber(downCount / 60000 + 1, 2, FontStyle::SMALL);
    Display::print(SmallSymbolMinutes, FontStyle::SMALL);
  } else {
    Display::printNumber(downCount / 1000 + 1, 2, FontStyle::SMALL);
    Display::print(SmallSymbolSeconds, FontStyle::SMALL);
  }
}
#endif
#endif
