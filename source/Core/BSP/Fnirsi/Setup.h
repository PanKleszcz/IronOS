#pragma once
#include "configuration.h"
#ifdef __cplusplus
extern "C" {
#endif

#define ADC_CHANNELS 4
#define ADC_FILTER_LEN 8
#define ADC_OVERSAMPLING 8
#define ADC_SAMPLES    (ADC_CHANNELS*ADC_OVERSAMPLING)
extern uint16_t ADCReadings[ADC_SAMPLES]; // Used to store the adc readings for the handle cold junction temp

void hwInit(void);
uint16_t getADCVin(uint8_t sample);
uint16_t getADCHandleTemp(uint8_t sample);

#ifdef __cplusplus
}
#endif
