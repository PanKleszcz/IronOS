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
#include "n32l40x.h"
#include <stdint.h>

#include "cmsis_os.h"

#define LCD_RST_LOW()   GPIO_ResetBits(LCD_Port, LCD_RESET_Pin)
#define LCD_RST_HIGH()  GPIO_SetBits(LCD_Port, LCD_RESET_Pin)
#define LCD_CS_LOW()    GPIO_ResetBits(LCD_Port, LCD_CS_Pin)
#define LCD_CS_HIGH()   GPIO_SetBits(LCD_Port, LCD_CS_Pin)
#define LCD_MODE_CMD()  GPIO_ResetBits(LCD_Port, LCD_CMD_Pin)
#define LCD_MODE_DATA() GPIO_SetBits(LCD_Port, LCD_CMD_Pin)

SemaphoreHandle_t FRToSSPI::xSemaphore = nullptr;
StaticSemaphore_t FRToSSPI::xSemaphoreBuffer;

static void _spiSendByte(uint8_t byte) {
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_BUSY_FLAG) == SET)
    ;
  SPI_I2S_TransmitData(SPI1, byte);
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_TE_FLAG) == RESET)
    ;
  while (SPI_I2S_GetStatus(SPI1, SPI_I2S_BUSY_FLAG) == SET)
    ;
}

void FRToSSPI::sendByte(uint8_t byte) {
  LCD_CS_LOW();
  _spiSendByte(byte);
  LCD_CS_HIGH();
}

void FRToSSPI::sendCmd(uint8_t cmd) {
  LCD_MODE_CMD();
  sendByte(cmd);
  LCD_MODE_DATA();
}

void FRToSSPI::sendByteMutiple(uint8_t byte, size_t times) {
  LCD_CS_LOW();
  for (size_t i = 0; i < times; i++) {
    _spiSendByte(byte);
  }
  LCD_CS_HIGH();
}

void FRToSSPI::sendData(uint8_t *data, size_t length) {
  LCD_CS_LOW();
  for (size_t i = 0; i < length; i++) {
    _spiSendByte(data[i]);
  }
  LCD_CS_HIGH();
}

void FRToSSPI::sendPixels(uint8_t *data, size_t length) {
  LCD_CS_LOW();
  for (size_t i = 0; i < length; i++) {
    uint8_t tmp = data[i];
    uint8_t pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;

    tmp >>= 1;
    pix = (tmp & 1) ? 0xFF : 0x00;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
    while (!(SPI1->STS & SPI_I2S_TE_FLAG));
    SPI1->DAT = pix;
  }
  while (!(SPI1->STS & SPI_I2S_TE_FLAG));
  while (SPI1->STS & SPI_I2S_BUSY_FLAG);
  LCD_CS_HIGH();
}

void FRToSSPI::sendCmdChain(const FRToSSPI::SPI_CMD *commands, size_t length) {
  for (size_t i = 0; i < length; i++) {
    FRToSSPI::sendCmd(commands[i].cmd);
    if (commands[i].type == FRToSSPI::SPI_CMD_PAYLOAD) {
      sendData(commands[i].data, commands[i].len);
    } else if (commands[i].type == FRToSSPI::SPI_CMD_DELAY_MS) {
      delay_ms(commands[i].len);
    }
  }
}

// TODO: move it elsewhere
void FRToSSPI::sendLcdReset(void) {
  LCD_RST_HIGH();
  delay_ms(10);
  LCD_RST_LOW();
  delay_ms(10);
  LCD_RST_HIGH();
  delay_ms(120);
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

void FRToSSPI::init() { sendLcdReset(); }
