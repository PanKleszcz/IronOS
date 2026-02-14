/*
 * Irqs.h
 *
 *  Created on: 30 May 2020
 *      Author: Ralim, MrTick
 */

#ifndef BSP_MINIWARE_IRQ_H_
#define BSP_MINIWARE_IRQ_H_

#include "BSP.h"
#include "Setup.h"
#include "main.hpp"

#ifdef __cplusplus
extern "C" {
#endif
// void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc);
// void HAL_GPIO_EXTI_Callback(uint16_t);


void TIM2_IRQHandler(void);
void ADC_IRQHandler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif
#endif /* BSP_MINIWARE_IRQ_H_ */
