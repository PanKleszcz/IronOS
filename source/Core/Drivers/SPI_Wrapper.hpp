/*
 * FRToSSPI.hpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */

#ifndef FRTOSSPI_HPP_
#define FRTOSSPI_HPP_

#include "cmsis_os.h"

class FRToSSPI {
public:
  // static void FRToSInit() {
  //   if (I2CSemaphore == nullptr) {
  //     I2CSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
  //     xSemaphoreGive(I2CSemaphore);
  //   }
  // }
  static void init(void);


//   static void CpltCallback(); // Normal Tx Callback

//   static bool Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
//   static bool Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
//   // Returns true if device ACK's being addressed
//   static bool    probe(uint16_t DevAddress);
//   static bool    wakePart(uint16_t DevAddress);
//   static bool    Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
//   static void    Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
//   static void    TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx);
//   static bool    I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data);
//   static uint8_t I2C_RegisterRead(uint8_t address, uint8_t reg);

  typedef enum {
    SPI_CMD_PAYLOAD,
    SPI_CMD_DELAY_MS,
  } SPI_CMD_TYPE;

  typedef struct {
    uint8_t cmd;
    SPI_CMD_TYPE type;
    uint8_t len; // payload or delay
    uint8_t* data; // optional payload
  }SPI_CMD;


  static void sendData(uint8_t* data, size_t length);
  static void sendByte(uint8_t byte);
  static void sendByteMutiple(uint8_t byte, size_t times);
  static void sendCmd(uint8_t cmd);

  // Consider abstracting parts of this implementation outside as LCD class
  static void sendCmdChain(const FRToSSPI::SPI_CMD* commands, size_t length);
  static void sendPixels(uint8_t* data, size_t length); // similar to sendData but converts OLED to LCD

//     const uint8_t reg;      // The register to write to
//     uint8_t       val;      // The value to write to this register
//     const uint8_t pause_ms; // How many ms to pause _after_ writing this reg
  // } I2C_REG;
//   static bool writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength);

private:
//   static void              unlock();
//   static bool              lock();
//   static void              I2C_Unstick();
//   static SemaphoreHandle_t I2CSemaphore;
//   static StaticSemaphore_t xSemaphoreBuffer;
};

#endif /* FRTOSI2C_HPP_ */
