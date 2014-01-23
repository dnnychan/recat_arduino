void setup () {
  pinMode(10,OUTPUT);
  int myEraser = 7;             // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  int myPrescaler = 1;         // this could be a number in [1 , 6].   
  TCCR2B |= myPrescaler;  //this operation (OR), replaces the last three bits in TCCR2B with our new value 011
}

void loop() {
  analogWrite(10,30);
}
