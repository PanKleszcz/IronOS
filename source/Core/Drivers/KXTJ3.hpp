/*
 * KXTJ3.hpp
 *
 *  Created on: 14 May 2026
 *      Author: OK2CM
 */

#ifndef DRIVERS_KXTJ3_HPP_
#define DRIVERS_KXTJ3_HPP_
#include "BSP.h"
#include "I2C_Wrapper.hpp"

class KXTJ3 {
public:
  // Returns true if this accelerometer is detected
  static bool detect();
  // Init any internal state
  static bool initalize();
  // Reads the I2C register and returns the orientation
  static Orientation getOrientation();
  // Return the x/y/z axis readings as signed int16's
  static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);

private:
};

#endif /* DRIVERS_KXTJ3_HPP_ */
