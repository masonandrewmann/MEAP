#include "samples/summer/C1_break.h"
#include "samples/summer/D1_uke.h"
#include "samples/summer/E1_sweep.h"
#include "samples/summer/F1_break_chop.h"
#include "samples/summer/G1_perc_high.h"
#include "samples/summer/A1_ma101_kick.h"
#include "samples/summer/B1_pt30_snare.h"
#include "samples/summer/C2_mt800_hhc.h"

// #include "samples/upright_bass.h"

#include "samples/indie/m0_casio_kick.h"
#include "samples/indie/m1_casio_snare.h"
#include "samples/indie/m2_chh.h"
#include "samples/indie/m3_ohh.h"
#include "samples/indie/m4_tom.h"
#include "samples/indie/m5_big_kick.h"
#include "samples/indie/m6_amen.h"

#include "samples/indie/round.h"

#define SUMMER_NUM_SAMPLES 8

const int8_t *summer_list[SUMMER_NUM_SAMPLES] = {
C1_break_DATA,
D1_uke_DATA,
E1_sweep_DATA,
F1_break_chop_DATA,
G1_perc_high_DATA,
A1_ma101_kick_DATA,
B1_pt30_snare_DATA,
C2_mt800_hhc_DATA
};

uint32_t summer_lengths[SUMMER_NUM_SAMPLES] = {
C1_break_NUM_CELLS,
D1_uke_NUM_CELLS,
E1_sweep_NUM_CELLS,
F1_break_chop_NUM_CELLS,
G1_perc_high_NUM_CELLS,
A1_ma101_kick_NUM_CELLS,
B1_pt30_snare_NUM_CELLS,
C2_mt800_hhc_NUM_CELLS
};


#define INDIE_NUM_SAMPLES 7

const int8_t *indie_list[INDIE_NUM_SAMPLES] = {
m0_casio_kick_DATA,
m1_casio_snare_DATA,
m2_chh_DATA,
m3_ohh_DATA,
m4_tom_DATA,
m5_big_kick_DATA,
m6_amen_DATA,
};

uint32_t indie_lengths[INDIE_NUM_SAMPLES] = {
m0_casio_kick_NUM_CELLS,
m1_casio_snare_NUM_CELLS,
m2_chh_NUM_CELLS,
m3_ohh_NUM_CELLS,
m4_tom_NUM_CELLS,
m5_big_kick_NUM_CELLS,
m6_amen_NUM_CELLS
};

#include "samples/collar/delay_drums/cbm1_kick.h"
#include "samples/collar/delay_drums/cbm2_hat.h"
#include "samples/collar/delay_drums/cbm3_ride.h"
#include "samples/collar/delay_drums/cbm4_snare.h"
#include "samples/collar/delay_drums/cbm5_rim.h"
#include "samples/collar/delay_drums/cbm6_rim2.h"

#define COLLAR_DELAY_NUM_SAMPLES 6

const int8_t *collar_delay_list[COLLAR_DELAY_NUM_SAMPLES] = {
cbm1_kick_DATA,
cbm2_hat_DATA,
cbm3_ride_DATA,
cbm4_snare_DATA,
cbm5_rim_DATA,
cbm6_rim2_DATA
};

uint32_t collar_delay_lengths[COLLAR_DELAY_NUM_SAMPLES] = {
cbm1_kick_NUM_CELLS,
cbm2_hat_NUM_CELLS,
cbm3_ride_NUM_CELLS,
cbm4_snare_NUM_CELLS,
cbm5_rim_NUM_CELLS,
cbm6_rim2_NUM_CELLS
};

#include "samples/collar/aux_drums/cba1_ride.h"
#include "samples/collar/aux_drums/cba2_rim.h"
#include "samples/collar/aux_drums/cba3_shaker.h"
#include "samples/collar/aux_drums/cba4_clave.h"
#include "samples/collar/aux_drums/cba5_rd.h"
#include "samples/collar/aux_drums/cba6_triangle.h"
#include "samples/collar/aux_drums/cba7_snare.h"
#include "samples/collar/aux_drums/cba8_kick.h"

#define COLLAR_AUX_NUM_SAMPLES 7

const int8_t *collar_aux_list[COLLAR_AUX_NUM_SAMPLES] = {
cba1_ride_DATA,
cba2_rim_DATA,
cba3_shaker_DATA,
cba4_clave_DATA,
// cba5_rd_DATA,
cba6_triangle_DATA,
cba7_snare_DATA,
cba8_kick_DATA
};

uint32_t collar_aux_lengths[COLLAR_AUX_NUM_SAMPLES] = {
cba1_ride_NUM_CELLS,
cba2_rim_NUM_CELLS,
cba3_shaker_NUM_CELLS,
cba4_clave_NUM_CELLS,
// cba5_rd_NUM_CELLS,
cba6_triangle_NUM_CELLS,
cba7_snare_NUM_CELLS,
cba8_kick_NUM_CELLS
};

#include "samples/collar/samples/cb_s0.h"
#include "samples/collar/samples/cb_s1.h"
#include "samples/collar/samples/cb_s2.h"
#include "samples/collar/samples/cb_s3.h"
#include "samples/collar/samples/cb_s4.h"
#include "samples/collar/samples/cb_s5.h"
#include "samples/collar/samples/cb_s6.h"

#define COLLAR_SPEECH_SAMPLES 7

const int8_t *collar_speech_list[COLLAR_SPEECH_SAMPLES] = {
cb_s0_DATA,
cb_s1_DATA,
cb_s2_DATA,
cb_s3_DATA,
cb_s4_DATA,
cb_s5_DATA,
cb_s6_DATA
};

uint32_t collar_speech_lengths[COLLAR_SPEECH_SAMPLES] = {
cb_s0_NUM_CELLS,
cb_s1_NUM_CELLS,
cb_s2_NUM_CELLS,
cb_s3_NUM_CELLS,
cb_s4_NUM_CELLS,
cb_s5_NUM_CELLS,
cb_s6_NUM_CELLS
};


#include "samples/sealed/sl_0kick.h"
#include "samples/sealed/sl_1kick.h"
#include "samples/sealed/sl_2snare.h"
#include "samples/sealed/sl_3chh.h"
#include "samples/sealed/sl_4chh.h"
#include "samples/sealed/sl_5ohh.h"
#include "samples/sealed/sl_6chh.h"
#include "samples/sealed/sl_7chh.h"
#include "samples/sealed/sl_8cym.h"
#include "samples/sealed/sl_9samp1.h"
#include "samples/sealed/sl_10samp2.h"
#include "samples/sealed/sl_11samp3.h"
#include "samples/sealed/sl_12shgl.h"
#include "samples/sealed/sl_13lgl.h"

#define SEALED_NUM_SAMPLES 14

const int8_t *sealed_samples_list[SEALED_NUM_SAMPLES] = {
sl_0kick_DATA,
sl_1kick_DATA,
sl_2snare_DATA,
sl_3chh_DATA,
sl_4chh_DATA,
sl_5ohh_DATA,
sl_6chh_DATA,
sl_7chh_DATA,
sl_8cym_DATA,
sl_9samp1_DATA,
sl_10samp2_DATA,
sl_11samp3_DATA,
sl_12shgl_DATA,
sl_13lgl_DATA
};

uint32_t sealed_samples_length[SEALED_NUM_SAMPLES] = {
sl_0kick_NUM_CELLS,
sl_1kick_NUM_CELLS,
sl_2snare_NUM_CELLS,
sl_3chh_NUM_CELLS,
sl_4chh_NUM_CELLS,
sl_5ohh_NUM_CELLS,
sl_6chh_NUM_CELLS,
sl_7chh_NUM_CELLS,
sl_8cym_NUM_CELLS,
sl_9samp1_NUM_CELLS,
sl_10samp2_NUM_CELLS,
sl_11samp3_NUM_CELLS,
sl_12shgl_NUM_CELLS,
sl_13lgl_NUM_CELLS
};


