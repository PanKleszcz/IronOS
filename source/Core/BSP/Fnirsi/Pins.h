/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim, MrTick
 */

#pragma once
#include "configuration.h"

#ifdef MODEL_HS02

#define LCD_RESET_Pin             GPIO_PIN_3
#define LCD_CS_Pin                GPIO_PIN_4
#define LCD_SCK_Pin               GPIO_PIN_5
#define LCD_CMD_Pin               GPIO_PIN_6
#define LCD_MOSI_Pin              GPIO_PIN_7
#define LCD_Port                  GPIOA

#define LCD_BL_Pin                GPIO_PIN_14
#define LCD_BL_Port               GPIOD

#define BUTTON_OK_Pin             GPIO_PIN_3
#define BUTTON_UP_Pin             GPIO_PIN_4
#define BUTTON_DOWN_Pin           GPIO_PIN_5
#define BUTTON_Port               GPIOB

#define ADC_TEMP_Pin              GPIO_PIN_1
#define ADC_TEMP_Port             GPIOA
#define ADC_3V3_Pin               GPIO_PIN_2
#define ADC_3V3_Port              GPIOA
#define ADC_VBUS_Pin              GPIO_PIN_0
#define ADC_VBUS_Port             GPIOB
#define ADC_CURR_Pin              GPIO_PIN_1
#define ADC_CURR_Port             GPIOB

#define I2C_SCL_Pin               GPIO_PIN_9
#define I2C_SDA_Pin               GPIO_PIN_10
#define I2C_Port                  GPIOA

#define ADC_TEMP_Channel          ADC_CH_2_PA1
#define ADC_3V3_Channel           ADC_CH_3_PA2
#define ADC_VBUS_Channel          ADC_CH_9_PB0
#define ADC_CURR_Channel          ADC_CH_10_PB1

#define USB_CTL_Pin               GPIO_PIN_15
#define USB_CTL_Port              GPIOD

#define CH224_CFG2_Pin            GPIO_PIN_7
#define CH224_CFG3_Pin            GPIO_PIN_6
#define CH224_CFG_Port            GPIOB

#define BUZZ_Pin                  GPIO_PIN_15
#define BUZZ_Port                 GPIOA

#define LED1_Pin                  GPIO_PIN_14
#define LED2_Pin                  GPIO_PIN_13
#define LED_Port                  GPIOA

#define PWR_OUT_Pin               GPIO_PIN_8
#define PWR_OUT_Port              GPIOA

#endif
