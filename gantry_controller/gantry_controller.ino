
#include "stepper_motor.h"
#include "pins.h"

//motor objects
struct StepperMotor x_stepper;
struct StepperMotor y_stepper;
struct StepperMotor z_stepper;

int cur_bldc_speed=0;

int x = 10;
int y = 1;
int z = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("Hi Danny!");
  
  initializeStepper(&x_stepper,X_STEP,X_DIR,X_ENABLE,AXIS_X);
  initializeStepper(&y_stepper,Y_STEP,Y_DIR,Y_ENABLE,AXIS_Y);
  initializeStepper(&z_stepper,Z_STEP,Z_DIR,Z_ENABLE,AXIS_Z);
  
  pinMode(DRILL_PWM,OUTPUT);
  
  initializeEncoders();
  
}

void loop() {
  
  analogWrite(DRILL_PWM,cur_bldc_speed);
  
  goToStepperPosition(&x_stepper, x);
  //goToStepperPosition(&y_stepper, y/MM_PER_REV*STEPPER_CPR);
  //goToStepperPosition(&z_stepper, z/MM_PER_REV*STEPPER_CPR);
  
  Serial.println("loop");
  Serial.println(getEncoderDistance(x_stepper.axis));
  Serial.println(checkPosition(&x_stepper));
  delay(2000);
  
  goToStepperPosition(&x_stepper, 0);
  //goToStepperPosition(&y_stepper, 0);
  //goToStepperPosition(&z_stepper, 0);
  
  Serial.println("loop");
  Serial.println(getEncoderDistance(x_stepper.axis));
  Serial.println(checkPosition(&x_stepper));
  delay(2000);
  
}
