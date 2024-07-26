#include "samples/piano.h"
#include "samples/C_maj.h"
#include "samples/tpt.h"
#include "samples/glock.h"
#include "samples/gtr.h"
#include "samples/sine.h"
#include "samples/harp.h"
#include "samples/clari.h"
#include "samples/C_aug.h"
#include "samples/C_dim.h"
#include "samples/C_min.h"


#include "samples/toy_piano.h"
#include "samples/marimba.h"
#include "samples/steel_drum.h"

#include "samples/uke.h"
#include "samples/harp2.h"
#include "samples/tine.h"
#include "samples/lead_gtr.h"

#include "samples/upright_bass.h"

#include "samples/fantasy.h"

#include "samples/strings.h"
#include "samples/vox.h"

#include "samples/e_piano.h"

#include "samples/cmaj_reverb.h"
#include "samples/pistachio_sine.h"

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