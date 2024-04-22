#ifndef PATTERNS_H_
#define PATTERNS_H_

#include "midiNotes.h"

#define NUM_PATTERN_BANKS 2

// SamplePattern *patterns = new SamplePattern[8];  // array of 8 patterns , just one bank for now


SamplePattern** patterns = new SamplePattern*[NUM_PATTERN_BANKS];

// int** a = new int*[rowCount];
// for(int i = 0; i < rowCount; ++i)
//     a[i] = new int[colCount];



uint8_t* pattern_note[NUM_PATTERN_BANKS][8];  // array of pointers to note arrays
uint16_t* pattern_dur[NUM_PATTERN_BANKS][8];  // array of pointers to dur arrays
uint16_t* pattern_sus[NUM_PATTERN_BANKS][8];  // array of pointers to sus arrays
uint16_t* pattern_amp[NUM_PATTERN_BANKS][8];  // array of pointers to amp arrays

// which patterns start with start button
bool pattern_auto_start[NUM_PATTERN_BANKS][8] = {
  {false, false, false, false, false, false, false, false},
  {true, false, true, true, false, false, false, false},
};

// int pattern_drum_num[NUM_PATTERN_BANKS] = {0, 4};


// ---------- BANK 0 START ----------

// Bank 0 Pattern 0 -- lead synth short
int16_t p_0_0_setup[4] = { 9, -1, 5, -1 };
uint8_t p_0_0_note[9] = { 68, 66, 68, 66, 63, 63, 61, 63, 66 };
uint16_t p_0_0_dur[9] = { 18, 2, 6, 4, 2, 16, 4, 4, 8 };
uint16_t p_0_0_sus[9] = { 1800, 300, 800, 400, 129, 1600, 400, 400, 800 };
uint16_t p_0_0_amp[9] = { 120, 120, 120, 120, 120, 120, 120, 120, 120 };


// Bank 0 Pattern 1 -- lead synth long
int16_t p_0_1_setup[4] = { 57, -1, 5, -1 };
uint8_t p_0_1_note[57] = { 68, 66, 68, 66, 63, 63, 61, 63, 66, 68, 66, 68, 66, 63, 63, 61, 63, 66, 68, 66, 68, 66, 63, 63, 61, 63, 66, 68, 66, 68, 66, 63, 63, 61, 63, 66, 68, 66, 68, 66, 63, 63, 61, 63, 66, 68, 66, 63, 61, 66, 63, 61, 68, 56, 61, 63, 59 };
uint16_t p_0_1_dur[57] = { 18, 2, 6, 4, 2, 16, 4, 4, 8, 18, 2, 6, 4, 2, 16, 4, 4, 8, 18, 2, 6, 4, 2, 16, 4, 4, 8, 18, 2, 6, 4, 2, 16, 4, 4, 8, 18, 2, 6, 4, 2, 16, 4, 4, 8, 8, 2, 5, 3, 8, 3, 12, 2, 2, 4, 4, 8 };
uint16_t p_0_1_sus[57] = { 1800, 300, 800, 400, 129, 1600, 400, 400, 800, 1800, 300, 800, 400, 129, 1600, 400, 400, 800, 1800, 300, 800, 400, 129, 1600, 400, 400, 800, 1800, 300, 800, 400, 129, 1600, 400, 400, 800, 1800, 300, 800, 400, 129, 1600, 400, 400, 800, 800, 250, 550, 300, 800, 229, 1212, 200, 200, 400, 400, 800 };
uint16_t p_0_1_amp[57] = { 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 100, 100, 120, 120, 120, 120, 100, 100, 120, 120, 100 };

// Bank 0 Pattern 2
int16_t p_0_2_setup[4] = { 20, -1, 1, -1 };
uint8_t p_0_2_note[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65 };
uint16_t p_0_2_dur[20] = { 2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1 };
uint16_t p_0_2_sus[20] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
uint16_t p_0_2_amp[20] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 3
int16_t p_0_3_setup[4] = { 20, -1, 2, -1 };
uint8_t p_0_3_note[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65 };
uint16_t p_0_3_dur[20] = { 2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1 };
uint16_t p_0_3_sus[20] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
uint16_t p_0_3_amp[20] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 4
int16_t p_0_4_setup[4] = { 20, -1, 3, -1 };
uint8_t p_0_4_note[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65 };
uint16_t p_0_4_dur[20] = { 2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1 };
uint16_t p_0_4_sus[20] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
uint16_t p_0_4_amp[20] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 5
int16_t p_0_5_setup[4] = { 20, -1, 4, -1 };
uint8_t p_0_5_note[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65 };
uint16_t p_0_5_dur[20] = { 2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1 };
uint16_t p_0_5_sus[20] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
uint16_t p_0_5_amp[20] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 6
int16_t p_0_6_setup[4] = { 20, -1, 5, -1 };
uint8_t p_0_6_note[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65 };
uint16_t p_0_6_dur[20] = { 2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1 };
uint16_t p_0_6_sus[20] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
uint16_t p_0_6_amp[20] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 0 Pattern 7
int16_t p_0_7_setup[4] = { 20, -1, 6, -1 };
uint8_t p_0_7_note[20] = { 60, 63, 65, 70, 67, 67, 63, 63, 67, 65, 67, 65, 70, 72, 70, 67, 63, 63, 65, 65 };
uint16_t p_0_7_dur[20] = { 2, 2, 1, 3, 1, 1, 1, 6, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1 };
uint16_t p_0_7_sus[20] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
uint16_t p_0_7_amp[20] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// ---------- BANK 0 END ----------

// ---------- BANK 1 START ----------

// Bank 1 Pattern 0 -- guitar notes
int16_t p_1_0_setup[4] = { 32, -1, 4, 1 };
uint8_t p_1_0_note[32] = { m_A3, m_D4, m_A3, m_D4, m_A3, m_D4, m_A3, m_D4, m_A3, m_Cs4, m_A3, m_Cs4, m_A3, m_Cs4, m_A3, m_Cs4, m_B3, m_D4, m_B3, m_D4, m_B3, m_D4, m_B3, m_D4, m_B3, m_D4, m_B3, m_D4, m_B3, m_D4, m_B3, m_D4};
uint16_t p_1_0_dur[32] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
uint16_t p_1_0_sus[32] = { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 };
uint16_t p_1_0_amp[32] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 1 Pattern 1 -- e piano
int16_t p_1_1_setup[4] = { 39, -1, 33, 1 };
uint8_t p_1_1_note[39] = { m_D3, m_E3, m_Fs3, m_G3, m_Fs3, m_E3, m_D3, m_E3, m_B2, m_D3, m_A3, m_E3, m_Fs3, m_D3, m_E3, m_G3, m_D3, m_E3, m_Fs3, m_Fs3, m_D3, m_A3, m_Fs3, m_G3, m_Fs3, m_E3, m_Cs3, m_D3, m_E3, m_B2, m_G2, m_Fs3, m_D3, m_A3, m_G3, m_E3, m_Cs3, m_D3, m_E3 };
uint16_t p_1_1_dur[39] = { 2, 4, 4, 4, 4, 4, 6, 4, 6, 26, 6, 10, 6, 6, 4, 12, 4, 6, 10, 2, 4, 6, 4, 2, 4, 6, 4, 6, 10, 6, 10, 6, 6, 4, 6, 6, 4, 16, 16};
uint16_t p_1_1_sus[39] = { 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 };
uint16_t p_1_1_amp[39] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

// Bank 1 Pattern 2 -- piano lo
int16_t p_1_2_setup[4] = { 4, -1, 0, -1 };
uint8_t p_1_2_note[4] = { 45, 45, 47, 43 };
uint16_t p_1_2_dur[4] = { 16, 16, 16, 16 };
uint16_t p_1_2_sus[4] = { 1751, 1724, 1751, 1788 };
uint16_t p_1_2_amp[4] = { 120, 120, 120, 120 };


// Bank 1 Pattern 3 -- piano high
int16_t p_1_3_setup[4] = { 11, -1, 0, -1 };
uint8_t p_1_3_note[11] = { 50, 49, 55, 57, 55, 50, 49, 55, 50, 52, 50 };
uint16_t p_1_3_dur[11] = { 16, 16, 16, 8, 8, 16, 16, 20, 4, 4, 4 };
uint16_t p_1_3_sus[11] = { 1788, 1724, 1788, 907, 843, 1788, 1724, 2226, 437, 437, 437 };
uint16_t p_1_3_amp[11] = { 120, 120, 120, 120, 120, 120, 120, 120, 118, 118, 118 };

// Bank 1 Pattern 4 -- glockenspiel
int16_t p_1_4_setup[4] = { 32, -1, 3, -1 };
uint8_t p_1_4_note[32] = { m_D3, m_A2, m_D3, m_A2, m_D3, m_A2, m_D3, m_A2, m_Cs3, m_A3, m_Cs3, m_A3, m_Cs3, m_A3, m_Cs3, m_A3, m_E3, m_B2, m_E3, m_B2, m_E3, m_B2, m_E3, m_B2, m_E3, m_B2, m_E3, m_B2, m_E3, m_B2, m_E3, m_B2};
uint16_t p_1_4_dur[32] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
uint16_t p_1_4_sus[32] = { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 };
uint16_t p_1_4_amp[32] = { 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };



// Bank 1 Pattern 5
int16_t p_1_5_setup[4] = { 4, -1, 3, -1 };
uint8_t p_1_5_note[4] = { 60, 62, 64, 67 };
uint16_t p_1_5_dur[4] = { 4, 4, 4, 4 };
uint16_t p_1_5_sus[4] = { 400, 400, 400, 400 };
uint16_t p_1_5_amp[4] = { 127, 127, 127, 127 };

// Bank 1 Pattern 6
int16_t p_1_6_setup[4] = { 4, -1, 3, -1 };
uint8_t p_1_6_note[4] = { 60, 62, 64, 67 };
uint16_t p_1_6_dur[4] = { 4, 4, 4, 4 };
uint16_t p_1_6_sus[4] = { 400, 400, 400, 400 };
uint16_t p_1_6_amp[4] = { 127, 127, 127, 127 };

// Bank 1 Pattern 7
int16_t p_1_7_setup[4] = { 4, -1, 3, -1 };
uint8_t p_1_7_note[4] = { 60, 62, 64, 67 };
uint16_t p_1_7_dur[4] = { 4, 4, 4, 4 };
uint16_t p_1_7_sus[4] = { 400, 400, 400, 400 };
uint16_t p_1_7_amp[4] = { 127, 127, 127, 127 };

// ---------- BANK 1 END ----------

void patternSetup() {

  // allocate pattern objects
  for (int i = 0; i < NUM_PATTERN_BANKS; ++i) {
    patterns[i] = new SamplePattern[8];
  }

  // ---------- BANK 1 SETUP START ----------

  // run pattern constructors
  patterns[0][0] = SamplePattern(p_0_0_setup[0], p_0_0_setup[1], p_0_0_setup[2], p_0_0_setup[3]);
  patterns[0][1] = SamplePattern(p_0_1_setup[0], p_0_1_setup[1], p_0_1_setup[2], p_0_1_setup[3]);
  patterns[0][2] = SamplePattern(p_0_2_setup[0], p_0_2_setup[1], p_0_2_setup[2], p_0_2_setup[3]);
  patterns[0][3] = SamplePattern(p_0_3_setup[0], p_0_3_setup[1], p_0_3_setup[2], p_0_3_setup[3]);
  patterns[0][4] = SamplePattern(p_0_4_setup[0], p_0_4_setup[1], p_0_4_setup[2], p_0_4_setup[3]);
  patterns[0][5] = SamplePattern(p_0_5_setup[0], p_0_5_setup[1], p_0_5_setup[2], p_0_5_setup[3]);
  patterns[0][6] = SamplePattern(p_0_6_setup[0], p_0_6_setup[1], p_0_6_setup[2], p_0_6_setup[3]);
  patterns[0][7] = SamplePattern(p_0_7_setup[0], p_0_7_setup[1], p_0_7_setup[2], p_0_7_setup[3]);

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

  // ---------- BANK 2 SETUP START ----------

  // run pattern constructors
  patterns[1][0] = SamplePattern(p_1_0_setup[0], p_1_0_setup[1], p_1_0_setup[2], p_1_0_setup[3]);
  patterns[1][1] = SamplePattern(p_1_1_setup[0], p_1_1_setup[1], p_1_1_setup[2], p_1_1_setup[3]);
  patterns[1][2] = SamplePattern(p_1_2_setup[0], p_1_2_setup[1], p_1_2_setup[2], p_1_2_setup[3]);
  patterns[1][3] = SamplePattern(p_1_3_setup[0], p_1_3_setup[1], p_1_3_setup[2], p_1_3_setup[3]);
  patterns[1][4] = SamplePattern(p_1_4_setup[0], p_1_4_setup[1], p_1_4_setup[2], p_1_4_setup[3]);
  patterns[1][5] = SamplePattern(p_1_5_setup[0], p_1_5_setup[1], p_1_5_setup[2], p_1_5_setup[3]);
  patterns[1][6] = SamplePattern(p_1_6_setup[0], p_1_6_setup[1], p_1_6_setup[2], p_1_6_setup[3]);
  patterns[1][7] = SamplePattern(p_1_7_setup[0], p_1_7_setup[1], p_1_7_setup[2], p_1_7_setup[3]);

  // build pattern notes array
  pattern_note[1][0] = p_1_0_note;
  pattern_note[1][1] = p_1_1_note;
  pattern_note[1][2] = p_1_2_note;
  pattern_note[1][3] = p_1_3_note;
  pattern_note[1][4] = p_1_4_note;
  pattern_note[1][5] = p_1_5_note;
  pattern_note[1][6] = p_1_6_note;
  pattern_note[1][7] = p_1_7_note;

  // build pattern dur array
  pattern_dur[1][0] = p_1_0_dur;
  pattern_dur[1][1] = p_1_1_dur;
  pattern_dur[1][2] = p_1_2_dur;
  pattern_dur[1][3] = p_1_3_dur;
  pattern_dur[1][4] = p_1_4_dur;
  pattern_dur[1][5] = p_1_5_dur;
  pattern_dur[1][6] = p_1_6_dur;
  pattern_dur[1][7] = p_1_7_dur;

  // build pattern sus array
  pattern_sus[1][0] = p_1_0_sus;
  pattern_sus[1][1] = p_1_1_sus;
  pattern_sus[1][2] = p_1_2_sus;
  pattern_sus[1][3] = p_1_3_sus;
  pattern_sus[1][4] = p_1_4_sus;
  pattern_sus[1][5] = p_1_5_sus;
  pattern_sus[1][6] = p_1_6_sus;
  pattern_sus[1][7] = p_1_7_sus;


  // build pattern amp array
  pattern_amp[1][0] = p_1_0_amp;
  pattern_amp[1][1] = p_1_1_amp;
  pattern_amp[1][2] = p_1_2_amp;
  pattern_amp[1][3] = p_1_3_amp;
  pattern_amp[1][4] = p_1_4_amp;
  pattern_amp[1][5] = p_1_5_amp;
  pattern_amp[1][6] = p_1_6_amp;
  pattern_amp[1][7] = p_1_7_amp;

  // ---------- BANK 2 SETUP END ----------



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