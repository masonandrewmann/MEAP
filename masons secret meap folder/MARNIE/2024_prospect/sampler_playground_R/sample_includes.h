// #include "samples/piano/cass_piano.h"
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

#include "samples/mel/clip1.h"
#include "samples/mel/clip2.h"
#include "samples/mel/clip3.h"
#include "samples/mel/clip4.h"
#include "samples/mel/clip5.h"
#include "samples/mel/clip6.h"
#include "samples/mel/clip7rev.h"
#include "samples/mel/clip8rev.h"

#include "samples/drones/cb_synth16.h"
#include "samples/drones/cello.h"
#include "samples/drones/clari.h"
#include "samples/drones/pad_hi.h"
#include "samples/drones/pad_lo.h"
#include "samples/drones/violin.h"

#include "samples/drums/balloon.h"
#include "samples/drums/chair.h"
#include "samples/drums/drop.h"
#include "samples/drums/match_light.h"
#include "samples/drums/pillow.h"
#include "samples/drums/ping.h"
#include "samples/drums/root.h"
#include "samples/drums/seeds.h"
#include "samples/drums/soap.h"
#include "samples/drums/suction.h"
#include "samples/drums/who.h"



#define NUM_SAMPLES 36

// add some lower octave drones

const int16_t *samples_list[NUM_SAMPLES] = {
  e_piano_DATA,            // 0
  Glass_Piano_C_DATA,      // 1
  glock_hi_c_DATA,         // 2
  organ_simple_DATA,       // 3
  anne_sine_DATA,          // 4
  soft_sk_DATA,            // 5
  gtr_lo2_DATA,            // 6
  Upright_Bass_C_DATA,     // 7
  eupho_long_lo_c_DATA,    // 8
  recorder_DATA,           // 9
  shuffle_DATA,            // 10
  donn_click_DATA,         // 11
  donn_ring_DATA,          // 12
  womp_DATA,               // 13
  clip1_DATA,              // 14 - guitar pattern
  clip2_DATA,              // 15 - vocal pattern
  clip4_DATA,              // 16 - guitar hit - its okay
  clip5_DATA,              // 17 - layered
  clip6_DATA,              // 18 - choppy
  clip7rev_DATA,           // 19 - reverb vocals
  clip8rev_DATA,           // 20 - reverb gtr
  ambitone_DATA,           // 21
  granularglockenfi_DATA,  // 22
  splashy_DATA,            // 23
  clari_DATA,              // 24
  balloon_DATA, // 25
  chair_DATA, // 26
  drop_DATA, // 27
  match_light_DATA, // 28
  pillow_DATA, // 29
  ping_DATA, // 30
  root_DATA, // 31
  seeds_DATA, // 32
  soap_DATA, // 33
  suction_DATA, // 34
  who_DATA // 35
};

const uint64_t samples_lengths[NUM_SAMPLES] = {
  e_piano_NUM_CELLS,            // 0
  Glass_Piano_C_NUM_CELLS,      // 1
  glock_hi_c_NUM_CELLS,         // 3
  organ_simple_NUM_CELLS,       // 6
  anne_sine_NUM_CELLS,          // 7
  soft_sk_NUM_CELLS,            // 8
  gtr_lo2_NUM_CELLS,            // 9
  Upright_Bass_C_NUM_CELLS,     // 11
  eupho_long_lo_c_NUM_CELLS,    // 12
  recorder_NUM_CELLS,           // 13
  shuffle_NUM_CELLS,            // 14
  donn_click_NUM_CELLS,         // 15
  donn_ring_NUM_CELLS,          // 16
  womp_NUM_CELLS,               // 17
  clip1_NUM_CELLS,              // 18
  clip2_NUM_CELLS,              // 19
  clip4_NUM_CELLS,              // 20
  clip5_NUM_CELLS,              // 21
  clip6_NUM_CELLS,              // 22
  clip7rev_NUM_CELLS,           // 23
  clip8rev_NUM_CELLS,           // 24
  ambitone_NUM_CELLS,           // 25
  granularglockenfi_NUM_CELLS,  // 26
  splashy_NUM_CELLS,            // 27
  clari_NUM_CELLS,
  balloon_NUM_CELLS,
  chair_NUM_CELLS,
  drop_NUM_CELLS,
  match_light_NUM_CELLS,
  pillow_NUM_CELLS,
  ping_NUM_CELLS,
  root_NUM_CELLS,
  seeds_NUM_CELLS,
  soap_NUM_CELLS,
  suction_NUM_CELLS,
  who_NUM_CELLS
};