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

#include "samples/fyof_chime16.h"
#include "samples/fyof_synth16.h"
#include "samples/cb_synth16.h"



#define NUM_SAMPLES 11

const int16_t *samples_list[NUM_SAMPLES] = { 
fyof_chime16_DATA,  //0
fyof_synth16_DATA, //1
cb_synth16_DATA, //2
e_piano_DATA, // 3
glock_hi_c_DATA, // 4
round_DATA, // 5
anne_sine_DATA, // 6
gtr_lo2_DATA, // 7
eupho_long_lo_c_DATA, // 8
tpt_cup_long_c_DATA, // 9
recorder_DATA, // 10
};


uint64_t samples_lengths[NUM_SAMPLES] = {
fyof_chime16_NUM_CELLS,  
fyof_synth16_NUM_CELLS,
cb_synth16_NUM_CELLS,
e_piano_NUM_CELLS,
glock_hi_c_NUM_CELLS,
round_NUM_CELLS, 
anne_sine_NUM_CELLS, 
gtr_lo2_NUM_CELLS, 
eupho_long_lo_c_NUM_CELLS, 
tpt_cup_long_c_NUM_CELLS, 
};