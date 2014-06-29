#include "stepper_motor.h"
#include "pins.h"
#include "ringbuffer.h"
#include "mag_encoder.h"
#include <QueueArray.h>
#include <Servo.h> 

/*
NICE TO HAVE:
-watch dog - ensure communication to pc
-real time magnetic encoder monitoring
*/

//motor objects
struct StepperMotor x_stepper;
struct StepperMotor y_stepper;
struct StepperMotor z_stepper;

//mag encoder objects
struct MagEncoder mag_encoder_1;
struct MagEncoder mag_encoder_2;
struct MagEncoder mag_encoder_3;
struct MagEncoder mag_encoder_4;

// previous_destination was used to check what the last action is. it isn't used in this implementation
int previous_direction = 0;
bool enable_steppers = true;
/*
bool bldc_is_spinning = false;*/
int drill_speed = 1105;
bool power_off = true;

struct RingBuffer ring_buffer;

// current destination we need to achieve.
double x_destination = 0;
double y_destination = 0;
double z_destination = 0;

// queue of destinations dumped in over serial
QueueArray<double> x_queue;
QueueArray<double> y_queue;
QueueArray<double> z_queue;

Servo bldc;

int new_commands = 0;

int stepper_speed = 5;    // I don't think this actually does anything

void setup() {
  
  // initialization for the drill
  bldc.attach(DRILL_PWM);
  bldc.writeMicroseconds(1100);
  delay(8000);
  
  // run the other initialization functions
  initializeStepper(&x_stepper,X_STEP,X_DIR,X_ENABLE,X_BUTTON,AXIS_X);
  initializeStepper(&y_stepper,Y_STEP,Y_DIR,Y_ENABLE,Y_BUTTON,AXIS_Y);
  initializeStepper(&z_stepper,Z_STEP,Z_DIR,Z_ENABLE,Z_BUTTON,AXIS_Z);
  initializeEncoders();
  initializeRingBuffer(&ring_buffer);
  initializeMagEncoder(&mag_encoder_1,&mag_encoder_2,&mag_encoder_3,&mag_encoder_4);
  Serial.begin(9600);
  
  // calibration runs twice because it isn't too reliable...
  calibrateStepper(&z_stepper);
  delay(100);
  calibrateStepper(&x_stepper);
  delay(100);
  calibrateStepper(&y_stepper);
  delay(100);
  calibrateStepper(&z_stepper);
  delay(100);
  calibrateStepper(&x_stepper);
  delay(100);
  calibrateStepper(&y_stepper);
  resetEncoders();
  
  // check if 12V power is on
  if (analogRead(MAIN_POWER) > 200)
    power_off = false;
  else
    power_off = true;
  
  //bldc.writeMicroseconds(1300);
  
  Serial.println("Hi Danny!");
  /*
  pinMode(DRILL_POWER, OUTPUT);
  digitalWrite(DRILL_POWER, LOW);*/
  
}

void loop() {
  serialHandler();
  if (analogRead(MAIN_POWER) > 200) {    // check if the main power is on
  
    if (power_off == true) {     // start up sequence for bldc. resets if power is removed, and runs once when turning on the power
      //Serial.println("hi");
      bldc.writeMicroseconds(1100);
      delay(8000);
      bldc.writeMicroseconds(drill_speed);
      power_off = false;
    }
    
    if (enable_steppers) {    // if steppers are enabled, we move each axis one at a time to the next point in the queue
    // the priority is to lift up the drill first so it isn't in the patient
    // the following is a series of if-else's 
    // each stepper is kept awake until we are done with that axis rather than sleeping after each step
    // the current position is read in the if statements and compared to the destination
    // if all of them (x,y,z) are correct, we pop the next destination from the queue and repeat the process
    // until there is nothing left in the queue
      
      if ((getEncoderDistance(z_stepper.axis) - z_destination) > POSITION_TOLERANCE) { // lift up first
        wakeStepper(&z_stepper);  
        
        // check position
        if (getEncoderDistance(z_stepper.axis) > z_destination) {
          changeStepperDir(&z_stepper, STEPPER_FORWARD);  // set the direction
          stepOnce(&z_stepper,stepper_speed);            // step the stepper
          previous_direction = 5;
        }
      } else {
        
        // x stepper control
        if (abs(getEncoderDistance(x_stepper.axis) - x_destination) > POSITION_TOLERANCE) {
          sleepStepper(&z_stepper);
          wakeStepper(&x_stepper);
          
          if (getEncoderDistance(x_stepper.axis) > x_destination) {
            changeStepperDir(&x_stepper, STEPPER_FORWARD);
            stepOnce(&x_stepper,stepper_speed);
            previous_direction = 1;
          } else if (getEncoderDistance(x_stepper.axis) < x_destination) {
            changeStepperDir(&x_stepper, STEPPER_BACKWARD);
            stepOnce(&x_stepper,stepper_speed);
            previous_direction = 2;
          }
        }
        else { // x is correct
          sleepStepper(&x_stepper);
          
          // y stepper control
          if (abs(getEncoderDistance(y_stepper.axis) - y_destination) > POSITION_TOLERANCE) {
            sleepStepper(&z_stepper);
            wakeStepper(&y_stepper);
            
            if (getEncoderDistance(y_stepper.axis) > y_destination) {
              changeStepperDir(&y_stepper, STEPPER_FORWARD);
              stepOnce(&y_stepper,stepper_speed);
              previous_direction = 3;
            } else if (getEncoderDistance(y_stepper.axis) < y_destination) {
              changeStepperDir(&y_stepper, STEPPER_BACKWARD);
              stepOnce(&y_stepper,stepper_speed);
              previous_direction = 4;
            }
          }
          else { //x, y are correct
            sleepStepper(&y_stepper);
             
            // z stepper control
            if (abs(getEncoderDistance(z_stepper.axis) - z_destination) > POSITION_TOLERANCE) {
              wakeStepper(&z_stepper);
              
              if (getEncoderDistance(z_stepper.axis) < z_destination) {
                changeStepperDir(&z_stepper, STEPPER_BACKWARD);
                stepOnce(&z_stepper,stepper_speed);
                previous_direction = 6;
              }
            }
            else {    //reached destination
              sleepStepper(&z_stepper);
              previous_direction = 0;
              
              //delay(100);
              
              if (!x_queue.isEmpty() && !y_queue.isEmpty() && !z_queue.isEmpty()) {  //pop next destination
                x_destination = x_queue.dequeue();
                y_destination = y_queue.dequeue();
                z_destination = z_queue.dequeue();
              }
            } // else z
          } // else y
        } // else x
      }
    } else { //!enable_steppers && !bldc_is_on
      
      // put everything to sleep
      sleepStepper(&x_stepper);
      sleepStepper(&y_stepper);
      sleepStepper(&z_stepper);
      //previous_direction = 0;
    }
  } else { // power is off
    delay(100);
    power_off = true;
    bldc.writeMicroseconds(1100);    // turn off the signal to the esc
    //Serial.println("power off");
    /*cur_bldc_speed = 0;
    new_bldc_speed = 0;
    analogWrite(DRILL_PWM,cur_bldc_speed);
    enable_steppers = false;*/
  }
}

double convertToNumber(String string_to_convert, int first_digit) {  //converts a two character string to a double. no checks if you give it letters.
  return (int)(string_to_convert.charAt(first_digit) - '0') * 10 + (int)(string_to_convert.charAt(first_digit + 1) -'0'); 
}

void serialHandler(){   //process commands stored in ring buffer
  if (ringBufferEmpty(&ring_buffer) || new_commands == 0)  //check for new commands
    return;
  
  String command = "";
  char cur_char = ringBufferDeque(&ring_buffer);
  
  while (cur_char != '\n' && (!ringBufferEmpty(&ring_buffer))) {
    // read a command from the string buffer.
    command += cur_char;
    cur_char = ringBufferDeque(&ring_buffer);
  }
  
  new_commands--;
  //Serial.println(ringBufferRemainingSpaces(&ring_buffer));
  
  //Serial.println(" ");
  
  /*
  List of commands (these functions should mostly have wrappers in the python):
  stop                    stops everything from running
  goto XX.xx YY.yy ZZZ.zz  adds that position to the queue of points the robot to move to. units in mm
  bldc xxxx               write xxxx to the pwm pin in the bldc. 1000 stops, and 2000 is max speed. typically ran at 1200-1400 ish during final tests
  step xx                 stepper delay when generating square waves. rarely used. changes all stepper speeds
  cont                    turns steppers back on
  on                      turns stepper and bldc on at default speeds
  off                     stops everything
  clear                   clears gantry queue
  gAng                    get the magnetic encoder angles in degrees. degrees are used over radians because degrees provide more resolution when sending (defaults to 2 decimal precision. didn't bother changing)
  gPos                    get the current position of the drill in the gantry
  qEmp                    check if the queue is empty
  
  
  */
  if (command.startsWith("stop")) {  //stop everything
  /*
    cur_bldc_speed = 0;
    new_bldc_speed = 0;
    analogWrite(DRILL_PWM,cur_bldc_speed);*/
    enable_steppers = false;
  } else if (command.startsWith("goto")) {  // go to position: goto XX.xx YY.yy ZZZ.zz
    //enable_steppers = true;
    
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
    /*new_bldc_speed = convertToNumber(command, 5);//(int)(command.charAt(5) - '0') * 100 + convertToNumber(command, 6); //3 digit. 099 results the same as 255 with our motor, so not going to bother
    digitalWrite(DRILL_POWER,HIGH);*/
    drill_speed = convertToNumber(command, 5) * 100 + convertToNumber(command, 7);
    bldc.writeMicroseconds(drill_speed);
    //Serial.println(drill_speed);
  } else if (command.startsWith("step")) { // change step speed: step xx
    stepper_speed = convertToNumber(command,5);
    changeStepperSpeed(&x_stepper,stepper_speed);
    changeStepperSpeed(&y_stepper,stepper_speed);
    changeStepperSpeed(&z_stepper,stepper_speed);
  } else if (command.startsWith("cont")) { // continue
    enable_steppers = true;
  } else if (command.startsWith("on")) {
    enable_steppers = true;
    drill_speed = 1300;
    bldc.writeMicroseconds(drill_speed);
    /*
    digitalWrite(DRILL_POWER,HIGH);
    new_bldc_speed = 20;*/
  } else if (command.startsWith("off")) {
    bldc.writeMicroseconds(1105);
    /*
    digitalWrite(DRILL_POWER,LOW);
    new_bldc_speed = 0;*/
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
    Serial.print(readMagEncoder(&mag_encoder_3));
    //Serial.print(0);
    Serial.print(" ");
    Serial.println(readMagEncoder(&mag_encoder_4));
  } else if (command.startsWith("qEmp")) {    //are queues empty
    if (!x_queue.isEmpty() && !y_queue.isEmpty() && !z_queue.isEmpty()) {
      Serial.println(0);    // queues aren't empty
    } else {
      Serial.println(1);    // queues are empty
    }
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 All this part does is store the incoming commands into
 a buffer so they can be processed when the system has 
 time.
 */
void serialEvent() {
  //Serial.println("new input");
  String incoming_data = "";
  while (Serial.available()) {  // while there is still data in the serial buffer
    // get the new byte: 
    char in_char = (char)Serial.read();
    Serial.print(in_char);
    
    // add one new byte to the ring buffer.
    ringBufferEnque(&ring_buffer,in_char);
    if (in_char == '\n')    // a full command is ended by a line break.
      new_commands++;
  }
  //Serial.println(" ");
}
