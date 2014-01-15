int step_pin_x = 52;
int dir_pin_x  = 53;
int step_pin_y = 50;
int dir_pin_y  = 51;
int step_pin_z = 48;
int dir_pin_z  = 49;

int enable_x = 43;
int enable_y = 45;
int enable_z = 47;

int default_speed = 15;
int a = 0;

void stepOnce(int step_pin, int step_speed) {
  digitalWrite(step_pin, HIGH);   
  delay(step_speed);               
  digitalWrite(step_pin, LOW);  
  delay(step_speed);     
}
  
void setup () {
  pinMode(step_pin_x, OUTPUT);
  pinMode(dir_pin_x, OUTPUT);  
  pinMode(step_pin_y, OUTPUT);
  pinMode(dir_pin_y, OUTPUT);     
  pinMode(step_pin_z, OUTPUT);
  pinMode(dir_pin_z, OUTPUT);
  pinMode(enable_x, OUTPUT); 
  pinMode(enable_y, OUTPUT); 
  pinMode(enable_z, OUTPUT);   
  
  digitalWrite(enable_x,LOW);
  digitalWrite(enable_y,LOW);
  digitalWrite(enable_z,LOW);
                 
  digitalWrite(dir_pin_x, LOW);
  digitalWrite(dir_pin_y, LOW);
  digitalWrite(dir_pin_z, LOW);
  digitalWrite(step_pin_x, LOW);
  digitalWrite(step_pin_y, LOW);
  digitalWrite(step_pin_z, LOW);
  
  Serial.begin(9600);
}

void loop() 
{
  if (a <  24)  //sweep 200 step in dir 1
   {
    a++;
    stepOnce(step_pin_z,default_speed);            
   }
   else if (a == 24)
   {
     digitalWrite(enable_z,LOW);
     delay(1000);
     digitalWrite(enable_z,HIGH);
     a++;
   }
  else 
   {
    digitalWrite(dir_pin_z, HIGH);
    a++;
    stepOnce(step_pin_z,default_speed);
    
    if (a>49)    //sweep 200 in dir 2
     {
      a = 0;
      digitalWrite(enable_z,LOW);
      delay(2000);
      digitalWrite(enable_z,HIGH);
      digitalWrite(dir_pin_z, LOW);
     }
    }
    Serial.println(a);
}
