 
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

#define trigPin 13
#define echoPin 12
#define echoPin2 10

long duration, distance, duration2;

boolean arm1, arm2;
int threshold_up = 1000;
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(echoPin2, INPUT);
}

void loop() {

//  unsigned long start = micros();

  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 8000);
  
  delay(5);

  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration2 = pulseIn(echoPin2, HIGH, 8000);  

  
    Serial.print(duration);
    Serial.print(" \t");
    Serial.println(duration2);

    if(duration > threshold_up) arm1 = true;
    if(duration2 > threshold_up) arm2 = true;

    if((duration < threshold_up) && arm1)
    {
      Serial.println("\t\t\ttrigger 1");
      arm1 = false;
    }
    delay(40);
}
