/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim, MrTick
 */
#include "Setup.h"
#include "TipThermoModel.h"
#include "Types.h"
#include "Utils.hpp"
#include "configuration.h"

extern uint16_t tipSenseResistancex10Ohms;

TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) {
  // let's assume 21uV per 1C here
  return (tipuVDelta) / 21;
}
