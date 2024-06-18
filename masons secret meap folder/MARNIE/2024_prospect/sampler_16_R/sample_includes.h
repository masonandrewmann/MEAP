// #include "samples/piano/cass_piano.h"
// #include "samples/piano/e_piano.h"
// #include "samples/piano/Glass_Piano_C.h"
// #include "samples/piano/piano_4C.h"
// #include "samples/piano/toy_piano_c.h"

// #include "samples/c_perc/bell_tree.h"
// #include "samples/c_perc/cass_bell.h"
// #include "samples/c_perc/celesta.h"
// #include "samples/c_perc/glock_hi_c.h"
// #include "samples/c_perc/Marimba_C.h"
// #include "samples/c_perc/Marimba_Rev_C.h"
// #include "samples/c_perc/milkjar.h"
// #include "samples/c_perc/round.h"
// #include "samples/c_perc/steeldrum.h"
// #include "samples/c_perc/tine.h"
// #include "samples/c_perc/valavophone.h"
// #include "samples/c_perc/valveophone_hand.h"

// #include "samples/Organ/buzzyOrgan.h"
// #include "samples/Organ/mellowtron.h"
// #include "samples/Organ/microwave_organ.h"
// #include "samples/Organ/organ_simple.h"
// #include "samples/Organ/organ.h"
// #include "samples/Organ/rainbows.h"
// #include "samples/Organ/rezOrgan.h"
// #include "samples/Organ/shorgan.h"

#include "samples/Synth/ambitone.h"
#include "samples/Synth/anne_sine.h"
// #include "samples/Synth/belton.h"
// #include "samples/Synth/bubblic.h"
// #include "samples/Synth/cassambi.h"
// #include "samples/Synth/digitalridoo.h"
// #include "samples/Synth/echoesyn.h"
// #include "samples/Synth/FaeryTale.h"
// #include "samples/Synth/flutterbug.h"
// #include "samples/Synth/fyof2_chime.h"
// // #include "samples/Synth/fyof2_synth.h"
// #include "samples/Synth/granularglockenfi.h"
// // #include "samples/Synth/JahrMarkt2.h"
// #include "samples/Synth/kiksynth.h"
// // #include "samples/Synth/KorgBow.h"
// #include "samples/Synth/lobassy.h"
// #include "samples/Synth/outer.h"
// #include "samples/Synth/padlet.h"
// #include "samples/Synth/pcmchip.h"
// #include "samples/Synth/shoresine.h"
// #include "samples/Synth/soft_sk.h"
// // #include "samples/Synth/squaccordion.h"
// #include "samples/Synth/strawman.h"
// // #include "samples/Synth/vocal_samp_C.h"
// #include "samples/Synth/whisper_pad.h"


#define NUM_SAMPLES 38

const int16_t *samples_list[NUM_SAMPLES] = { 
// cass_piano_DATA, // 0
// e_piano_DATA, // 1
// Glass_Piano_C_DATA, // 2
// piano_4C_DATA, // 3
// toy_piano_c_DATA, // 4

// bell_tree_DATA, // 5
// cass_bell_DATA, // 6
// celesta_DATA, // 7
// glock_hi_c_DATA, // 8
// Marimba_C_DATA, // 9
// Marimba_Rev_C_DATA, // 10
// milkjar_DATA, // 11
// round_DATA, // 12
// steeldrum_DATA, // 13
// tine_DATA, // 14
// valavophone_DATA, // 15
// valveophone_hand_DATA, // 16

// buzzyOrgan_DATA, // 17
// mellowtron_DATA, // 18
// microwave_organ_DATA, // 19
// organ_simple_DATA, // 20
// organ_DATA, // 21
// rainbows_DATA, // 22
// rezOrgan_DATA, // 23
// shorgan_DATA, // 24

ambitone_DATA, // 25
anne_sine_DATA, // 26
// belton_DATA, // 27
// bubblic_DATA, // 28
// cassambi_DATA, // 29 
// digitalridoo_DATA, // 30
// echoesyn_DATA, // 31
// FaeryTale_DATA, // 32
// flutterbug_DATA, // 33
// fyof2_chime_DATA, // 34
// // fyof2_synth_DATA, // 35
// granularglockenfi_DATA, // 36
// // JahrMarkt2_DATA, // 37
// kiksynth_DATA, // 38
// // KorgBow_DATA, // 39
// lobassy_DATA, // 40
// outer_DATA, // 41
// padlet_DATA, // 42
// pcmchip_DATA, // 43
// shoresine_DATA, // 44
// soft_sk_DATA, // 45
// // squaccordion_DATA, // 46
// strawman_DATA, // 47
// // vocal_samp_C_DATA, // 48
// whisper_pad_DATA, // 49

};


const uint64_t samples_lengths[NUM_SAMPLES] = {
// cass_piano_NUM_CELLS, // 0
// e_piano_NUM_CELLS, // 1
// Glass_Piano_C_NUM_CELLS, // 2
// piano_4C_NUM_CELLS, // 3
// toy_piano_c_NUM_CELLS, // 4

// bell_tree_NUM_CELLS, // 5
// cass_bell_NUM_CELLS, // 6
// celesta_NUM_CELLS, // 7
// glock_hi_c_NUM_CELLS, // 8
// Marimba_C_NUM_CELLS, // 9
// Marimba_Rev_C_NUM_CELLS, // 10
// milkjar_NUM_CELLS, // 11
// round_NUM_CELLS, // 12
// steeldrum_NUM_CELLS, // 13
// tine_NUM_CELLS, // 14
// valavophone_NUM_CELLS, // 15
// valveophone_hand_NUM_CELLS, // 16

// buzzyOrgan_NUM_CELLS, // 17
// mellowtron_NUM_CELLS, // 18
// microwave_organ_NUM_CELLS, // 19
// organ_simple_NUM_CELLS, // 20
// organ_NUM_CELLS, // 21
// rainbows_NUM_CELLS, // 22
// rezOrgan_NUM_CELLS, // 23
// shorgan_NUM_CELLS, // 24

ambitone_NUM_CELLS, // 25
anne_sine_NUM_CELLS, // 26  
// belton_NUM_CELLS, // 27
// bubblic_NUM_CELLS, // 28
// cassambi_NUM_CELLS, // 29 
// digitalridoo_NUM_CELLS, // 30
// echoesyn_NUM_CELLS, // 31
// FaeryTale_NUM_CELLS, // 32
// flutterbug_NUM_CELLS, // 33
// fyof2_chime_NUM_CELLS, // 34
// // fyof2_synth_NUM_CELLS, // 35
// granularglockenfi_NUM_CELLS, // 36
// // JahrMarkt2_NUM_CELLS, // 37
// kiksynth_NUM_CELLS, // 38
// // KorgBow_NUM_CELLS, // 39
// lobassy_NUM_CELLS, // 40
// outer_NUM_CELLS, // 41
// padlet_NUM_CELLS, // 42
// pcmchip_NUM_CELLS, // 43
// shoresine_NUM_CELLS, // 44
// soft_sk_NUM_CELLS, // 45
// // squaccordion_NUM_CELLS, // 46
// strawman_NUM_CELLS, // 47
// // vocal_samp_C_NUM_CELLS, // 48
// whisper_pad_NUM_CELLS, // 49
};