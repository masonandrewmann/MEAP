#ifndef MEAP_H_
#define MEAP_H_

#include "Meap_classes.h"
#include <MIDI.h> // Arduino Midi library https://github.com/FortySevenEffects/arduino_midi_library

#include "MozziConfigValues.h"
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ // PT8211 uses LSBJ format

#define MOZZI_AUDIO_BITS 16
#define MOZZI_I2S_PIN_BCK 48
#define MOZZI_I2S_PIN_WS 47
#define MOZZI_I2S_PIN_DATA 21

#ifndef MOZZI_AUDIO_CHANNELS
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO
#endif

// might need
#define MOZZI_ANALOG_READ_RESOLUTION 10

// Attempts to include all Mozzi modules, though I may have missed them. Surely Mozzi has an option to include all modules in one line???
#include <Mozzi.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <mozzi_rand.h>
#include <ADSR.h>
#include <AudioDelay.h>
#include <AudioDelayFeedback.h>
#include <AutoMap.h>
#include <AutoRange.h>
#include <CapPoll.h>
// #include <CircularBuffer.h> // not included because it causes a redeclaration error, should have an #ifdef clause like rest of modules
#include <ControlDelay.h>
#include <DCFilter.h>
#include <Ead.h>
#include <EventDelay.h>
#include <IntMap.h>
#include <Line.h>
#include <Metronome.h>
#include <ResonantFilter.h>
#include <Oscil.h>
#include <PdResonant.h>
#include <Phasor.h>
#include <Portamento.h>
#include <RCPoll.h>
#include <ReverbTank.h>
#include <RollingAverage.h>
#include <Sample.h>
#include <SampleHuffman.h>
#include <Smooth.h>
#include <StateVariable.h>
#include <WaveFolder.h>
#include <WavePacket.h>
#include <WavePacketSample.h>
#include <WaveShaper.h>

#include <meap_stk/meap_delay.h>
#include <meap_stk/meap_delay_linear.h>
#include <meap_stk/meap_delay_allpass.h>
#include <meap_stk/meap_fir.h>
#include <meap_stk/meap_twang.h>
#include <meap_stk/meap_one_pole.h>
#include <meap_stk/meap_one_zero.h>
// #include <meap_stk/meap_plucked.h>
#include <meap_stk/meap_guitar.h>
#include <meap_stk/meap_effect.h>
#include <meap_stk/meap_prc_rev.h>
#include <meap_stk/meap_biquad.h>
#include <meap_stk/meap_bowtable.h>
#include <meap_stk/meap_adsr.h>
#include <meap_stk/meap_banded_waveguide.h>
#include <meap_stk/meap_iir.h>
#include <meap_stk/meap_sinewave.h>
#include <meap_stk/meap_chorus.h>
#include <meap_stk/meap_polezero.h>
#include <meap_stk/meap_brass.h>
#include <meap_stk/meap_filter.h>
#include <meap_stk/meap_bowed.h>
#include <meap_stk/meap_twozero.h>
// #include <meap_stk/meap_lentpitshift.h>
// #include <meap_stk/meap_recorder.h>
#include <meap_stk/meap_freeverb.h>
#include <meap_stk/meap_control_sine.h>

// meap extensions to mozzi objects
#include <meap_mozzi/mSample.h>
#include <meap_mozzi/mOscil.h>

// meap instruments
#include <meap_modules/mSampler.h>
#include <meap_modules/mSamplerVoice.h>
#include <meap_modules/mDrumRack.h>
#include <meap_modules/mRompler.h>

#include <meap_modules/mBasicFM.h>
#include <meap_modules/mBasicFMPoly.h>

#include <meap_modules/mWavetable.h>
#include <meap_modules/mWavetableSynth.h>

#include <meap_modules/mOscBank.h>

#include <meap_modules/mOperator.h>
#include <meap_modules/mFM4Voice.h>
#include <meap_modules/mFM4Poly.h>

#include <meap_modules/mSubSynthVoice.h>
#include <meap_modules/mSubSynthPoly.h>

// meap effects
#include <meap_modules/mChopper.h>

#include <dependencies/LinkedList/LinkedList.h>

#endif // MEAP_H_