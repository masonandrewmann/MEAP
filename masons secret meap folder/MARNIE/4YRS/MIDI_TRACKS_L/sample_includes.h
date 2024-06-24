#include "samples/C1_break.h"
#include "samples/D1_uke.h"
#include "samples/E1_sweep.h"
#include "samples/F1_break_chop.h"
#include "samples/G1_perc_high.h"
#include "samples/A1_ma101_kick.h"
#include "samples/B1_pt30_snare.h"
#include "samples/C2_mt800_hhc.h"

#include "samples/upright_bass.h"

#define NUM_SAMPLES 30

#define SUMMER_NUM 8

const int8_t *summer_list[SUMMER_NUM] = {
C1_break_DATA,
D1_uke_DATA,
E1_sweep_DATA,
F1_break_chop_DATA,
G1_perc_high_DATA,
A1_ma101_kick_DATA,
B1_pt30_snare_DATA,
C2_mt800_hhc_DATA
};

uint32_t summer_lengths[SUMMER_NUM] = {
C1_break_NUM_CELLS,
D1_uke_NUM_CELLS,
E1_sweep_NUM_CELLS,
F1_break_chop_NUM_CELLS,
G1_perc_high_NUM_CELLS,
A1_ma101_kick_NUM_CELLS,
B1_pt30_snare_NUM_CELLS,
C2_mt800_hhc_NUM_CELLS
};