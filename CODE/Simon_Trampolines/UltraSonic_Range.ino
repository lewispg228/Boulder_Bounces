 
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

long thresh_up = 900;

void ultra_sonic_test() {

//  unsigned long start = micros();

  duration = readDistance(echoPin);
  duration2 = readDistance(echoPin2);
  duration3 = readDistance(echoPin3);
  duration4 = readDistance(echoPin4);

    Serial.print(duration);
    Serial.print(" \t");
    Serial.print(duration2);
    Serial.print(" \t");
    Serial.print(duration3);
    Serial.print(" \t");
    Serial.println(duration4);

//    if(duration > threshold_up) arm1 = true;
//    if(duration2 > threshold_up) arm2 = true;
//
//    if((duration < threshold_up) && arm1)
//    {
//      Serial.println("\t\t\ttrigger 1");
//      arm1 = false;
//    }
    delay(40);
}

long readDistance(int pin)
{
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
  else if(trampoline == 4) trampoline = echoPin4;
  if(readDistance(trampoline) > thresh_up) return true;
  else return false;
}

void digitalReadTrampoline_test()
{
    Serial.print(digitalReadTrampoline(1));
    Serial.print(" \t");
    Serial.print(digitalReadTrampoline(2));
    Serial.print(" \t");
    Serial.print(digitalReadTrampoline(3));
    Serial.print(" \t");
    Serial.println(digitalReadTrampoline(4));
}

