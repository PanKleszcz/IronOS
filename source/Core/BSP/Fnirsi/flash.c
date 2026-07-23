/*
 * flash.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim, MrTick
 */
#include "BSP.h"
#include "BSP_Flash.h"
#include "n32l40x_flash.h"
#include "string.h"
#define FLASH_PAGE_SIZE (0x200)

void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  // TODO: implement length > FLASH_PAGE_SIZE and SETTINGS_START_PAGE not at page boundary
  uint8_t pageBackup[FLASH_PAGE_SIZE];
  memcpy(pageBackup, (uint8_t *)SETTINGS_START_PAGE, FLASH_PAGE_SIZE);

  // overwrite with given data:
  memcpy(pageBackup, buffer, length);

  FLASH_Unlock();
  resetWatchdog();

  FLASH_EraseOnePage(SETTINGS_START_PAGE);
  resetWatchdog();

  for (size_t offset = 0; offset < FLASH_PAGE_SIZE; offset += 4) {
    FLASH_ProgramWord(SETTINGS_START_PAGE + offset, *(uint32_t *)(pageBackup + offset));
    resetWatchdog();
  }

  FLASH_Lock();
  resetWatchdog();
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) { memcpy(buffer, (uint8_t *)SETTINGS_START_PAGE, length); }
