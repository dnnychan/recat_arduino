#pragma once

// List of pins on the Arduino

// ADC pin for 12V power. used to monitor and run appropriate commands when turning on and off main power
#define MAIN_POWER 1

// stepper pins
#define X_STEP 46
#define X_DIR 47
#define X_ENABLE 37
#define Y_STEP 44
#define Y_DIR 45
#define Y_ENABLE 39
#define Z_STEP 42
#define Z_DIR 43
#define Z_ENABLE 41

//limit sensor pins. X, Y button unused
#define X_BUTTON 36
#define Y_BUTTON 38
#define Z_BUTTON 40

// encoder pins
#define X_ENCODER_A 19
#define X_ENCODER_B 22
#define Y_ENCODER_A 20
#define Y_ENCODER_B 23
#define Z_ENCODER_A 21
#define Z_ENCODER_B 24

// limit to how far the stepper can move
#define X_LENGTH 52.00
#define Y_LENGTH 50.00
#define Z_LENGTH 95.00

//pwm for drill
#define DRILL_PWM 10
#define DRILL_FEEDBACK 0
#define DRILL_POWER 26

//pins for the magnetic encoder. data and clock pins are common between all magnetic encoders
#define DATAPIN 50
#define CLOCKPIN 52
#define CS_1 32
#define CS_2 33
#define CS_3 34
#define CS_4 35

// convert rotational distance to linear distance
#define MM_PER_REV 0.609113 //1/41.7"

// counts per revolution
#define STEPPER_CPR 24
#define ENCODER_CPR 512

#define STEPPER_FORWARD 0
#define STEPPER_BACKWARD 1

#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3

#define STEPPER_ENCODER_TOL 0.1

#define POSITION_TOLERANCE 0.05 //mm
