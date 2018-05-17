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

#include "hardware_versions.h"

#define MODE_JUNGLE  0
#define MODE_CARTOON  1
#define MODE_BIRD 2
#define MODE_OCEAN 3
#define MODE_FARM 4
#define MODE_COLORADO 5

#define JUNGLE_TRACKS 8
#define CARTOON_TRACKS 10
#define BIRD_TRACKS 10
#define OCEAN_TRACKS 10
#define FARM_TRACKS 6
#define COLORADO_TRACKS 8 // note these are ducks right now

// Game state variables
byte gameMode = MODE_JUNGLE;

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

long threshold_up = 1000;

int group_start_track = 10;
int group_tracks = 6;
int delay_ms_tsunami_com = 100;

#include <Tsunami.h>            // Include the Tsunami library header
#include <AltSoftSerial.h>    // Optional for using AltSoftSerial

Tsunami tsunami;                // Our Tsunami object

int global_static_timeout_count; // this climbs as we see no jumping at all (people have stopped jumping)
#define GLOBAL_STATIC_TIMEOUT_LIMIT 3000 // about 10 seconds

void setup()
{
  //Setup hardware inputs/outputs. These pins are defined in the hardware_versions header file

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
}

void loop()
{
  checkButton_trampoline(); // this function fires the sounds and changes modes
    
  if(global_static_timeout_count > GLOBAL_STATIC_TIMEOUT_LIMIT)
  {
    Serial.println("Global static timeout");
    global_static_timeout_count = 0; // reset
    gameMode = MODE_JUNGLE;
    delay(1000);
  }  
}


// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
// but the FLAG of that button must have been set to

byte checkButton_trampoline(void)
{
  byte CHOICE = CHOICE_NONE; // if nothing is pressed
  byte start_note = 0; 
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
    //return(CHOICE_RED);
  }
  
  if((T_boolean[0] == 1) && (HIGH_COUNTER_RED < 6)) HIGH_COUNTER_RED++;
  
  //GLOBAL STATIC TIMEOUT STUFF ////////////////////////////////////////////
  if(CHOICE == CHOICE_NONE) global_static_timeout_count++; // keep count
  else global_static_timeout_count = 0; // reset
  Serial.println(global_static_timeout_count);
  
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
        gameMode = MODE_JUNGLE;
        Serial.println("JUNGLE");
        group_start_track = 10;
        group_tracks = JUNGLE_TRACKS;
        return true;
      }
      
      else if((mode_ADC_reading > (button_value[1] - 5)) && (mode_ADC_reading < (button_value[1] + 5)))
      {
        gameMode = MODE_CARTOON;
        Serial.println("CARTOON");
        group_start_track = 20;
        group_tracks = CARTOON_TRACKS;        
        return true;
      }
      
      else if((mode_ADC_reading > (button_value[2] - 5)) && (mode_ADC_reading < (button_value[2] + 5)))
      {
        gameMode = MODE_BIRD;
        Serial.println("BIRD");
        group_start_track = 30;
        group_tracks = BIRD_TRACKS;        
        return true;
      }

      else if((mode_ADC_reading > (button_value[3] - 5)) && (mode_ADC_reading < (button_value[3] + 5)))
      {
        gameMode = MODE_OCEAN;
        Serial.println("OCEAN");
        group_start_track = 40;
        group_tracks = OCEAN_TRACKS;        
        return true;
      }      

      else if((mode_ADC_reading > (button_value[4] - 5)) && (mode_ADC_reading < (button_value[4] + 5)))
      {
        gameMode = MODE_FARM;
        Serial.println("FARM");
        group_start_track = 50;
        group_tracks = FARM_TRACKS;        
        return true;
      }            

      else if((mode_ADC_reading > (button_value[5] - 5)) && (mode_ADC_reading < (button_value[5] + 5)))
      {
        gameMode = MODE_COLORADO;
        Serial.println("COLORADO");
        group_start_track = 70;
        group_tracks = COLORADO_TRACKS;        
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


