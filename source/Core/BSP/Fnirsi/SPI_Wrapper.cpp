/*
 * Software_I2C.h
 *
 *  Created on: 25 Jul 2020
 *      Author: Ralim, MrTick
 */
#include "SPI_Wrapper.hpp"
#include "BSP.h"
#include "Pins.h"
// #include "configuration.h"
// #include "history.hpp"
#include <stdint.h>
#include "n32l40x.h"

#include "cmsis_os.h"


#define LCD_RST_LOW()   GPIO_ResetBits(LCD_Port, LCD_RESET_Pin)
#define LCD_RST_HIGH()    GPIO_SetBits(LCD_Port, LCD_RESET_Pin)
#define LCD_CS_LOW()    GPIO_ResetBits(LCD_Port, LCD_CS_Pin)
#define LCD_CS_HIGH()     GPIO_SetBits(LCD_Port, LCD_CS_Pin)
#define LCD_MODE_CMD()  GPIO_ResetBits(LCD_Port, LCD_CMD_Pin)
#define LCD_MODE_DATA()   GPIO_SetBits(LCD_Port, LCD_CMD_Pin)

SemaphoreHandle_t FRToSSPI::xSemaphore = nullptr;
StaticSemaphore_t FRToSSPI::xSemaphoreBuffer;

static void _spiSendByte(uint8_t byte) {
    while(SPI_I2S_GetStatus(SPI1, SPI_I2S_TE_FLAG) == RESET);
    SPI_I2S_TransmitData(SPI1, byte);
    while(SPI_I2S_GetStatus(SPI1, SPI_I2S_BUSY_FLAG) == SET);
}

void FRToSSPI::sendCmd(uint8_t cmd) {
    LCD_MODE_CMD();
    LCD_CS_LOW();
    _spiSendByte(cmd);
    LCD_CS_HIGH();
}

void FRToSSPI::sendByte(uint8_t byte) {
    LCD_MODE_DATA();
    LCD_CS_LOW();
    _spiSendByte(byte);
    LCD_CS_HIGH();
}

void FRToSSPI::sendByteMutiple(uint8_t byte, size_t times) {
    LCD_MODE_DATA();
    LCD_CS_LOW();
    for(size_t i = 0; i < times; i++) {
        _spiSendByte(byte);
    }
    LCD_CS_HIGH();
}

void FRToSSPI::sendData(uint8_t* data, size_t length) {
    LCD_MODE_DATA();
    LCD_CS_LOW();
    for(size_t i = 0; i < length; i++) {
        _spiSendByte(data[i]);
    }
    LCD_CS_HIGH();
}

void FRToSSPI::sendPixels(uint8_t* data, size_t length) {
    LCD_MODE_DATA();
    LCD_CS_LOW();
    for(size_t i = 0; i < length; i++) {
        for(uint8_t b=0; b<8; b++) {
            lock();
            if(data[i] & (1<<b)) {
                _spiSendByte(0xFF);
                _spiSendByte(0xFF);
            } else {
                _spiSendByte(0x00);
                _spiSendByte(0x00);
            }
            unlock();
        }
    }
    LCD_CS_HIGH();
}

void FRToSSPI::sendCmdChain(const FRToSSPI::SPI_CMD* commands, size_t length) {
    lock();
    for(size_t i = 0; i < length; i++) {
        FRToSSPI::sendCmd(commands[i].cmd);
        if(commands[i].type == FRToSSPI::SPI_CMD_PAYLOAD) {
            sendData(commands[i].data, commands[i].len);
        } else if (commands[i].type == FRToSSPI::SPI_CMD_DELAY_MS) {
            vTaskDelay(commands[i].len);
        }
    }
    unlock();
}

// TODO: move it elsewhere
void FRToSSPI::sendLcdReset(void) {
    lock();
    LCD_RST_HIGH();
    vTaskDelay(TICKS_10MS);
    LCD_RST_LOW();
    vTaskDelay(TICKS_10MS);
    LCD_RST_HIGH();
    vTaskDelay(TICKS_100MS);
    unlock();
}


bool FRToSSPI::lock() {
  if (xSemaphore == nullptr) {
    return false;
  }
  return xSemaphoreTake(xSemaphore, TICKS_SECOND) == pdTRUE;
}

void FRToSSPI::unlock() {
  if (xSemaphore == nullptr) {
    return;
  }
  xSemaphoreGive(xSemaphore);
}

void FRToSSPI::init() {

    // Init SPI GPIO
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1, ENABLE);  // APB2 runs at 32MHz

    GPIO_InitStructure.Pin = LCD_SCK_Pin | LCD_MOSI_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF0_SPI1;
    GPIO_InitPeripheral(LCD_Port, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LCD_RESET_Pin | LCD_CMD_Pin | LCD_CS_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;
    GPIO_InitPeripheral(LCD_Port, &GPIO_InitStructure);

    // Init SPI itself
    SPI_InitType SPI_InitStructure;
    SPI_InitStruct(&SPI_InitStructure);

    SPI_InitStructure.DataDirection = SPI_DIR_SINGLELINE_TX;
    SPI_InitStructure.SpiMode = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL = SPI_CLKPOL_HIGH;
    SPI_InitStructure.CLKPHA = SPI_CLKPHA_SECOND_EDGE;
    SPI_InitStructure.NSS = SPI_NSS_SOFT;

    SPI_InitStructure.BaudRatePres = SPI_BR_PRESCALER_2; // 16MHz is max what ST7735 supports

    SPI_InitStructure.FirstBit = SPI_FB_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Enable(SPI1, ENABLE);

    sendLcdReset();
}
