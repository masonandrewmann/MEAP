#include "wavetables/acytol.h"
// #include "wavetables/alternating_harmonics.h"
#include "wavetables/analog_table.h"
// #include "wavetables/basic_shapes.h"
#include "wavetables/classic_fade.h"
#include "wavetables/crappy.h"
// #include "wavetables/drink_the_juice.h"
// #include "wavetables/drumtable.h"
#include "wavetables/false_noise.h"
// #include "wavetables/fold.h"
// #include "wavetables/harmonic_series.h"
#include "wavetables/solar.h"
// #include "wavetables/squinch2.h"
#include "wavetables/water.h"

#define NUM_WAVETABLES 7


const int16_t *wavetables_list[NUM_WAVETABLES] = { 
acytol_DATA,
// alternating_harmonics_DATA,
analog_table_DATA,
// basic_shapes_DATA,
classic_fade_DATA,
crappy_DATA,
// drink_the_juice_DATA,
// drumtable_DATA,
false_noise_DATA,
// fold_DATA,
// harmonic_series_DATA,
solar_DATA,
// squinch2_DATA,
water_DATA
};
