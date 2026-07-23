/*
 * Software_I2C.h
 *
 *  Created on: 25 Jul 2020
 *      Author: Ralim, MrTick
 */
#ifndef BSP_FNIRSI_SOFTWARE_I2C_H_
#define BSP_FNIRSI_SOFTWARE_I2C_H_
#include "BSP.h"
#include "configuration.h"
#include "n32l40x.h"

// undefine conflicting LCD symbol
#undef LCD

#ifdef I2C_SOFT_BUS_2
#error "not implemented"
#endif

#ifdef I2C_SOFT_BUS_1
#define SOFT_SCL1_HIGH() GPIO_SetBits(I2C_Port, I2C_SCL_Pin)
#define SOFT_SCL1_LOW()  GPIO_ResetBits(I2C_Port, I2C_SCL_Pin)
#define SOFT_SDA1_HIGH() GPIO_SetBits(I2C_Port, I2C_SDA_Pin)
#define SOFT_SDA1_LOW()  GPIO_ResetBits(I2C_Port, I2C_SDA_Pin)
#define SOFT_SDA1_READ() (GPIO_ReadInputDataBit(I2C_Port, I2C_SDA_Pin) == Bit_SET ? 1 : 0)
#define SOFT_SCL1_READ() (GPIO_ReadInputDataBit(I2C_Port, I2C_SCL_Pin) == Bit_SET ? 1 : 0)
#endif

// TODO: verify I2C clock frequency
#define SOFT_I2C_DELAY()              \
  {                                   \
    for (int xx = 0; xx < 15; xx++) { \
      asm("nop");                     \
    }                                 \
  }

#endif /* BSP_FNIRSI_SOFTWARE_I2C_H_ */