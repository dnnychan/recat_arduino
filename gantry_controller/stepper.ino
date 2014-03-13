#include "stepper_motor.h"
#include "pins.h"

void initializeStepper(struct StepperMotor * stepper, int step_pin, int dir_pin, int enable_pin, int switch_pin, int axis)
{
  stepper->steps = 0;
  stepper->cur_dir = STEPPER_FORWARD;
  
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin,OUTPUT);
  pinMode(enable_pin, OUTPUT);
  pinMode(switch_pin, INPUT);
  
  digitalWrite(enable_pin,LOW);
  digitalWrite(dir_pin,LOW);
  digitalWrite(switch_pin,HIGH); // internal pullup
  
  stepper->step_pin = step_pin;
  stepper->dir_pin = dir_pin;
  stepper->enable_pin = enable_pin;
  stepper->switch_pin = switch_pin;
  stepper->axis = axis;
  stepper->current_speed=25;
}

void stepOnce(struct StepperMotor * stepper, int step_speed) {
  digitalWrite(stepper->step_pin, HIGH);
  delay(stepper->current_speed);
  digitalWrite(stepper->step_pin, LOW);
  delay(stepper->current_speed);
  
  if (stepper->cur_dir==STEPPER_FORWARD)
    stepper->steps++;
  else 
    stepper->steps--;
}

void calibrateStepper(struct StepperMotor * stepper) {
  //stepper->cur_dir = STEPPER_BACKWARD;
  changeStepperDir(stepper, STEPPER_FORWARD);
  wakeStepper(stepper);
  
  if (stepper->axis == AXIS_Z) {
    while(digitalRead(stepper->switch_pin)) {
      stepOnce(stepper,stepper->current_speed);
    }
  } else {
    double previous_position, new_position;
    previous_position = getEncoderDistance(stepper->axis);
    stepOnce(stepper,stepper->current_speed);
    delay(20);
    stepOnce(stepper,stepper->current_speed);
    delay(20);
    stepOnce(stepper,stepper->current_speed);
    delay(20);
    new_position = getEncoderDistance(stepper->axis);
    //Serial.print(previous_position);
    //Serial.println(new_position);
    while (abs(new_position - previous_position) > 0.01) {
      //Serial.println(stepper->axis);
      previous_position = new_position;
      stepOnce(stepper,stepper->current_speed);
      stepOnce(stepper,stepper->current_speed);
      stepOnce(stepper,stepper->current_speed);
      new_position = getEncoderDistance(stepper->axis);
      //Serial.print(previous_position);
      //Serial.println(new_position);
    }
  }
  stepper->steps = 0;
  sleepStepper(stepper);
}

void changeStepperDir(struct StepperMotor * stepper, int dir) {
  stepper->cur_dir = dir;
  digitalWrite(stepper->dir_pin,dir);
}

void changeStepperSpeed(struct StepperMotor * stepper, int new_speed) { stepper->current_speed = new_speed; }

void wakeStepper(struct StepperMotor * stepper) { digitalWrite(stepper->enable_pin,HIGH); }
void sleepStepper(struct StepperMotor * stepper) { digitalWrite(stepper->enable_pin,LOW); }

/*
void goToStepperPosition(struct StepperMotor * stepper, double pos) {
  
  //int encoder_position = getEncoderDistance(stepper->axis);
  
  wakeStepper(stepper);
  
  if (getEncoderDistance(stepper->axis) > pos) {
    changeStepperDir(stepper, STEPPER_FORWARD);
    while (getEncoderDistance(stepper->axis) > pos) {
      stepOnce(stepper,current_speed);
    }
  } else if (getEncoderDistance(stepper->axis) < pos) {
    changeStepperDir(stepper, STEPPER_BACKWARD);
    while (getEncoderDistance(stepper->axis) < pos) {
      stepOnce(stepper,current_speed);
    }
  }
  
  sleepStepper(stepper);
}

boolean checkPosition(struct StepperMotor * stepper) {
  Serial.println(stepper->steps*MM_PER_REV/STEPPER_CPR);
  if (abs(stepper->steps/MM_PER_REV*STEPPER_CPR - getEncoderDistance(stepper->axis)) < STEPPER_ENCODER_TOL)
    return true;
  else
    return false;
}*/
