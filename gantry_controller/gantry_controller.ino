
#include "stepper_motor.h"
#include "pins.h"
#include "ringbuffer.h"

//motor objects
struct StepperMotor x_stepper;
struct StepperMotor y_stepper;
struct StepperMotor z_stepper;

int cur_bldc_speed=0;

struct RingBuffer ring_buffer;

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
  
  initializeRingBuffer(&ring_buffer);
  
}

void loop() {
  /*
  analogWrite(DRILL_PWM,cur_bldc_speed);
  
  /goToStepperPosition(&x_stepper, x);
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
  Serial.println(checkPosition(&x_stepper));*/
  delay(2000);
  
}

void serialHandler(){
  
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  String incoming_data = "";
  while (Serial.available()) {
    // get the new byte:
    char in_char = (char)Serial.read();
    // add it to the string
    incoming_data += in_char;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (in_char == '\n') {
      // TODO: check for E-Stop command
      if (incoming_data.length() < ringBufferRemainingSpaces(&ring_buffer)) {
        for (unsigned int i = 0; i < incoming_data.length(); i++)
          ringBufferEnque(&ring_buffer,incoming_data[i]);
      }
    }
  }
}


