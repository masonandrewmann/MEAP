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


#include "samples/cass_piano.h"
#include "samples/piano_4C.h"
#include "samples/toy_piano.h"
#include "samples/marimba.h"
#include "samples/marimba_rev.h"
#include "samples/steel_drum.h"


#include "samples/organ.h"
#include "samples/rez_organ.h"
#include "samples/buzzyOrgan.h"
#include "samples/org_5th.h"

#include "samples/uke.h"
#include "samples/harp2.h"
#include "samples/tine.h"
#include "samples/lead_gtr.h"

#include "samples/upright_bass.h"
#include "samples/mono_bass.h"

#include "samples/shimmer.h"
#include "samples/fantasy.h"
#include "samples/korg_bow.h"
#include "samples/lead_synth.h"
#include "samples/buzzy.h"

#include "samples/flute.h"
#include "samples/strings.h"
#include "samples/vox.h"

#include "samples/e_piano.h"
// #include "samples/belldrone.h"

#define NUM_SAMPLES 34

const int8_t *samples_list[NUM_SAMPLES] = { 
piano_DATA, // 0 -   piano
clari_DATA, // 1 -   clarinet
tpt_DATA,   // 2 -   trumpet
glock_DATA, // 3 -   glockenspiel
gtr_DATA,   // 4 -   guitar
sine_DATA,  // 5 -   sine
harp_DATA,  // 6 -   harp
C_maj_DATA, // 7 -   Guitar chords
C_min_DATA, // 8 -   DEAD ZONE
C_dim_DATA, // 9 -   DEADZONE
C_aug_DATA, // 10 -  DEAD ZONE
toy_piano_DATA, // 11 - Toy Piano
marimba_DATA, // 12 - Marimba
marimba_rev_DATA, // 13 - Marimba with reverb
steel_drum_DATA, // 14 - Steel Drum
organ_DATA, // 15 - Organ
rez_organ_DATA, // 16 - Resonant Organ
buzzyOrgan_DATA, // 17 - Buzzy Organ
org_5th_DATA, // 18 - 5ths Organ
uke_DATA, // 19 - Ukulele
harp2_DATA, // 20 - High Harp
tine_DATA, // 21 - Plucked Melodica Tine
lead_gtr_DATA, // 22 - Lead Guitar
upright_bass_DATA, // 23 - Upright Bass
mono_bass_DATA, // 24 - Mono Synth Bass
shimmer_DATA, // 25 - Shimmer 
fantasy_DATA, // 26 - Fantasy
korg_bow_DATA, // 27 - Korg Bow
lead_synth_DATA, // 28 - Lead Synth
buzzy_DATA, // 29 - Buzzy Synth
flute_DATA, // 30 - Flute
strings_DATA, // 31 - Strings
vox_DATA, // 32 - Vox
e_piano_DATA, // 33 - E Piano
};


int samples_lengths[NUM_SAMPLES] = {
piano_NUM_CELLS, // 0 -   piano
clari_NUM_CELLS, // 1 -   clarinet
tpt_NUM_CELLS,   // 2 -   trumpet
glock_NUM_CELLS, // 3 -   glockenspiel
gtr_NUM_CELLS,   // 4 -   guitar
sine_NUM_CELLS,  // 5 -   sine
harp_NUM_CELLS,  // 6 -   harp
C_maj_NUM_CELLS, // 7 -   Guitar chords
C_min_NUM_CELLS, // 8 -   DEAD ZONE
C_dim_NUM_CELLS, // 9 -   DEADZONE
C_aug_NUM_CELLS, // 10 -  DEAD ZONE
toy_piano_NUM_CELLS, // 11 - Toy Piano
marimba_NUM_CELLS, // 12 - Marimba
marimba_rev_NUM_CELLS, // 13 - Marimba with reverb
steel_drum_NUM_CELLS, // 14 - Steel Drum
organ_NUM_CELLS, // 15 - Organ
rez_organ_NUM_CELLS, // 16 - Resonant Organ
buzzyOrgan_NUM_CELLS, // 17 - Buzzy Organ
org_5th_NUM_CELLS, // 18 - 5ths Organ
uke_NUM_CELLS, // 19 - Ukulele
harp2_NUM_CELLS, // 20 - High Harp
tine_NUM_CELLS, // 21 - Plucked Melodica Tine
lead_gtr_NUM_CELLS, // 22 - Lead Guitar
upright_bass_NUM_CELLS, // 23 - Upright Bass
mono_bass_NUM_CELLS, // 24 - Mono Synth Bass
shimmer_NUM_CELLS, // 25 - Shimmer 
fantasy_NUM_CELLS, // 26 - Fantasy
korg_bow_NUM_CELLS, // 27 - Korg Bow
lead_synth_NUM_CELLS, // 28 - Lead Synth
buzzy_NUM_CELLS, // 29 - Buzzy Synth
flute_NUM_CELLS, // 30 - Flute
strings_NUM_CELLS, // 31 - Strings
vox_NUM_CELLS, // 32 - Vox
e_piano_NUM_CELLS, // 33 - E Piano
};