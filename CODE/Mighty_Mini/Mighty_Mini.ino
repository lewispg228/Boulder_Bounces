/*
 Started: 6-19-2007
 Spark Fun Electronics
 Nathan Seidle
 
 Simon Says is a memory game. Start the game by pressing one of the four buttons. When a button lights up, 
 press the button, repeating the sequence. The sequence will get longer and longer. The game is won after 
 13 rounds.
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Simon Says game originally written in C for the PIC16F88.
 Ported for the ATmega168, then ATmega328, then Arduino 1.0.
 Fixes and cleanup by Joshua Neal <joshua[at]trochotron.com>
 
 Generates random sequence, plays music, and displays button lights.
 
 Simon tones from Wikipedia
 - A (red, upper left) - 440Hz - 2.272ms - 1.136ms pulse
 - a (green, upper right, an octave higher than A) - 880Hz - 1.136ms,
 0.568ms pulse
 - D (blue, lower left, a perfect fourth higher than the upper left) 
 587.33Hz - 1.702ms - 0.851ms pulse
 - G (yellow, lower right, a perfect fourth higher than the lower left) - 
 784Hz - 1.276ms - 0.638ms pulse
 
 The tones are close, but probably off a bit, but they sound all right.
 
 The old version of SparkFun simon used an ATmega8. An ATmega8 ships
 with a default internal 1MHz oscillator.  You will need to set the
 internal fuses to operate at the correct external 16MHz oscillator.
 
 Original Fuses:
 avrdude -p atmega8 -P lpt1 -c stk200 -U lfuse:w:0xE1:m -U hfuse:w:0xD9:m
 
 Command to set to fuses to use external 16MHz: 
 avrdude -p atmega8 -P lpt1 -c stk200 -U lfuse:w:0xEE:m -U hfuse:w:0xC9:m
 
 The current version of Simon uses the ATmega328. The external osciallator
 was removed to reduce component count.  This version of simon relies on the
 internal default 1MHz osciallator. Do not set the external fuses.

 */


   /*
 Controlling large 7-segment displays
 By: Nathan Seidle
 SparkFun Electronics
 Date: February 25th, 2015
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This code demonstrates how to post two numbers to a 2-digit display usings two large digit driver boards.

 Here's how to hook up the Arduino pins to the Large Digit Driver IN

 Arduino pin 6 -> CLK (Green on the 6-pin cable)
 5 -> LAT (Blue)
 7 -> SER on the IN side (Yellow)
 5V -> 5V (Orange)
 Power Arduino with 12V and connect to Vin -> 12V (Red)
 GND -> GND (Black)

 There are two connectors on the Large Digit Driver. 'IN' is the input side that should be connected to
 your microcontroller (the Arduino). 'OUT' is the output side that should be connected to the 'IN' of addtional
 digits.

 Each display will use about 150mA with all segments and decimal point on.

*/




#include "hardware_versions.h"

#define MODE_DUCK  0
#define MODE_DRUMS  1

#define DUCK_TRACKS 8
#define DRUMS_TRACKS 10

// Game state variables
byte gameMode = MODE_DUCK;

int button_leds[] = {13,10}; // 1 and 3

// TRAMPOLINE EXTENTION STUFF
  
// FOUR variables to keep track of each trampoline status.
// To register a true jump on a trampoline, 
// we are looking for when the trampoline changes from HIGH (not standing on it) to LOW (standing on it)
// This is different than bressing a button,
// Because when pressing a button, we wait for the release moment to actually trigger the button
// When jumping on trampolines, we want to the trigger point to be exactly when a tampoline transitions
// from HIGH to LOW. This means that the player could still hold the previous trampoline LOW 
// (i.e. that could have one foot on the previous step)
// and still use their other foot to trigger the next step.
// These four variables keep track of how long each trampoline has been in the HIGH state
// Doing this, allows us to require that a trampoline be in the HIGH state for a certain
// amount of time. This helps debounce and prevent false triggering.

long T_distance[4] = {0,0,0,0};
boolean T_boolean[4] = {0,0,0,0};

// 3/2/2016, initiating all of these to 0, so that you can start a round standing on one trampoline, or any combo.

int HIGH_COUNTER_RED = 0;
int HIGH_COUNTER_GREEN = 0;
int HIGH_COUNTER_BLUE = 0;
int HIGH_COUNTER_YELLOW = 0;

// ULTRA_SONIC RANGE FINDER SETUP STUFF
#define trigPin 12
#define echoPin 2
#define echoPin2 3
#define echoPin3 4
#define echoPin4 8

int mode_ADC_reading;
int button_value[7] = {90, 168, 234, 290, 339, 381, 419};

byte bounceCount = 0;
boolean bounceCount_active = false;

long threshold_up = 1000;

int group_start_track = 70; // start on ducks - if no button is pressed on a hard micro reset
int group_tracks = 6;
int delay_ms_tsunami_com = 100;

#include <Tsunami.h>            // Include the Tsunami library header
#include <AltSoftSerial.h>    // Optional for using AltSoftSerial

Tsunami tsunami;                // Our Tsunami object

int global_static_timeout_count; // this climbs as we see no jumping at all (people have stopped jumping)
#define GLOBAL_STATIC_TIMEOUT_LIMIT 3000 // about 10 seconds


//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;


void setup()
{
  //Setup hardware inputs/outputs. These pins are defined in the hardware_versions header file

  // button LEDs aka mode leds
  for(int i = 0 ; i < 2 ; i++ )
  {
    pinMode(button_leds[i], OUTPUT);
    digitalWrite(button_leds[i], LOW);
  }

  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);
  
  // ULTRA_SONIC RANGE FINDER SETUP STUFF
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);
  pinMode(echoPin4, INPUT);  

  Serial.begin(115200);
  Serial.println("HOP AND HEAR Debug ON"); 

  tsunami.start();
  delay(delay_ms_tsunami_com);
  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  tsunami.stopAllTracks();
  delay(delay_ms_tsunami_com);  
  tsunami.samplerateOffset(0, 0);
  delay(delay_ms_tsunami_com);  
  // disable track reporting from the Tsunami
  tsunami.setReporting(false);
  delay(delay_ms_tsunami_com);  
  // Call update on the Tsunami to keep the track playing status current.
  tsunami.update();  
  delay(delay_ms_tsunami_com);  
  tsunami.masterGain(0, 0);              // Reset the master gain to 0dB
  delay(delay_ms_tsunami_com);  
  test_audio_playback();

  clear_display();
  delay(1000);
  set_button_leds();
}

void loop()
{
  checkButton_trampoline(); // this function fires the sounds and changes modes
    
  if(global_static_timeout_count > GLOBAL_STATIC_TIMEOUT_LIMIT)
  {
    Serial.println("Global static timeout");
    global_static_timeout_count = 0; // reset
    bounceCount = 0; // reset
    bounceCount_active = false;
    clear_display();
    gameMode = MODE_DUCK;
    set_button_leds();
    delay(1000);
  }  
}


// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
// but the FLAG of that button must have been set to

byte checkButton_trampoline(void)
{
  byte CHOICE = CHOICE_NONE; // if nothing is pressed
  //ultra_sonic_test();
  //digitalReadTrampoline_test();
  read_T_distances();
  set_T_booleans();
  //print_data();
  /////////////////RED
  if ((T_boolean[0] == 0) && (HIGH_COUNTER_RED > 5)) // if FLAG_RED is set to true, that means that it has been release at some point previously.
  {
    HIGH_COUNTER_RED = 0; // reset counter
    play_track(group_start_track + random(0, (group_tracks+1)));
    CHOICE = CHOICE_RED;
    if(bounceCount_active == true)
    {
      bounceCount++;
      showNumber(bounceCount);
      Serial.println(bounceCount);      
    }
  }
  
  if((T_boolean[0] == 1) && (HIGH_COUNTER_RED < 6)) HIGH_COUNTER_RED++;
  
  //GLOBAL STATIC TIMEOUT STUFF ////////////////////////////////////////////
  if(CHOICE == CHOICE_NONE) global_static_timeout_count++; // keep count
  else global_static_timeout_count = 0; // reset
  //Serial.println(global_static_timeout_count);
  
  return(CHOICE); // If no button is pressed, this will be default CHOICE_NONE, but if something is pressed, then it will be set in the IFs above.
}

boolean check_mode_buttons()
{
  mode_ADC_reading = analogRead(A7);
  if(mode_ADC_reading > 900) return false; // no button being pressed
  else
  {
    Serial.println(mode_ADC_reading);
    // take some more readings and average them, to avoid false button presses.
    // this requires that the button be held down a little more solidly
    int total_readings = 0;
    for (int i = 0 ; i < 10 ; i++)
    {
      total_readings += analogRead(A7);
      delay(10);
    }
    mode_ADC_reading = (total_readings/10);
    Serial.println(mode_ADC_reading);
      if((mode_ADC_reading > (button_value[0] - 5)) && (mode_ADC_reading < (button_value[0] + 5)))
      {
        gameMode = MODE_DUCK;
        Serial.println("DUCK");
        group_start_track = 70;
        group_tracks = DUCK_TRACKS;
        set_button_leds();
        return true;
      }
      
      else if((mode_ADC_reading > (button_value[2] - 5)) && (mode_ADC_reading < (button_value[2] + 5)))
      {
        gameMode = MODE_DRUMS;
        Serial.println("DRUMS");
        group_start_track = 60;
        group_tracks = DRUMS_TRACKS;     
        set_button_leds();   
        return true;
      }

      else if((mode_ADC_reading > (button_value[4] - 5)) && (mode_ADC_reading < (button_value[4] + 5)))
      {
        Serial.println("BOUNCE_COUNT_RESET");
        bounceCount = 0;     
        bounceCount_active = true;
        showNumber(bounceCount);
        return true;
      }      
                   
      else
      {
        Serial.println("BAD VALUE");
        return false;
      }
  }
}

void mode_button_calibration()
{
  Serial.println(analogRead(A7));
  delay(50);
  /*
   * 
   * These are the readings I'm seeing on my first MAIN board built up.
   * open: 1023
   * B1: 90
   * B2: 168
   * B3: 234
   * B4: 290
   * B5: 339
   * B6: 381
   * B7: 419
   */
}

void test_audio_playback()
{
  for(int i = 10 ; i < 11 ; i++)
  {
    Serial.print("track: ");
    Serial.println(i);
    tsunami.trackGain(i, -5);
    delay(delay_ms_tsunami_com);
    tsunami.trackPlayPoly(i, 0, false); 
    delay(1000);                  
  }
}

void play_track(int track)
{
    Serial.print("track: ");
    Serial.println(track);
    tsunami.trackGain(track, -5);
    delay(delay_ms_tsunami_com);
    tsunami.trackPlayPoly(track, 0, false); 
    delay(delay_ms_tsunami_com);
}


//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value)
{
  int number = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);

    number /= 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case 10: segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

void clear_display(void)
{
  
    //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, LOW);
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK

    //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, LOW);
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK  
}


void set_button_leds(void)
{
  int select;
  for(int i = 0 ; i < 2 ; i++ ) 
  {
    digitalWrite(button_leds[i], LOW);
  }
  
  if(gameMode == MODE_DUCK) select = 0;
  else if(gameMode == MODE_DRUMS) select = 1;
  
  digitalWrite(button_leds[select], HIGH);
}

