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



// #include "samples/cass_piano.h"
// #include "samples/piano_4C.h"
#include "samples/toy_piano.h"
#include "samples/marimba.h"
// #include "samples/marimba_rev.h"
#include "samples/steel_drum.h"


// #include "samples/organ.h"
// #include "samples/rez_organ.h"
// #include "samples/buzzyOrgan.h"
// #include "samples/org_5th.h"

#include "samples/uke.h"
#include "samples/harp2.h"
// #include "samples/tine.h"
#include "samples/lead_gtr.h"

#include "samples/upright_bass.h"
// #include "samples/mono_bass.h"

// #include "samples/shimmer.h"
#include "samples/fantasy.h"
// #include "samples/korg_bow.h"
// #include "samples/lead_synth.h"
// #include "samples/buzzy.h"

// #include "samples/flute.h"
#include "samples/strings.h"
#include "samples/vox.h"

#include "samples/e_piano.h"
// #include "samples/belldrone.h"

#include "samples/cmaj_reverb.h"
#include "samples/pistachio_sine.h"

#include "samples/rebirth1.h"
#include "samples/rebirth2.h"
#include "samples/ion_kick.h"

#include "samples/ep2.h"
#include "samples/fm.h"
#include "samples/glass.h"
// #include "samples/harp3.h"
// #include "samples/harp4.h"
// #include "samples/lemon.h"
// #include "samples/pad1.h"
#include "samples/pad2.h"
// #include "samples/arp_steady.h"
#include "samples/rhodes.h"
// #include "samples/riley.h"
// #include "samples/samu.h"
// #include "samples/space.h"
// #include "samples/tone.h"
// #include "samples/vince.h"
#include "samples/fyof2.h"
#include "samples/fyof_chime.h"

#define NUM_SAMPLES 30

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
steel_drum_DATA, // 13 - Steel Drum
uke_DATA, // 14 - Ukulele
harp2_DATA, // 15 - High Harp
fyof2_DATA, // 16 - fyof2
fyof_chime_DATA, // 17 - fyof chime
lead_gtr_DATA, // 18 - Lead Guitar
upright_bass_DATA, // 19 - Upright Bass
fantasy_DATA, // 20 - Fantasy
strings_DATA, // 21 - Strings
vox_DATA, // 22 - Vox
e_piano_DATA, // 23 - E Piano
pistachio_sine_DATA, // 24 pistachio_sine
ep2_DATA, // 25 ep2
fm_DATA, // 26 fm
glass_DATA, // 27 glass
pad2_DATA, // 28 pad2
rhodes_DATA, // 29 rhodes
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
steel_drum_NUM_CELLS, // 13 - Steel Drum
uke_NUM_CELLS, // 14 - Ukulele
harp2_NUM_CELLS, // 15 - High Harp
fyof2_NUM_CELLS, // 16 - fyof2
fyof_chime_NUM_CELLS, // 17 - fyof chime
lead_gtr_NUM_CELLS, // 18 - Lead Guitar
upright_bass_NUM_CELLS, // 19 - Upright Bass
fantasy_NUM_CELLS, // 20 - Fantasy
strings_NUM_CELLS, // 21 - Strings
vox_NUM_CELLS, // 22 - Vox
e_piano_NUM_CELLS, // 23 - E Piano
pistachio_sine_NUM_CELLS, // 24 pistachio_sine
ep2_NUM_CELLS, // 25 ep2
fm_NUM_CELLS, // 26 fm
glass_NUM_CELLS, // 27 glass
pad2_NUM_CELLS, // 28 pad2
rhodes_NUM_CELLS, // 29 rhodes
};
