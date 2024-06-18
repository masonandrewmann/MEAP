#include "samples/piano/cass_piano.h"
#include "samples/piano/e_piano.h"
#include "samples/piano/Glass_Piano_C.h"
// #include "samples/piano/piano_4C.h"
#include "samples/piano/toy_piano_c.h"

#include "samples/c_perc/bell_tree.h"
// #include "samples/c_perc/cass_bell.h"
// #include "samples/c_perc/celesta.h"
#include "samples/c_perc/glock_hi_c.h"
// #include "samples/c_perc/Marimba_C.h"
// #include "samples/c_perc/Marimba_Rev_C.h"
#include "samples/c_perc/milkjar.h"
#include "samples/c_perc/round.h"
#include "samples/c_perc/steeldrum.h"
// #include "samples/c_perc/tine.h"
// #include "samples/c_perc/valavophone.h"
// #include "samples/c_perc/valveophone_hand.h"

// #include "samples/Organ/buzzyOrgan.h"
#include "samples/Organ/mellowtron.h"
// #include "samples/Organ/microwave_organ.h"
#include "samples/Organ/organ_simple.h"
// #include "samples/Organ/organ.h"
// #include "samples/Organ/rainbows.h"
// #include "samples/Organ/rezOrgan.h"
#include "samples/Organ/shorgan.h"

#include "samples/Synth/ambitone.h"
#include "samples/Synth/anne_sine.h"
#include "samples/Synth/belton.h"
// #include "samples/Synth/bubblic.h"
// #include "samples/Synth/cassambi.h"
// #include "samples/Synth/digitalridoo.h"
// #include "samples/Synth/echoesyn.h"
// #include "samples/Synth/FaeryTale.h"
#include "samples/Synth/flutterbug.h"
// #include "samples/Synth/fyof2_chime.h"
// // #include "samples/Synth/fyof2_synth.h"
#include "samples/Synth/granularglockenfi.h"
// // #include "samples/Synth/JahrMarkt2.h"
// #include "samples/Synth/kiksynth.h"
// // #include "samples/Synth/KorgBow.h"
// #include "samples/Synth/lobassy.h"
// #include "samples/Synth/outer.h"
// #include "samples/Synth/padlet.h"
// #include "samples/Synth/pcmchip.h"
#include "samples/Synth/shoresine.h"
#include "samples/Synth/soft_sk.h"
// // #include "samples/Synth/squaccordion.h"
// #include "samples/Synth/strawman.h"
// // #include "samples/Synth/vocal_samp_C.h"
// #include "samples/Synth/whisper_pad.h"

#include "samples/Guitar/gtr_lo2.h"
#include "samples/Guitar/koto_choir.h"
#include "samples/Guitar/uke_c_hi.h"

#include "samples/Bass/Upright_Bass_C.h"

#include "samples/Strings/Strings2.h"

#include "samples/Brass/eupho_long_lo_c.h"
#include "samples/Brass/tpt_cup_long_c.h"

#include "samples/Pipe/andes.h"
#include "samples/Pipe/recorder.h"

#include "samples/Effect/splashy.h"
#include "samples/Effect/shuffle.h"

#include "samples/Percussion/donn_click.h"
#include "samples/Percussion/donn_ring.h"
#include "samples/Percussion/womp.h"



#define NUM_SAMPLES 33

const int16_t *samples_list[NUM_SAMPLES] = { 
cass_piano_DATA, // 0
e_piano_DATA, // 1
Glass_Piano_C_DATA, // 2
toy_piano_c_DATA, // 4
// bell_tree_DATA, // 5
glock_hi_c_DATA, // 8
milkjar_DATA, // 11
round_DATA, // 12
steeldrum_DATA, // 13
// mellowtron_DATA, // 18
organ_simple_DATA, // 20
// shorgan_DATA, // 24
ambitone_DATA, // 25
anne_sine_DATA, // 26
// belton_DATA, // 27
// flutterbug_DATA, // 33
granularglockenfi_DATA, // 36
shoresine_DATA, // 44
soft_sk_DATA, // 45
gtr_lo2_DATA, // 45
// koto_choir_DATA, // 45
// uke_c_hi_DATA, // 45
Upright_Bass_C_DATA, // 45
Strings2_DATA, // 45
eupho_long_lo_c_DATA, // 45
tpt_cup_long_c_DATA, // 45
// andes_DATA, // 45
recorder_DATA, // 45
splashy_DATA, // 45
shuffle_DATA, // 45
donn_click_DATA, // 45
donn_ring_DATA, // 45
womp_DATA, // 45
};


const uint64_t samples_lengths[NUM_SAMPLES] = {
cass_piano_NUM_CELLS, // 0
e_piano_NUM_CELLS, // 1
Glass_Piano_C_NUM_CELLS, // 2
toy_piano_c_NUM_CELLS, // 4
// bell_tree_NUM_CELLS, // 5
glock_hi_c_NUM_CELLS, // 8
milkjar_NUM_CELLS, // 11
round_NUM_CELLS, // 12
steeldrum_NUM_CELLS, // 13
// mellowtron_NUM_CELLS, // 18
organ_simple_NUM_CELLS, // 20
// shorgan_NUM_CELLS, // 24
ambitone_NUM_CELLS, // 25
anne_sine_NUM_CELLS, // 26
// belton_NUM_CELLS, // 27
// flutterbug_NUM_CELLS, // 33
granularglockenfi_NUM_CELLS, // 36
shoresine_NUM_CELLS, // 44
soft_sk_NUM_CELLS, // 45
gtr_lo2_NUM_CELLS, // 45
// koto_choir_NUM_CELLS, // 45
// uke_c_hi_NUM_CELLS, // 45
Upright_Bass_C_NUM_CELLS, // 45
Strings2_NUM_CELLS, // 45
eupho_long_lo_c_NUM_CELLS, // 45
tpt_cup_long_c_NUM_CELLS, // 45
// andes_NUM_CELLS, // 45
recorder_NUM_CELLS, // 45
splashy_NUM_CELLS, // 45
shuffle_NUM_CELLS, // 45
donn_click_NUM_CELLS, // 45
donn_ring_NUM_CELLS, // 45
womp_NUM_CELLS, // 45
};