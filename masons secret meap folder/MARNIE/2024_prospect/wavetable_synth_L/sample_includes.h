#include "loops/loop1.h"
#include "loops/loop2.h"
#include "loops/loop3.h"
#include "loops/loop4.h"
#include "loops/loop5.h"
#include "loops/loop6.h"
#include "loops/loop7.h"
#include "loops/loop8.h"
#include "loops/yann_c.h"

#include "samples/piano.h"
#include "samples/gtr.h"
#include "samples/sine.h"
#include "samples/steel_drum.h"
#include "samples/fantasy.h"
#include "samples/glock.h"



const int16_t *loop_list[8] = { 
  loop1_DATA,
  loop2_DATA,
  loop3_DATA,
  loop4_DATA,
  yann_c_DATA,
  loop6_DATA,
  loop7_DATA,
  loop8_DATA,
};

const uint64_t loop_lengths[8] = { 
  loop1_NUM_CELLS,
  loop2_NUM_CELLS,
  loop3_NUM_CELLS,
  loop4_NUM_CELLS,
  yann_c_NUM_CELLS,
  loop6_NUM_CELLS,
  loop7_NUM_CELLS,
  loop8_NUM_CELLS,
};


#define NUM_SAMPLES 6

const int8_t *samples_list[NUM_SAMPLES] = { 
piano_DATA, // 0 -   piano
gtr_DATA,   // 1 -   guitar
sine_DATA,  // 2 -   sine
steel_drum_DATA, // 3 - Steel Drum
fantasy_DATA, // 4 - Fantasy
glock_DATA, // 5 - Glock
};


const uint64_t samples_lengths[NUM_SAMPLES] = {
piano_NUM_CELLS, // 0 -   piano
gtr_NUM_CELLS,   // 1 -   guitar
sine_NUM_CELLS,  // 2 -   sine
steel_drum_NUM_CELLS, // 3 - Steel Drum
fantasy_NUM_CELLS, // 4 - Fantasy
glock_NUM_CELLS, // 5 - Glock
};