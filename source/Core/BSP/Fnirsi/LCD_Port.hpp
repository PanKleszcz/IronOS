/*
 * LCD_Port.cpp
 *
 *  Created on: 27 May 2026
 *      Author: OK2CM
 */

#pragma once
#include "n32l40x_tim.h"
#undef LCD

// Set backlight brightness
static inline void LCDSetBacklight(const uint8_t brightness){
  TIM_SetCmp3(TIM1, brightness > 100 ? 127 : brightness);
}
