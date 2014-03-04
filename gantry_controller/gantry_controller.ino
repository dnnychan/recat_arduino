#include "stepper_motor.h"
#include "pins.h"
#include "ringbuffer.h"
#include "mag_encoder.h"
#include <QueueArray.h>

//motor objects
struct StepperMotor x_stepper;
struct StepperMotor y_stepper;
struct StepperMotor z_stepper;

//mag encoder objects
struct MagEncoder mag_encoder_1;
struct MagEncoder mag_encoder_2;
struct MagEncoder mag_encoder_3;
struct MagEncoder mag_encoder_4;

int cur_bldc_speed = 0; //30 to 90 at R=470
int new_bldc_speed = 0;
int bldc_feedback = 1023;
bool enable_steppers = true;
bool bldc_is_spinning = false;

struct RingBuffer ring_buffer;

double x_destination = 0;
double y_destination = 0;
double z_destination = 0;

QueueArray<double> x_queue;
QueueArray<double> y_queue;
QueueArray<double> z_queue;

int new_commands = 0;

int stepper_speed = 5;    // I don't think this actually does anything

//int encoder_position_x = 0;

void setup() {
  // TODO: Make BLDC not jump around during start up
  pinMode(DRILL_PWM,OUTPUT);
  // Changing PWM Frequency from http://forum.arduino.cc/index.php?PHPSESSID=vtjh1giaejdbbssm01hvhlnl76&topic=72092.msg541587#msg541587
  int myEraser = 7;             // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  int myPrescaler = 1;         // this could be a number in [1 , 6]. 1=31kHz
  TCCR2B |= myPrescaler;  //this operation (OR), replaces the last three bits in TCCR2B with our new value 011
  analogWrite(DRILL_PWM,0);
  
  initializeStepper(&x_stepper,X_STEP,X_DIR,X_ENABLE,X_BUTTON,AXIS_X);
  initializeStepper(&y_stepper,Y_STEP,Y_DIR,Y_ENABLE,Y_BUTTON,AXIS_Y);
  initializeStepper(&z_stepper,Z_STEP,Z_DIR,Z_ENABLE,Z_BUTTON,AXIS_Z);
  initializeEncoders();
  initializeRingBuffer(&ring_buffer);
  initializeMagEncoder(&mag_encoder_1,&mag_encoder_2,&mag_encoder_3,&mag_encoder_4);
  Serial.begin(9600);
  
  calibrateStepper(&z_stepper);
  delay(500);
  calibrateStepper(&x_stepper);
  delay(500);
  calibrateStepper(&y_stepper);
  resetEncoders();
  
  Serial.println("Hi Danny!");
  
  pinMode(DRILL_POWER, OUTPUT);
  digitalWrite(DRILL_POWER, LOW);
  
}

void loop() {
  serialHandler();
  
  if (cur_bldc_speed != new_bldc_speed) {
    if (abs(cur_bldc_speed - new_bldc_speed) < 5)
      cur_bldc_speed = new_bldc_speed;
    else if (new_bldc_speed > cur_bldc_speed)
      cur_bldc_speed += 5;
    else if (new_bldc_speed < cur_bldc_speed)
      cur_bldc_speed -= 5;
    analogWrite(DRILL_PWM,cur_bldc_speed);
  }
  
  bldc_feedback=analogRead(DRILL_FEEDBACK);
  if (bldc_feedback < 50)
    bldc_is_spinning = true;
  else
    bldc_is_spinning = false;
  /*
  Serial.print(readMagEncoder(&mag_encoder_1));
  Serial.print("\t");
  Serial.print(readMagEncoder(&mag_encoder_2));
  Serial.print("\t");
  Serial.println(readMagEncoder(&mag_encoder_4));   */
  
  if (enable_steppers && bldc_is_spinning) {
    /*Serial.print(getEncoderDistance(x_stepper.axis));
    Serial.print("\t");
    Serial.print(getEncoderDistance(y_stepper.axis));
    Serial.print("\t");
    Serial.println(getEncoderDistance(z_stepper.axis));*/
    
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
    else { // x is correct
      sleepStepper(&x_stepper);
      
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
      else { //x, y are correct
        sleepStepper(&y_stepper);
         
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
          
          //delay(100);
          
          if (!x_queue.isEmpty() && !y_queue.isEmpty() && !z_queue.isEmpty()) {
            x_destination = x_queue.dequeue();
            y_destination = y_queue.dequeue();
            z_destination = z_queue.dequeue();
          }
        } // else z
      } // else y
    } // else x
  } else { //!enable_steppers
    
    sleepStepper(&x_stepper);
    sleepStepper(&y_stepper);
    sleepStepper(&z_stepper);
  }
  
}

double convertToNumber(String string_to_convert, int first_digit) {
  return (int)(string_to_convert.charAt(first_digit) - '0') * 10 + (int)(string_to_convert.charAt(first_digit + 1) -'0'); 
}

void serialHandler(){
  if (ringBufferEmpty(&ring_buffer) || new_commands == 0)
    return;
  
  String command = "";
  char cur_char = ringBufferDeque(&ring_buffer);
  
  while (cur_char != '\n' && (!ringBufferEmpty(&ring_buffer))) {
    command += cur_char;
    cur_char = ringBufferDeque(&ring_buffer);
  }
  
  new_commands--;
  //Serial.println(ringBufferRemainingSpaces(&ring_buffer));
  
  //Serial.println(" ");
  
  if (command.startsWith("stop")) {  //stop everything
    cur_bldc_speed = 0;
    new_bldc_speed = 0;
    analogWrite(DRILL_PWM,cur_bldc_speed);
    enable_steppers = false;
  } else if (command.startsWith("goto")) {  // go to position: goto XX.xx YY.yy ZZZ.zz
    enable_steppers = true;
    
    double enqueue_value = convertToNumber(command, 5) + convertToNumber(command, 8) * 0.01; 
    if (enqueue_value > X_LENGTH)    
      x_queue.enqueue(X_LENGTH);
    else if (enqueue_value < 0)
      x_queue.enqueue(0);
    else
      x_queue.enqueue(enqueue_value);  
      
    enqueue_value = convertToNumber(command,11) + convertToNumber(command,14) * 0.01;
    if (enqueue_value > Y_LENGTH)
      y_queue.enqueue(Y_LENGTH);
    else if (enqueue_value < 0)
      y_queue.enqueue(0);
    else
      y_queue.enqueue(enqueue_value);
    
    enqueue_value = (int)(command.charAt(17) - '0') * 100 + convertToNumber(command,18) + convertToNumber(command,21) * 0.01;
    if (enqueue_value > Z_LENGTH) 
      z_queue.enqueue(Z_LENGTH);
    else if (enqueue_value < 0)
      z_queue.enqueue(0);
    else
      z_queue.enqueue(enqueue_value); 

  } else if (command.startsWith("bldc")) { // change bldc speed: bldc xx
    new_bldc_speed = convertToNumber(command, 5); 
  } else if (command.startsWith("step")) { // change step speed: step xx
    stepper_speed = convertToNumber(command,5);
    changeStepperSpeed(&x_stepper,stepper_speed);
    changeStepperSpeed(&y_stepper,stepper_speed);
    changeStepperSpeed(&z_stepper,stepper_speed);
  } else if (command.startsWith("cont")) { // continue
    enable_steppers = true;
  } else if (command.startsWith("on")) {
    digitalWrite(DRILL_POWER,HIGH);
    new_bldc_speed = 20;
  } else if (command.startsWith("off")) {
    digitalWrite(DRILL_POWER,LOW);
    new_bldc_speed = 0;
  } else if (command.startsWith("clear")) {
    while (!x_queue.isEmpty() && !y_queue.isEmpty() && !z_queue.isEmpty()) {
      // clear the queues
      x_destination = x_queue.dequeue();
      y_destination = y_queue.dequeue();
      z_destination = z_queue.dequeue();
    }
    // set next destination to current position
    
    x_destination = getEncoderDistance(x_stepper.axis);
    y_destination = getEncoderDistance(y_stepper.axis);
    z_destination = getEncoderDistance(z_stepper.axis);
  } else if (command.startsWith("gPos")) {    //get position
    //char buffer [20];
    //sprintf(buffer, "%05.2f %05.2f %06.f", getEncoderDistance(x_stepper.axis), getEncoderDistance(y_stepper.axis), getEncoderDistance(z_stepper.axis));
    Serial.print(getEncoderDistance(x_stepper.axis));
    Serial.print(" ");
    Serial.print(getEncoderDistance(y_stepper.axis));
    Serial.print(" ");
    Serial.println(getEncoderDistance(z_stepper.axis));
  } else if (command.startsWith("gAng")) {    // get angles
    //char buffer [25];
    //sprintf(buffer, "%05.2f %05.2f %05.2f %05.2f", readMagEncoder(&mag_encoder_1), readMagEncoder(&mag_encoder_2), /*readMagEncoder(&mag_encoder_3)*/0, readMagEncoder(&mag_encoder_4));
    //Serial.println(buffer);
    Serial.print(readMagEncoder(&mag_encoder_1));
    Serial.print(" ");
    Serial.print(readMagEncoder(&mag_encoder_2));
    Serial.print(" ");
    //Serial.print(readMagEncoder(&mag_encoder_3));
    Serial.print(0);
    Serial.print(" ");
    Serial.println(readMagEncoder(&mag_encoder_4));
  } else if (command.startsWith("qEmp")) {    //are queues empty
    if (!x_queue.isEmpty() && !y_queue.isEmpty() && !z_queue.isEmpty()) {
      Serial.println(0);    // queues aren't empty
    } else {
      Serial.println(1);    //queues are empty
    }
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  //Serial.println("new input");
  String incoming_data = "";
  while (Serial.available()) {
    // get the new byte:
    char in_char = (char)Serial.read();
    Serial.print(in_char);
    // add it to the string
    //incoming_data += in_char;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    /*if (in_char == '\n') {
      // TODO: check for E-Stop command
      if (incoming_data.length() < ringBufferRemainingSpaces(&ring_buffer)) {
        for (unsigned int i = 0; i < incoming_data.length(); i++)
          ringBufferEnque(&ring_buffer,incoming_data[i]);
      }
    }*/
    ringBufferEnque(&ring_buffer,in_char);
    if (in_char == '\n')
      new_commands++;
  }
  //Serial.println(" ");
}
