
#include "stepper_motor.h"
#include "pins.h"
#include "ringbuffer.h"
#include "mag_encoder.h"

//motor objects
struct StepperMotor x_stepper;
struct StepperMotor y_stepper;
struct StepperMotor z_stepper;

//mag encoder objects
struct MagEncoder mag_encoder_1;
struct MagEncoder mag_encoder_2;
struct MagEncoder mag_encoder_3;
struct MagEncoder mag_encoder_4;

int cur_bldc_speed=40; // no more than 70?

struct RingBuffer ring_buffer;

int x_destination = 10;
int y_destination = 1;
int z_destination = 1;

int stepper_speed = 5;

//int encoder_position_x = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Hi Danny!");
  
  initializeStepper(&x_stepper,X_STEP,X_DIR,X_ENABLE,AXIS_X);
  initializeStepper(&y_stepper,Y_STEP,Y_DIR,Y_ENABLE,AXIS_Y);
  initializeStepper(&z_stepper,Z_STEP,Z_DIR,Z_ENABLE,AXIS_Z);
  initializeEncoders();
  initializeRingBuffer(&ring_buffer);
  initializeMagEncoder(&mag_encoder_1,&mag_encoder_2,&mag_encoder_3,&mag_encoder_4);
  
  pinMode(DRILL_PWM,OUTPUT);

  // Changing PWM Frequency from http://forum.arduino.cc/index.php?PHPSESSID=vtjh1giaejdbbssm01hvhlnl76&topic=72092.msg541587#msg541587
  int myEraser = 7;             // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  int myPrescaler = 1;         // this could be a number in [1 , 6]. 1=31kHz
  TCCR2B |= myPrescaler;  //this operation (OR), replaces the last three bits in TCCR2B with our new value 011
  
}

void loop() {
  
  //analogWrite(DRILL_PWM,cur_bldc_speed);
  /*
  goToStepperPosition(&x_stepper, x);
  //goToStepperPosition(&y_stepper, y);
  //goToStepperPosition(&z_stepper, z);
  
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
  delay(2000);*/
  
  Serial.println(readMagEncoder(&mag_encoder_1));
  /*Serial.println(getEncoderDistance(x_stepper.axis));
  
  // x stepper control
  if (abs(getEncoderDistance(x_stepper.axis) - x_destination) > POSITION_TOLERANCE) {
    wakeStepper(&x_stepper);
    
    if (getEncoderDistance(x_stepper.axis) > x_destination) {
      changeStepperDir(&x_stepper, STEPPER_FORWARD);
      stepOnce(&x_stepper,stepper_speed);
    } else if (getEncoderDistance(x_stepper.axis) < x_destination) {
      changeStepperDir(&x_stepper, STEPPER_BACKWARD);
      stepOnce(&x_stepper,stepper_speed);
    }
  }
  else {
    sleepStepper(&x_stepper);
    // pull next destination
  }
  /*
  // y stepper control
  if (abs(getEncoderDistance(y_stepper.axis) - y_destination) > POSITION_TOLERANCE) {
    wakeStepper(&y_stepper);
    
    if (getEncoderDistance(y_stepper.axis) > y_destination) {
      changeStepperDir(&y_stepper, STEPPER_FORWARD);
      stepOnce(&y_stepper,stepper_speed);
    } else if (getEncoderDistance(y_stepper.axis) < y_destination) {
      changeStepperDir(&y_stepper, STEPPER_BACKWARD);
      stepOnce(&y_stepper,stepper_speed);
    }
  }
  else {
    sleepStepper(&y_stepper);
  }
  
  // z stepper control
  if (abs(getEncoderDistance(z_stepper.axis) - z_destination) > POSITION_TOLERANCE) {
    wakeStepper(&z_stepper);
    
    if (getEncoderDistance(z_stepper.axis) > z_destination) {
      changeStepperDir(&z_stepper, STEPPER_FORWARD);
      stepOnce(&z_stepper,stepper_speed);
    } else if (getEncoderDistance(z_stepper.axis) < z_destination) {
      changeStepperDir(&z_stepper, STEPPER_BACKWARD);
      stepOnce(&z_stepper,stepper_speed);
    }
  }
  else {
    sleepStepper(&z_stepper);
  }*/
  
}

void serialHandler(){
  if (!ringBufferEmpty(&ring_buffer))
    return;
  
  String command = "";
  char cur_char = ringBufferDeque(&ring_buffer);
  
  while (cur_char != '\n' && (!ringBufferEmpty(&ring_buffer))) {
    command += cur_char;
    cur_char = ringBufferDeque(&ring_buffer);
  }
  
  if (command.startsWith("m0")) {
    // stop
  } else if (command.startsWith("g0")) {
    // goto x y z
  } 
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


