#include "samples/e_piano.h"
#include "samples/gtr_lo2.h"
#include "samples/glock_hi_c.h"
#include "samples/melo_long_hi_c.h"

#define ROMPLER_NUM_SAMPLES 4

const int16_t *rompler_list[ROMPLER_NUM_SAMPLES] = {
e_piano_DATA,
gtr_lo2_DATA,
glock_hi_c_DATA,
melo_long_hi_c_DATA,
};

uint64_t rompler_lengths[ROMPLER_NUM_SAMPLES] = {
e_piano_NUM_CELLS,
gtr_lo2_NUM_CELLS,
glock_hi_c_NUM_CELLS,
melo_long_hi_c_NUM_CELLS,
};
