/*
  Example that tests the basic harware setup of a M.E.A.P. board.
  
  Plays a constant sine wave at 440Hz and prints to the console 
  whenever a DIP switch or capacitive touch input is pressed.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_midi.h>
#include <Meap.h>
#include <tables/sin8192_int8.h>  // loads sine wavetable

#define CONTROL_RATE 64  // Hz, powers of 2 are most reliable

int button_pins[6] = {15, 16, 12, 13, 18, 35};
int button_vals[7] = {0, 0, 0, 0, 0, 0, 0};
int last_button_vals[7] = {0, 0, 0, 0, 0, 0, 0};

Meap meap;

Oscil<8192, AUDIO_RATE> my_sine(SIN8192_DATA);

void setup() {
  Serial.begin(115200);      // begins Serial communication with computer
  startMozzi(CONTROL_RATE);  // starts Mozzi engine with control rate defined above
  meap.begin();

  for(int i = 0; i < 6; i++){
    pinMode(button_pins[i], INPUT_PULLUP);
  }
}


void loop() {
  audioHook();
}


void updateControl() {
  meap.readPots();
  meap.readTouch();
  meap.readDip();
  meap.readAuxMux();
  updateButtons();

  for (int i = 0; i < 7; i++){
    Serial.print(meap.auxMuxVals[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}


AudioOutput_t updateAudio() {
  int sample = my_sine.next();
  return StereoOutput::from8Bit(sample, sample);
}

void Meap::updateTouch(int number, bool pressed) {
  switch (number) {
    case 0:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed ");
      } else {  // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
      } else {  // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
      } else {  // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
      } else {  // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 5 pressed
        Serial.println("t5 pressed");
      } else {  // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 6 pressed
        Serial.println("t6 pressed");
      } else {  // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 7 pressed
        Serial.println("t7 pressed");
      } else {  // Pad 7 released
        Serial.println("t7 released");
      }
      break;
    case 7:
      if (pressed) {  // Pad 8 pressed
        Serial.println("t8 pressed");
      } else {  // Pad 8 released
        Serial.println("t8 released");
      }
      break;
  }
}

void Meap::updateDip(int number, bool up) {
  switch (number) {
    case 0:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
      } else {  // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 1:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
      } else {  // DIP 2 down
        Serial.println("d2 down");
      }
      break;
    case 2:
      if (up) {  // DIP 3 up
        Serial.println("d3 up");
      } else {  // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 3:
      if (up) {  // DIP 4 up
        Serial.println("d4 up");
      } else {  // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 4:
      if (up) {  // DIP 5 up
        Serial.println("d5 up");
      } else {  // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 5:
      if (up) {  // DIP 6 up
        Serial.println("d6 up");
      } else {  // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 6:
      if (up) {  // DIP 7 up
        Serial.println("d7 up");
      } else {  // DIP 7 down
        Serial.println("d7 down");
      }
      break;
    case 7:
      if (up) {  // DIP 8 up
        Serial.println("d8 up");
      } else {  // DIP 8 down
        Serial.println("d8 down");
      }
      break;
  }
}

void updateButtons(){

  for(int i = 0; i < 7; i++){
    // Read the buttons
    if (i == 0){
      if(meap.auxMuxVals[7] < 1000){
          button_vals[i] = 0;
      } else{
        button_vals[i] = 1;
      }
    } else{
      button_vals[i] = digitalRead(button_pins[i-1]);
    }
    
    // Check if state has changed
    if(button_vals[i] != last_button_vals[i]){
      switch(i){
        case 0:
          if(button_vals[i]){ // button 0 pressed
            Serial.println("Button 0 pressed");
          } else{ // button 0 released
            Serial.println("Button 0 released");
          }
          break;
        case 1:
          if(button_vals[i]){ // button 1 pressed
            Serial.println("Button 1 pressed");
          } else{ // button 1 released
            Serial.println("Button 1 released");
          }
          break;
        case 2:
          if(button_vals[i]){ // button 2 pressed
            Serial.println("Button 2 pressed");
          } else{ // button 2 released
            Serial.println("Button 2 released");
          }
          break;
        case 3:
          if(button_vals[i]){ // button 3 pressed
            Serial.println("Button 3 pressed");
          } else{ // button 3 released
            Serial.println("Button 3 released");
          }
          break;
        case 4:
          if(button_vals[i]){ // button 4 pressed
            Serial.println("Button 4 pressed");
          } else{ // button 4 released
            Serial.println("Button 4 released");
          }
          break;
        case 5:
          if(button_vals[i]){ // button 5 pressed
            Serial.println("Button 5 pressed");
          } else{ // button 5 released
            Serial.println("Button 5 released");
          }
          break;
        case 6:
          if(button_vals[i]){ // button 6 pressed
            Serial.println("Button 6 pressed");
          } else{ // button 6 released
            Serial.println("Button 6 released");
          }
          break;
      }
    }
    last_button_vals[i] = button_vals[i];
  }
}
