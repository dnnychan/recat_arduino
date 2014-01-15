#pragma once

struct StepperMotor {
  int steps;
  int cur_dir;

//encoder
//limit

//step speed??

  int step_pin;
  int dir_pin;
  int enable_pin;
};
