 
/*
HC-SR04 Ping distance sensor]
VCC to arduino 5v GND to arduino GND
Echo to Arduino pin 13 Trig to Arduino pin 12
Red POS to Arduino pin 11
Green POS to Arduino pin 10
560 ohm resistor to both LED NEG and GRD power rail
More info at: http://goo.gl/kJ8Gl
Original code improvements to the Ping sketch sourced from Trollmaker.com
Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
*/

long thresh_up = 1400;
long readDistance(int pin)
{
  delay(4);
  check_mode_buttons();
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  return pulseIn(pin, HIGH, 8000);
  delay(5);
}

boolean digitalReadTrampoline(int trampoline)
{
  if(trampoline == 1) trampoline = echoPin;
  else if(trampoline == 2) trampoline = echoPin2;
  else if(trampoline == 3) trampoline = echoPin3;
//  else if(trampoline == 4) trampoline = echoPin4;
  if(readDistance(trampoline) > thresh_up) return true;
  else return false;
}

void digitalReadTrampoline_test()
{
    Serial.print(digitalReadTrampoline(1));
    Serial.print(" \t");
    Serial.print(digitalReadTrampoline(2));
    Serial.print(" \t");
    Serial.println(digitalReadTrampoline(3));
//    Serial.print(" \t");
//    Serial.println(digitalReadTrampoline(4));
}

void read_T_distances()
{
  T_distance[0] = readDistance(echoPin);
  T_distance[1] = readDistance(echoPin2);
  T_distance[2] = readDistance(echoPin3);
//  T_distance[3] = readDistance(echoPin4);
}

void set_T_booleans()
{
  for(int i = 0 ; i < 3 ; i++)
  {
    if(T_distance[i] > thresh_up) T_boolean[i] = true;
    //else if(T_distance[i] < 200) T_boolean[i] = true; // bad reading, probably still high
    else T_boolean[i] = false;
  }
}

void print_data()
{
    for(int i = 0 ; i < 3 ; i++)
    {
      Serial.print(T_distance[i]); 
      Serial.print(" \t");
    }
    for(int i = 0 ; i < 3 ; i++)
    {
      if(T_boolean[i]) Serial.print(T_boolean[i]);
      else Serial.print(" "); // visually, this is easy to see a space, than a "0"
      Serial.print(" \t");
    }    
    Serial.print(HIGH_COUNTER_RED);
    Serial.print(" \t");
    Serial.print(HIGH_COUNTER_GREEN);
    Serial.print(" \t");
    Serial.print(HIGH_COUNTER_BLUE);
    Serial.print(" \t");
//    Serial.print(HIGH_COUNTER_YELLOW);
//    Serial.print(" \t");
    
    Serial.println();
}

