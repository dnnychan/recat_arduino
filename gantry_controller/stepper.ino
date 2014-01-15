#include "stepper_motor.h"

#define STEPPER_FORWARD 0
#define STEPPER_BACKWARD 1

int current_speed = 15;

void initializeStepper(struct StepperMotor * stepper, int step_pin, int dir_pin, int enable_pin)
{
  stepper->steps = 0;
  stepper->cur_dir = STEPPER_FORWARD;
  
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin,OUTPUT);
  pinMode(enable_pin, OUTPUT);
  
  digitalWrite(enable_pin,LOW);
  digitalWrite(dir_pin,LOW);
  
  stepper->step_pin = step_pin;
  stepper->dir_pin = dir_pin;
  stepper->enable_pin = enable_pin;
}

void stepOnce(struct StepperMotor * stepper, int step_speed) {
  digitalWrite(stepper->step_pin, HIGH);
  delay(step_speed);
  digitalWrite(stepper->step_pin, LOW);
  delay(step_speed);
  
  if (stepper->cur_dir==STEPPER_FORWARD)
    stepper->steps++;
  else 
    stepper->steps--;
}

void changeStepperDir(struct StepperMotor * stepper, int dir) {
  stepper->cur_dir = dir;
  digitalWrite(stepper->dir_pin,dir);
}

void wakeStepper(struct StepperMotor * stepper) { digitalWrite(stepper->enable_pin,HIGH); }
void sleepStepper(struct StepperMotor * stepper) { digitalWrite(stepper->enable_pin,LOW); }

void goToStepperPosition(struct StepperMotor * stepper, int pos) {
  
  wakeStepper(stepper);
  
  if (stepper->steps < pos) {
    changeStepperDir(stepper, STEPPER_FORWARD);
    while (stepper->steps < pos) {
      stepOnce(stepper,current_speed);
    }
  } else if (stepper->steps > pos) {
    changeStepperDir(stepper, STEPPER_BACKWARD);
    while (stepper->steps > pos) {
      stepOnce(stepper,current_speed);
    }
  }
  
  sleepStepper(stepper);
}
