#pragma once

// Magnetic encoder object
struct MagEncoder {
  // encoder calibration values
  
  // use m1, m2, b1, b2. they are found by doing linear regression on multiple points to convert from the digital reading to degrees.
  // the equation is degrees = m1 * digital reading + b1. since the magnetic encoders for the region we want to use may not be continuous
  // and wraps around at 1024, we use two regression lines to fit the data.
  
  // zero, minus_90, and plus_90 are obsolete
  int zero;
  int minus_90;
  int plus_90;
  double m1,m2,b1,b2;
  
  int CS_pin;
};
