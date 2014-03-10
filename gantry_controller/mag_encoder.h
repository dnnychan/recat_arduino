#pragma once

struct MagEncoder {
  // encoder calibration values
  int zero;
  int minus_90;
  int plus_90;
  double m1,m2,b1,b2;
  
  int CS_pin;
};
