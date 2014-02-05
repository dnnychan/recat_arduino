#include "pins.h"
#include "mag_encoder.h"

void initializeMagEncoder (struct MagEncoder * mag_encoder_1, struct MagEncoder * mag_encoder_2, struct MagEncoder * mag_encoder_3, struct MagEncoder * mag_encoder_4) {
  // calibration values. determine this.
  mag_encoder_1->zero = 1984;
  mag_encoder_1->plus_90 = 2982;
  mag_encoder_1->minus_90 = 930;
  mag_encoder_1->CS_pin = CS_1;
  
  mag_encoder_2->zero = 1984;
  mag_encoder_2->plus_90 = 2982;
  mag_encoder_2->minus_90 = 930;
  mag_encoder_2->CS_pin = CS_2;
  
  mag_encoder_3->zero = 1984;
  mag_encoder_3->plus_90 = 2982;
  mag_encoder_3->minus_90 = 930;
  mag_encoder_3->CS_pin = CS_3;
  
  mag_encoder_4->zero = 1984;
  mag_encoder_4->plus_90 = 2982;
  mag_encoder_4->minus_90 = 930;
  mag_encoder_4->CS_pin = CS_4;
  
  pinMode(CS_1,OUTPUT);
  pinMode(CS_2,OUTPUT);
  pinMode(CS_3,OUTPUT);
  pinMode(CS_4,OUTPUT);
  pinMode(DATAPIN,INPUT);
  pinMode(CLOCKPIN,OUTPUT);
}

double readMagEncoder (struct MagEncoder * mag_encoder) {
  digitalWrite(mag_encoder->CS_pin,LOW);
  digitalWrite(CLOCKPIN,HIGH);
  delay(50);
  
  int raw_data = (shiftIn(DATAPIN,CLOCKPIN,MSBFIRST) << 4) + (shiftIn(DATAPIN,CLOCKPIN,MSBFIRST) >> 4);
  
  digitalWrite(mag_encoder->CS_pin,HIGH);
  
  // only works if plus_90 > zero > minus_90
  if (raw_data < mag_encoder->zero) {
    return (double)(raw_data - mag_encoder->zero) * 90 / (mag_encoder->zero - mag_encoder->minus_90);
  } else {
    return (double)(raw_data - mag_encoder->zero) * 90 / (mag_encoder->plus_90 - mag_encoder->zero);
  }
}