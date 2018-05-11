 
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

#define trigPin 12
#define echoPin 2
#define echoPin2 3
#define echoPin3 4
#define echoPin4 8

long duration, duration2, duration3, duration4;

boolean arm1, arm2;
int threshold_up = 1000;
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);
  pinMode(echoPin4, INPUT);
}

void loop() {

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

