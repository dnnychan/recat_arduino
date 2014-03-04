#pragma once

struct StepperMotor {
  int steps;
  int cur_dir;
  int axis;
  int current_speed;

  int step_pin;
  int dir_pin;
  int enable_pin;
  int switch_pin;
};
