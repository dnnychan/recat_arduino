#pragma once

struct MagEncoder {
  // encoder calibration values
  int zero;
  int minus_90;
  int plus_90;
  
  int CS_pin;
};
