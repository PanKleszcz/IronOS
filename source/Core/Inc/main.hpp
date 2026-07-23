#ifndef __MAIN_H
#define __MAIN_H
#include "Display.hpp"
#include "Setup.h"
#include "Types.h"
#include <stdint.h>
extern volatile TemperatureType_t currentTempTargetDegC;
extern bool                       settingsWereReset;
extern bool                       usb_pd_available;
#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

// Threads
void                startGUITask(void const *argument);
void                startPIDTask(void const *argument);
void                startMOVTask(void const *argument);
void                startPOWTask(void const *argument);
extern volatile TaskHandle_t pidTaskNotification;
extern int32_t      powerSupplyWattageLimit;
extern uint8_t      accelInit;
extern volatile TickType_t   lastMovementTime;
#ifdef __cplusplus
}
// Accelerometer type
enum class AccelType {
  Scanning  = 0,
  None      = 1,
  MMA       = 2,
  LIS       = 3,
  BMA       = 4,
  MSA       = 5,
  SC7       = 6,
  GPIO      = 7,
  LIS_CLONE = 8,
  KXTJ3     = 9,
};
extern volatile AccelType DetectedAccelerometerVersion;

#endif
#endif /* __MAIN_H */
