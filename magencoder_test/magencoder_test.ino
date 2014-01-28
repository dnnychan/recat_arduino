//#include <SPI.h>

int dataPin = 50;
int clockPin = 52;

int CS_1 = 32;

void setup() {
  Serial.begin(9600);
  
  //SPI.begin();
  
  pinMode(CS_1,OUTPUT);
  pinMode(dataPin,INPUT);
  pinMode(clockPin,OUTPUT);
  
}

void loop() {
  digitalWrite(CS_1,LOW);
  digitalWrite(clockPin,HIGH);
  delay(50);
  //Serial.println(SPI.transfer(0x00));
  Serial.println((shiftIn(dataPin,clockPin,MSBFIRST) << 4) + (shiftIn(dataPin,clockPin,MSBFIRST) >> 4));
  //Serial.println(shiftIn(dataPin,clockPin,MSBFIRST));
  digitalWrite(CS_1,HIGH);
  
  delay(100);
}
