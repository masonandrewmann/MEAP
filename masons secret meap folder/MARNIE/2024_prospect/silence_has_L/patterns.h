#ifndef PATTERNS_H_
#define PATTERNS_H_

#include "midiNotes.h"

#define NUM_PATTERN_BANKS 1

SamplePattern** patterns = new SamplePattern*[NUM_PATTERN_BANKS];

uint8_t* pattern_note[NUM_PATTERN_BANKS][8];  // array of pointers to note arrays
uint16_t* pattern_dur[NUM_PATTERN_BANKS][8];  // array of pointers to dur arrays
uint16_t* pattern_sus[NUM_PATTERN_BANKS][8];  // array of pointers to sus arrays
uint16_t* pattern_amp[NUM_PATTERN_BANKS][8];  // array of pointers to amp arrays

// tempo of each song
// 1 - silence has its posibilities: 80
uint16_t bank_tempos[NUM_PATTERN_BANKS] = {120};

// setup: step, repeats, sample, MIDI channel num, pan
// ---------- BANK 0 START ---------- SILENCE HAS ITS POSSIBILITIES

// Bank 0 Pattern 0 ----- sine arps
int16_t p_0_0_setup[5] = { 32, -1, 0, -1, 0 };  // anne sine
uint8_t p_0_0_note[32] = { 48, 52, 55, 59, 60, 59, 55, 52, 48, 52, 55, 59, 60, 59, 55, 52, 48, 53, 57, 60, 64, 60, 57, 53, 48, 53, 57, 60, 64, 60, 57, 53 };
uint16_t p_0_0_dur[32] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
uint16_t p_0_0_sus[32] = { 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800 };
uint16_t p_0_0_amp[32] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 1 ----- gtr 1
int16_t p_0_1_setup[5] = { 5, -1, 1, -1, -1 };
uint8_t p_0_1_note[5] = { 67, 67, 60, 67, 69 };
uint16_t p_0_1_dur[5] = { 6, 2, 2, 4, 18 };
uint16_t p_0_1_sus[5] = { 800, 800, 800, 800, 800 };
uint16_t p_0_1_amp[5] = { 127, 127, 127, 127, 127 };

// Bank 0 Pattern 2 ----- gtr 2
int16_t p_0_2_setup[5] = { 4, -1, 1, -1, 1  };
uint8_t p_0_2_note[4] = { 64, 64, 64, 64 };
uint16_t p_0_2_dur[4] = { 6, 4, 18, 4 };
uint16_t p_0_2_sus[4] = { 800, 800, 800, 800 };
uint16_t p_0_2_amp[4] = { 127, 127, 127, 127 };

// Bank 0 Pattern 3 ----- piano
int16_t p_0_3_setup[5] = { 7, -1, 0, -1, 0 };
uint8_t p_0_3_note[7] = { 0, 48, 52, 50, 43, 52, 45 };
uint16_t p_0_3_dur[7] = { 2, 2, 2, 2, 12, 4, 8 };
uint16_t p_0_3_sus[7] = { 800, 800, 800, 800, 800, 800, 800 };
uint16_t p_0_3_amp[7] = { 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 4 ---- ending steel drum arp
int16_t p_0_4_setup[5] = { 3, -1, 3, -1, -1 };
uint8_t p_0_4_note[3] = { m_C2, m_E2, m_G2 };
uint16_t p_0_4_dur[3] = { 4, 2, 2};
uint16_t p_0_4_sus[3] = { 200, 200, 200 };
uint16_t p_0_4_amp[3] = { 127, 127, 127};

// Bank 0 Pattern 5 -- 6/4 arp
int16_t p_0_5_setup[5] = { 6, -1, 4, -1, 1 };
uint8_t p_0_5_note[6] = { m_C1, m_E1, m_G1, m_E2, m_C2, m_B1 };
uint16_t p_0_5_dur[6] = { 2, 2, 2, 2, 2, 2 };
uint16_t p_0_5_sus[6] = { 200, 200, 200, 200, 200, 200};
uint16_t p_0_5_amp[6] = { 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 6
int16_t p_0_6_setup[5] = { 6, -1, 4, -1, 1 };
uint8_t p_0_6_note[6] = { m_C1, m_E1, m_G1, m_E2, m_C2, m_B1 };
uint16_t p_0_6_dur[6] = { 2, 2, 2, 2, 2, 2 };
uint16_t p_0_6_sus[6] = { 200, 200, 200, 200, 200, 200};
uint16_t p_0_6_amp[6] = { 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 7
int16_t p_0_7_setup[5] = { 6, -1, 4, -1, 1 };
uint8_t p_0_7_note[6] = { m_C1, m_E1, m_G1, m_E2, m_C2, m_B1 };
uint16_t p_0_7_dur[6] = { 2, 2, 2, 2, 2, 2 };
uint16_t p_0_7_sus[6] = { 200, 200, 200, 200, 200, 200};
uint16_t p_0_7_amp[6] = { 127, 127, 127, 127, 127, 127 };

// ---------- BANK 0 END ----------

// ---------- BANK 1 END ----------

void patternSetup() {

  // allocate pattern objects
  for (int i = 0; i < NUM_PATTERN_BANKS; ++i) {
    patterns[i] = new SamplePattern[8];
  }

  // ---------- BANK 1 SETUP START ----------

  // run pattern constructors
  patterns[0][0] = SamplePattern(p_0_0_setup[0], p_0_0_setup[1], p_0_0_setup[2], p_0_0_setup[3], p_0_0_setup[4]);
  patterns[0][1] = SamplePattern(p_0_1_setup[0], p_0_1_setup[1], p_0_1_setup[2], p_0_1_setup[3], p_0_1_setup[4]);
  patterns[0][2] = SamplePattern(p_0_2_setup[0], p_0_2_setup[1], p_0_2_setup[2], p_0_2_setup[3], p_0_2_setup[4]);
  patterns[0][3] = SamplePattern(p_0_3_setup[0], p_0_3_setup[1], p_0_3_setup[2], p_0_3_setup[3], p_0_3_setup[4]);
  patterns[0][4] = SamplePattern(p_0_4_setup[0], p_0_4_setup[1], p_0_4_setup[2], p_0_4_setup[3], p_0_4_setup[4]);
  patterns[0][5] = SamplePattern(p_0_5_setup[0], p_0_5_setup[1], p_0_5_setup[2], p_0_5_setup[3], p_0_5_setup[4]);
  patterns[0][6] = SamplePattern(p_0_6_setup[0], p_0_6_setup[1], p_0_6_setup[2], p_0_6_setup[3], p_0_6_setup[4]);
  patterns[0][7] = SamplePattern(p_0_7_setup[0], p_0_7_setup[1], p_0_7_setup[2], p_0_7_setup[3], p_0_7_setup[4]);

  // build pattern notes array
  pattern_note[0][0] = p_0_0_note;
  pattern_note[0][1] = p_0_1_note;
  pattern_note[0][2] = p_0_2_note;
  pattern_note[0][3] = p_0_3_note;
  pattern_note[0][4] = p_0_4_note;
  pattern_note[0][5] = p_0_5_note;
  pattern_note[0][6] = p_0_6_note;
  pattern_note[0][7] = p_0_7_note;

  // build pattern dur array
  pattern_dur[0][0] = p_0_0_dur;
  pattern_dur[0][1] = p_0_1_dur;
  pattern_dur[0][2] = p_0_2_dur;
  pattern_dur[0][3] = p_0_3_dur;
  pattern_dur[0][4] = p_0_4_dur;
  pattern_dur[0][5] = p_0_5_dur;
  pattern_dur[0][6] = p_0_6_dur;
  pattern_dur[0][7] = p_0_7_dur;

  // build pattern sus array
  pattern_sus[0][0] = p_0_0_sus;
  pattern_sus[0][1] = p_0_1_sus;
  pattern_sus[0][2] = p_0_2_sus;
  pattern_sus[0][3] = p_0_3_sus;
  pattern_sus[0][4] = p_0_4_sus;
  pattern_sus[0][5] = p_0_5_sus;
  pattern_sus[0][6] = p_0_6_sus;
  pattern_sus[0][7] = p_0_7_sus;

  // build pattern amp array
  pattern_amp[0][0] = p_0_0_amp;
  pattern_amp[0][1] = p_0_1_amp;
  pattern_amp[0][2] = p_0_2_amp;
  pattern_amp[0][3] = p_0_3_amp;
  pattern_amp[0][4] = p_0_4_amp;
  pattern_amp[0][5] = p_0_5_amp;
  pattern_amp[0][6] = p_0_6_amp;
  pattern_amp[0][7] = p_0_7_amp;

  // ---------- BANK 1 SETUP END ----------
  // send pattern notes, dur, sus and amp to objects
  for (int bank = 0; bank < NUM_PATTERN_BANKS; bank++) {
    for (int i = 0; i < 8; i++) {
      patterns[bank][i].note = pattern_note[bank][i];
      patterns[bank][i].sus = pattern_sus[bank][i];
      patterns[bank][i].dur = pattern_dur[bank][i];
      patterns[bank][i].amp = pattern_amp[bank][i];
    }
  }
}
#endif  //PATTERNS_H_