#include "pins.h"
#include "mag_encoder.h"

// initialize all encoders at once
void initializeMagEncoder (struct MagEncoder * mag_encoder_1, struct MagEncoder * mag_encoder_2, struct MagEncoder * mag_encoder_3, struct MagEncoder * mag_encoder_4) {
  // calibration values. determined in mag_encoder.xlsx
  
  mag_encoder_1->m1 = -0.0892;
  mag_encoder_1->m2 = -0.0892;
  mag_encoder_1->b1 = 59.799;
  mag_encoder_1->b2 = 59.799;
  mag_encoder_1->zero = 2725;
  mag_encoder_1->plus_90 = 3736;
  mag_encoder_1->minus_90 = 1690;
  mag_encoder_1->CS_pin = CS_1;
  
  mag_encoder_2->m1 = -0.09226;
  mag_encoder_2->m2 = -0.0884;
  mag_encoder_2->b1 = 76.91179;
  mag_encoder_2->b2 = 439.19;
  mag_encoder_2->zero = 849;
  mag_encoder_2->plus_90 = 1855;
  mag_encoder_2->minus_90 = 3938;
  mag_encoder_2->CS_pin = CS_2;
  
  mag_encoder_3->m1 = -0.0893;
  mag_encoder_3->m2 = -0.0945;
  mag_encoder_3->b1 = 3.573;
  mag_encoder_3->b2 = 390.37;
  mag_encoder_3->zero = 3162;
  mag_encoder_3->plus_90 = 32;
  mag_encoder_3->minus_90 = 2017;
  mag_encoder_3->CS_pin = CS_3;
  
  mag_encoder_4->m1 = -0.0925;
  mag_encoder_4->m2 = -0.0898;
  mag_encoder_4->b1 = -34.829;
  mag_encoder_4->b2 = 335.12;
  mag_encoder_4->zero = 3720;
  mag_encoder_4->plus_90 = 657;
  mag_encoder_4->minus_90 = 2670;
  mag_encoder_4->CS_pin = CS_4;
  
  pinMode(CS_1,OUTPUT);
  pinMode(CS_2,OUTPUT);
  pinMode(CS_3,OUTPUT);
  pinMode(CS_4,OUTPUT);
  pinMode(DATAPIN,INPUT);
  pinMode(CLOCKPIN,OUTPUT);
  digitalWrite(CS_1,HIGH);
  digitalWrite(CS_2,HIGH);
  digitalWrite(CS_3,HIGH);
  digitalWrite(CS_4,HIGH);
}

double readMagEncoder (struct MagEncoder * mag_encoder) {
  // read the magnetic encoder
  
  // turn on the desired chip select pin, and change the clock to high. this causes the encoder to start sending data
  digitalWrite(mag_encoder->CS_pin,LOW);
  digitalWrite(CLOCKPIN,HIGH);
  delay(50);
  
  // this function reads the data from the spi-like port using the clock and data pins. also converts the 10 bit data to an int
  int raw_data = (shiftIn(DATAPIN,CLOCKPIN,MSBFIRST) << 4) + (shiftIn(DATAPIN,CLOCKPIN,MSBFIRST) >> 4);
  
  digitalWrite(mag_encoder->CS_pin,HIGH);
  //return raw_data;    //uncomment for raw data
  
  // use equations
  if (raw_data < 2048)
    return raw_data * mag_encoder->m1 + mag_encoder->b1;
  else
    return raw_data * mag_encoder->m2 + mag_encoder->b2;
    
    //stuff below is obsolete
  /*
  // deals with wrapping around for encoder readings
  if (raw_data < mag_encoder->zero && raw_data > mag_encoder->minus_90) {
    return (double)(raw_data - mag_encoder->zero) * 90 / (mag_encoder->zero - mag_encoder->minus_90);
    //return (double)(raw_data - mag_encoder->zero) * 1.570796327 / abs(mag_encoder->zero - mag_encoder->minus_90);
  } else if (raw_data > mag_encoder->zero && raw_data < mag_encoder->plus_90) {
    return (double)(raw_data - mag_encoder->zero) * 90 / (mag_encoder->plus_90 - mag_encoder->zero);
    //return (double)(raw_data - mag_encoder->zero) * 1.570796327 / abs(mag_encoder->plus_90 - mag_encoder->zero);
  } else if (mag_encoder->plus_90 < mag_encoder->zero) {
    if (raw_data > mag_encoder->zero) {
      return (double)(raw_data - mag_encoder->zero) * 90 / (4096 - mag_encoder->zero + mag_encoder->plus_90);
    } else if (raw_data < mag_encoder->plus_90) {
      return (double)(4096 - mag_encoder->zero + raw_data) * 90 / (4096 - mag_encoder->zero + mag_encoder->plus_90);
    }    
  } else if (mag_encoder->minus_90 > mag_encoder->zero) {
    if (raw_data < mag_encoder->zero) {
      return (double)(raw_data - mag_encoder->zero) * 90 / (4096 - mag_encoder->minus_90 + mag_encoder->zero);
    } else if (raw_data > mag_encoder->minus_90) {
      return (double)(raw_data - 4096 - mag_encoder->zero) * 90 / (4096 - mag_encoder->minus_90 + mag_encoder->zero);
    }
  } else // out of range
    return 0;*/
}
