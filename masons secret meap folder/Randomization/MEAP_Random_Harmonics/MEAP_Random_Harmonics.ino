/*
  Basic demo of randomization. 

  Touch pads function as a basic C major keyboard.

  Each time a pad is pressed, the corresponding sine wave is played with first 8 harmonics 
  at random amplitudes.

  Mason Mann, CC0
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <tables/sin8192_int8.h> // loads sine wavetable

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

// variables for capacitive touch pads
int touchPins[] = {2, 13, 27, 32, 4, 15, 14, 33};
int touchAvgs[] = {100, 100, 100, 100, 100, 100, 100, 100};
int touchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int prevTouchVals[] = {0, 0, 0, 0, 0, 0, 0, 0};
int touchThreshold = 20;

// variables for potentiometers
int potVals[] = {0, 0};

Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc1(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc2(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc3(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc4(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc5(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc6(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc7(SIN8192_DATA);
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> osc8(SIN8192_DATA);

int amp1 = 255;
int amp2 = 255;
int amp3 = 255;
int amp4 = 255;
int amp5 = 255;
int amp6 = 255;
int amp7 = 255;
int amp8 = 255;


void setup(){
  Serial.begin(115200);
  pinMode(34, INPUT);
  startMozzi();
  setOscFreqs(mtof(48));

  randomSeed(10);
  xorshiftSeed((long)random(1000));
  
}


void loop(){
  audioHook();
}


void updateControl(){
  readTouch(); // reads capacitive touch pads
}


int updateAudio(){
//  uint16_t myVal = mySine.next();
  return MonoOutput::fromAlmostNBit(16, 
    osc1.next() * amp1 + 
    osc2.next() * amp2 + 
    osc3.next() * amp3 + 
    osc4.next() * amp4 + 
    osc5.next() * amp5 + 
    osc6.next() * amp6 + 
    osc7.next() * amp7 + 
    osc8.next() * amp8);
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
            setOscFreqs(mtof(48));
            randomizeHarmonics();
          } else { // pad 0 released
            Serial.println("pad 0 released");
          }
          break;
        case 1:
          if(touchVals[i]){ // pad 1 pressed
            Serial.println("pad 1 pressed");
            setOscFreqs(mtof(50));
            randomizeHarmonics();
          } else { // pad 1 released
            Serial.println("pad 1 released");
          }
          break;
        case 2:
          if(touchVals[i]){ // pad 2 pressed
            Serial.println("pad 2 pressed");
            setOscFreqs(mtof(52));
            randomizeHarmonics();
          } else { // pad 2 released
            Serial.println("pad 2 released");
          }
          break;
        case 3:
          if(touchVals[i]){ // pad 3 pressed
            Serial.println("pad 3 pressed");
            setOscFreqs(mtof(53));
            randomizeHarmonics();
          } else { // pad 3 released
            Serial.println("pad 3 released");
          }
          break;
        case 4:
          if(touchVals[i]){ // pad 4 pressed
            Serial.println("pad 4 pressed");
            setOscFreqs(mtof(55));
            randomizeHarmonics();
          } else { // pad 4 released
            Serial.println("pad 4 released");
          }
          break;
        case 5:
          if(touchVals[i]){ // pad 5 pressed
            Serial.println("pad 5 pressed");
            setOscFreqs(mtof(57));
            randomizeHarmonics();
          } else { // pad 5 released
            Serial.println("pad 5 released");
          }
          break;
        case 6:
          if(touchVals[i]){ // pad 6 pressed
            Serial.println("pad 6 pressed");
            setOscFreqs(mtof(59));
            randomizeHarmonics();
          } else { // pad 6 released
            Serial.println("pad 6 released");
          }
          break;
        case 7:
          if(touchVals[i]){ // pad 7 pressed
            Serial.println("pad 7 pressed");
            setOscFreqs(mtof(60));
            randomizeHarmonics();
          } else { // pad 7 released
            Serial.println("pad 7 released");
          }
          break;
      }
    }
    prevTouchVals[i] = touchVals[i];
  }
}

void setOscFreqs(int root){
  osc1.setFreq(root); //set frequency of sine oscillator
  osc2.setFreq(root * 2); //set frequency of sine oscillator
  osc3.setFreq(root * 3); //set frequency of sine oscillator
  osc4.setFreq(root * 4); //set frequency of sine oscillator
  osc5.setFreq(root * 5); //set frequency of sine oscillator
  osc6.setFreq(root * 6); //set frequency of sine oscillator
  osc7.setFreq(root * 7); //set frequency of sine oscillator
  osc8.setFreq(root * 8); //set frequency of sine oscillator
}

void randomizeHarmonics(){
  uint16_t myRandom = xorshift96();
  amp1 = map(myRandom, 0, 65535, 0, 255);
  Serial.println(amp1);

  myRandom = xorshift96();
  amp2 = map(myRandom, 0, 65535, 0, 255) / 1.5;
  Serial.println(amp2);

  myRandom = xorshift96();
  amp3 = map(myRandom, 0, 65535, 0, 255) / 2;
  Serial.println(amp3);

  myRandom = xorshift96();
  amp4 = map(myRandom, 0, 65535, 0, 255) / 2.5;
  Serial.println(amp4);

  myRandom = xorshift96();
  amp5 = map(myRandom, 0, 65535, 0, 255) / 3;
  Serial.println(amp5);

  myRandom = xorshift96();
  amp6 = map(myRandom, 0, 65535, 0, 255) / 3.5;
  Serial.println(amp6);

  myRandom = xorshift96();
  amp7 = map(myRandom, 0, 65535, 0, 255) / 4;
  Serial.println(amp7);

  myRandom = xorshift96();
  amp8 = map(myRandom, 0, 65535, 0, 255) / 4.5;
  Serial.println(amp8);

  
}
