#ifndef MEAP_H_
#define MEAP_H_

#include "MozziConfigValues.h"
#define ESP_SAMPLE_SIZE 4 // 2 * sizeof(int16_t)

#ifndef MOZZI_AUDIO_RATE
#define MOZZI_AUDIO_RATE 32768
#endif

#ifndef MOZZI_CONTROL_RATE
#define MOZZI_CONTROL_RATE 64
#endif

#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_CUSTOM
#define MOZZI_AUDIO_BITS 16
#define MOZZI_PDM_RESOLUTION 8

#ifndef MOZZI_AUDIO_CHANNELS
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO
#endif

// #if defined(MEAPV4B)
// #include "hardware_support/meap_v4b.h"
// #elif defined(MEAPV4C)
// #include "hardware_support/meap_v4c.h"
// #else
// #error No valid hardware version defined
// #endif

// #define ESP_SAMPLE_SIZE (2 * sizeof(int16_t))
// #define ESP_SAMPLE_SIZE 4 // 2 * sizeof(int16_t)
#include "Arduino.h"
#include <Mozzi.h>

#include "driver/i2s_std.h"
// // I2S communication with SGTL5000 Audio Codec

#include "Meap_classes.h"

#include <MIDI.h> // Arduino Midi library https://github.com/FortySevenEffects/arduino_midi_library

// include all mozzi modules
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
#include <DCfilter.h>
#include <Ead.h>
#include <EventDelay.h>
#include <IntMap.h>
#include <Line.h>
#include <Metronome.h>
#include <ResonantFilter.h>
#include <Oscil.h>
#include <PDResonant.h>
#include <Phasor.h>
#include <Portamento.h>
#include <RCpoll.h>
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

#include <meap_modules/tools/midi_notes.h>

// #include <meap_stk/meap_delay.h>
// #include <meap_stk/meap_delay_linear.h>
// #include <meap_stk/meap_delay_allpass.h>
// #include <meap_stk/meap_fir.h>
// #include <meap_stk/meap_twang.h>
// #include <meap_stk/meap_one_pole.h>
// #include <meap_stk/meap_one_zero.h>
// #include <meap_stk/meap_plucked.h>
// #include <meap_stk/meap_guitar.h>
// #include <meap_stk/meap_effect.h>
// #include <meap_stk/meap_prc_rev.h>
// #include <meap_stk/meap_biquad.h>
// #include <meap_stk/meap_bowtable.h>
// #include <meap_stk/meap_adsr.h>
// #include <meap_stk/meap_banded_waveguide.h>
// #include <meap_stk/meap_iir.h>
// #include <meap_stk/meap_sinewave.h>
// #include <meap_stk/meap_chorus.h>
// #include <meap_stk/meap_polezero.h>
// #include <meap_stk/meap_brass.h>
// #include <meap_stk/meap_filter.h>
// #include <meap_stk/meap_bowed.h>
// #include <meap_stk/meap_twozero.h>
// #include <meap_stk/meap_lentpitshift.h>
// #include <meap_stk/meap_recorder.h>
// #include <meap_stk/meap_freeverb.h>
// #include <meap_stk/meap_control_sine.h>

// ---- dependencies ----
#include <dependencies/LinkedList/LinkedList.h>

// ---- meap extensions to mozzi objects ----
#include <meap_mozzi/mSample.h>
#include <meap_mozzi/mOscil.h>
#include <meap_mozzi/mEad.h>
#include <meap_mozzi/mPortamento.h>
#include <meap_mozzi/mStateVariable.h>

// clutter
#include <meap_modules/clutter/mSampler.h>
#include <meap_modules/clutter/mSamplerVoice.h>
#include <meap_modules/clutter/mDrumRack.h>

#include <meap_modules/clutter/mSamplerPoly.h>

#include <meap_modules/clutter/mBasicFM.h>
#include <meap_modules/clutter/mBasicFMPoly.h>

#include <meap_modules/clutter/mGrainGenerator.h>
#include <meap_modules/clutter/mGrainCloud.h>
#include <meap_modules/clutter/mSampleGrainGenerator.h>
#include <meap_modules/clutter/mSampleGrainCloud.h>

// #include <meap_modules/clutter/mGuitarPoly.h>

#include <meap_modules/clutter/mFM2Voice.h>
#include <meap_modules/clutter/mFM2Poly.h>

#include <meap_modules/clutter/mWavetableSynth.h>
#include <meap_modules/clutter/mOscBank.h>

#include <meap_modules/clutter/mRealSamplerPoly.h>

#include <meap_modules/clutter/mChopper.h>

// -------------------- MEAP DSP --------------------

// ---- TOOLS ----
#include <meap_modules/tools/fast_pow2.h>
#include <meap_modules/tools/mRandomDistribution.h>
#include <meap_modules/tools/mEventDelayMicros.h>
#include <meap_modules/tools/mSD.h>
#include <meap_modules/tools/mPlaySD16.h>
#include <meap_modules/tools/mPlaySD8.h>

// ---- DSP ----
#include <meap_modules/dsp/mNaturalComb.h>
#include <meap_modules/dsp/mNaturalCombLinear.h>
#include <meap_modules/dsp/mSchroederAllpass.h>
#include <meap_modules/dsp/mSchroederAllpassLinear.h>
#include <meap_modules/dsp/mOnePoleLPF.h>
#include <meap_modules/dsp/mDelayLine.h>
#include <meap_modules/dsp/mRingz.h>
#include <meap_modules/dsp/mResonz.h>

// ---- EFFECTS ----
#include <meap_modules/effects/mSchroederReverb.h>
#include <meap_modules/effects/mDattorroPlate.h>
#include <meap_modules/effects/mDigitalDelay.h>
#include <meap_modules/effects/mBitcrusher.h>
#include <meap_modules/effects/mAnalogDelay.h>

// ---- GENERATORS ----
#include <meap_modules/generators/mSineLFO.h>
#include <meap_modules/generators/mOperator.h>
#include <meap_modules/generators/mDust.h>
#include <meap_modules/generators/mWavetable.h>
#include <meap_modules/generators/mNoise.h>
#include <meap_modules/generators/mEnvSection.h>
#include <meap_modules/generators/mEnvelope.h>

// ---- INSTRUMENTS ----

#include <meap_modules/instruments/mInstrument.h>

#include <meap_modules/instruments/mFM4Voice.h>
#include <meap_modules/instruments/mFM4Instrument.h>

#include <meap_modules/instruments/mSubSynthVoice.h>
#include <meap_modules/instruments/mSubSynthInstrument.h>

#include <meap_modules/instruments/mStringSynthVoice.h>
#include <meap_modules/instruments/mStringSynthInstrument.h>

#include <meap_modules/instruments/mStringVoice.h>
#include <meap_modules/instruments/mStringInstrument.h>

#include <meap_modules/instruments/mPopVoice.h>
#include <meap_modules/instruments/mPopInstrument.h>

#include <meap_modules/instruments/mMarimbaVoice.h>
#include <meap_modules/instruments/mMarimbaInstrument.h>

#include <meap_modules/instruments/mSamplerInstrument.h>

#include <meap_modules/instruments/mRompler.h>

// ---- COMPOSITION ----

#include <meap_modules/composition/mMML.h>

i2s_chan_handle_t tx_handle;
i2s_chan_handle_t rx_handle;

// MOZZI output buffer
bool _esp32_can_buffer_next = true; // initialize to true
int16_t _esp32_prev_sample[2];

int16_t meap_input_frame[2];

// #define ESP_SAMPLE_SIZE (2 * sizeof(int16_t))

inline bool esp32_tryWriteSample()
{
    size_t bytes_written;
    i2s_channel_write(tx_handle, &_esp32_prev_sample, 4, &bytes_written, 0); // 4 = 2 * sizeof(int16_t)
    return (bytes_written != 0);
}

inline void audioOutput(const AudioOutput f)
{
    _esp32_prev_sample[0] = f.l();
    _esp32_prev_sample[1] = f.r();

    _esp32_can_buffer_next = esp32_tryWriteSample();

    // audio input
    size_t bytes_read;
    i2s_channel_read(rx_handle, &meap_input_frame, 4, &bytes_read, 0);
}

inline bool canBufferAudioOutput()
{
    if (_esp32_can_buffer_next)
    {
        return true;
    }
    _esp32_can_buffer_next = esp32_tryWriteSample();
    return _esp32_can_buffer_next;
}

#endif // MEAP_H_