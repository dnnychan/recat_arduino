#include "pins.h"

long encoder_x_pos = 0;
long encoder_y_pos = 0;
long encoder_z_pos = 0;

void initializeEncoders() {
  pinMode(X_ENCODER_A, INPUT);
  pinMode(X_ENCODER_B, INPUT);
  pinMode(Y_ENCODER_A, INPUT);
  pinMode(Y_ENCODER_B, INPUT);
  pinMode(Z_ENCODER_A, INPUT);
  pinMode(Z_ENCODER_B, INPUT);
  
  digitalWrite(X_ENCODER_A, HIGH);
  digitalWrite(X_ENCODER_B, HIGH);
  digitalWrite(Y_ENCODER_A, HIGH);
  digitalWrite(Y_ENCODER_B, HIGH);
  digitalWrite(Z_ENCODER_A, HIGH);
  digitalWrite(Z_ENCODER_B, HIGH);
  
  attachInterrupt(4,xEncoderInterrupt,CHANGE);
  attachInterrupt(3,yEncoderInterrupt,CHANGE);
  attachInterrupt(2,zEncoderInterrupt,CHANGE);
}

void xEncoderInterrupt() {
  if (digitalRead(X_ENCODER_A) == digitalRead(X_ENCODER_B)) {
    encoder_x_pos++;
  } else {
    encoder_x_pos--;
  }
}

void yEncoderInterrupt() {
  if (digitalRead(Y_ENCODER_A) == digitalRead(Y_ENCODER_B)) {
    encoder_y_pos++;
  } else {
    encoder_y_pos--;
  }
}

void zEncoderInterrupt() {
  if (digitalRead(Z_ENCODER_A) == digitalRead(Z_ENCODER_B)) {
    encoder_z_pos++;
  } else {
    encoder_z_pos--;
  }
}

double getEncoderDistance(int encoder_num) {
  if (encoder_num == AXIS_X)
    return ((double)encoder_x_pos / ENCODER_CPR * MM_PER_REV / 2);
  else if (encoder_num == AXIS_Y)
    return ((double)encoder_y_pos / ENCODER_CPR * MM_PER_REV / 2);
  else if (encoder_num == AXIS_Z)
    return ((double)encoder_z_pos / ENCODER_CPR * MM_PER_REV / 2);
}
