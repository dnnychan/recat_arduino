
#include "stepper_motor.h"

#define X_STEP 52
#define X_DIR 53
#define X_ENABLE 43
#define Y_STEP 50
#define Y_DIR 51
#define Y_ENABLE 45
#define Z_STEP 48
#define Z_DIR 49
#define Z_ENABLE 47

#define STEPPER_CPR 24

//motor objects
struct StepperMotor x_stepper;
struct StepperMotor y_stepper;
struct StepperMotor z_stepper;

void setup() {
  Serial.begin(9600);
  Serial.println("Hi Danny!");
  
  initializeStepper(&x_stepper,X_STEP,X_DIR,X_ENABLE);
  initializeStepper(&y_stepper,Y_STEP,Y_DIR,Y_ENABLE);
  initializeStepper(&z_stepper,Z_STEP,Z_DIR,Z_ENABLE);
  
}

void loop() {
  goToStepperPosition(&x_stepper, STEPPER_CPR*2);
  goToStepperPosition(&y_stepper, STEPPER_CPR*2);
  goToStepperPosition(&z_stepper, STEPPER_CPR*2);
  
  Serial.println("loop");
  delay(5000);
  
  goToStepperPosition(&x_stepper, 0);
  goToStepperPosition(&y_stepper, 0);
  goToStepperPosition(&z_stepper, 0);
  
  Serial.println("loop");
  delay(5000);
  
}
