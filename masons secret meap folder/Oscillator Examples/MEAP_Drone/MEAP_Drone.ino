/*  
  Sine wave drone based around beating patterns between detuned sine waves

  Touch pads activate different pairs of detuned sine waves corresponding to notes in a C major chord

  Each time a pad is pressed, the corresponding sine wave is played with first 8 harmonics 
  at random amplitudes.

  Pot #1 will increase the detuning amount, speeding up

  Mason Mann, CC0
*/

#include <MozziGuts.h>
#include <Mux.h>
#include <Oscil.h>
#include <mozzi_midi.h>
#include <tables/cos8192_int8.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

using namespace admux;

// variables for DIP switches
Mux mux(Pin(34, INPUT, PinType::Digital), Pinset(16, 17, 12));
int dipPins[] = {5, 6, 7, 4, 3, 0, 2, 1};
int dipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevDipVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

// variables for capacitive touch pads
int touchPins[] = {2, 13, 27, 32, 4, 15, 14, 33};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;

// variables for potentiometers
int potVals[] = {0, 0};

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos8(COS8192_DATA);

// duplicates but slightly off frequency for adding to originals
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos8b(COS8192_DATA);

// base pitch frequencies in Q16n16 fixed int format (for speed later)
float f1,f2,f3,f4,f5,f6, f7, f8;//,f7;

float amp1, amp2, amp3, amp4, amp5, amp6, amp7, amp8;

float wobbleOffset = 0;




void setup(){
  startMozzi();
  Serial.begin(115200);
  pinMode(34, INPUT);

//  // select base frequencies using mtof (midi to freq) and fixed-point numbers
//  f1 = mtof(48);
//  f2 = mtof(74);
//  f3 = mtof(64);
//  f4 = mtof(77);
//  f5 = mtof(67);
//  f6 = mtof(57);
//  f7 = mtof(60);
//  f8 = mtof(72);

  // select base frequencies using mtof (midi to freq) and fixed-point numbers
  f1 = mtof(48);
  f2 = mtof(55);
  f3 = mtof(60);
  f4 = mtof(64);
  f5 = mtof(67);
  f6 = mtof(72);
  f7 = mtof(76);
  f8 = mtof(79);

  // set Oscils with chosen frequencies
  aCos1.setFreq(f1);
  aCos2.setFreq(f2);
  aCos3.setFreq(f3);
  aCos4.setFreq(f4);
  aCos5.setFreq(f5);
  aCos6.setFreq(f6);
  aCos7.setFreq(f7);
  aCos8.setFreq(f8);

  // set frequencies of duplicate oscillators
  aCos1b.setFreq(f1 + 1.0f);
  aCos2b.setFreq(f2 + 1.5f);
  aCos3b.setFreq(f3 + 2.0f);
  aCos4b.setFreq(f4 + 2.5f);
  aCos5b.setFreq(f5 + 3.0f);
  aCos6b.setFreq(f6 + 3.5f);
  aCos7b.setFreq(f7 + 4.0f);
  aCos8b.setFreq(f8 + 4.5f);

  // is oscillator on or not
  amp1 = 1;
  amp2 = 1;
  amp3 = 1;
  amp4 = 1;
  amp5 = 1;
  amp6 = 1;
  amp7 = 1;
  amp8 = 1;
}


void loop(){
  audioHook();
}


void updateControl(){
  readDip(); // reads DIP switch65es
  readTouch(); // reads capacitive touch pads
  readPots(); // reads potentiometers

  wobbleOffset = (potVals[0] / 4095.0) * 5;

  
  aCos1b.setFreq((float)(f1 + 1.0 + wobbleOffset));
  aCos2b.setFreq((float)(f2 + 1.5 + wobbleOffset));
  aCos3b.setFreq((float)(f3 + 2.0 + wobbleOffset));
  aCos4b.setFreq((float)(f4 + 2.5 + wobbleOffset));
  aCos5b.setFreq((float)(f5 + 3.0 + wobbleOffset));
  aCos6b.setFreq((float)(f6 + 3.5 + wobbleOffset));
  aCos7b.setFreq((float)(f7 + 4.0 + wobbleOffset));
  aCos8b.setFreq((float)(f8 + 4.5 + wobbleOffset));
}


AudioOutput_t updateAudio(){
  int asig =
    (aCos1.next() + aCos1b.next()) * touchVals[0] +
    (aCos2.next() + aCos2b.next()) * touchVals[1] +
    (aCos3.next() + aCos3b.next()) * touchVals[2] +
    (aCos4.next() + aCos4b.next()) * touchVals[3] +
    (aCos5.next() + aCos5b.next()) * touchVals[4] +
    (aCos6.next() + aCos6b.next()) * touchVals[5] +
    (aCos7.next() + aCos7b.next()) * touchVals[6] +
    (aCos8.next() + aCos8b.next()) * touchVals[7];
  return MonoOutput::fromAlmostNBit(12, asig);
}

void readDip(){
  //Read DIP values using mux
  for (int i = 0; i < 8; i++) {
//    int myVal = mux.read(i);
    mux.read(i); //read once and throw away result (for reliability)
    mux.read(i);
    dipVals[dipPins[i]] = mux.read(i);
  }

  //Check if any switches changed position
  for (int i = 0; i < 8; i++){
//    Serial.print(muxVals[i]); Serial.print(" "); // uncomment if you want to print the current state of all DIP switches
    switch(i){
      case 0:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 1 up");
          } else {
            Serial.println("DIP 1 down");
          }
        }
        break;
      case 1:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 2 up");
          } else {
            Serial.println("DIP 2 down");
          }
        }
        break;
      case 2:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 3 up");
          } else {
            Serial.println("DIP 3 down");
          }
        }
        break;
      case 3:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 4 up");
          } else {
            Serial.println("DIP 4 down");
          }
        }
        break;
      case 4:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 5 up");
          } else {
            Serial.println("DIP 5 down");
          }
        }
        break;
      case 5:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 6 up");
          } else {
            Serial.println("DIP 6 down");
          }
        }
        break;
      case 6:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 7 up");
          } else {
            Serial.println("DIP 7 down");
          }
        }
        break;
      case 7:
        if (dipVals[i] != prevDipVals[i]){
          if(!dipVals[i]){
            Serial.println("DIP 8 up");
          } else {
            Serial.println("DIP 8 down");
          }
        }
        break;   
    }
    prevDipVals[i] = dipVals[i]; // update prevDipVals array
  }
}

void readTouch(){
  int pinVal = 0;
  for (int i = 0; i < 8; i++){
    pinVal = touchRead(touchPins[i]);  
    touchAvgs[i] = 0.6 * touchAvgs[i] + 0.4 * pinVal; 
    if (touchAvgs[i] < touchThreshold){
      touchVals[i] = 1; 
    } else {
      touchVals[i] = 0;
    }
    if (touchVals[i] != prevTouchVals[i]){
      switch(i){
        case 0:
          if(touchVals[i]){ // pad 0 pressed
            Serial.println("pad 0 pressed");
          } else { // pad 0 released
            Serial.println("pad 0 released");
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
          } else { // pad 1 released
            Serial.println("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
          } else { // pad 2 released
            Serial.println("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
          } else { // pad 3 released
            Serial.println("pad 3 released");
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            Serial.println("pad 4 pressed");
          } else { // pad 4 released
            Serial.println("pad 4 released");
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
          } else { // pad 5 released
            Serial.println("pad 5 released");
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
          } else { // pad 6 released
            Serial.println("pad 6 released");
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            Serial.println("pad 7 pressed");
          } else { // pad 7 released
            Serial.println("pad 7 released");
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}

void readPots(){
  mozziAnalogRead(0);
  potVals[0] = mozziAnalogRead(39);
  
  mozziAnalogRead(1);
  potVals[1] = mozziAnalogRead(36);
}
