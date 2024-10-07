#ifndef MEAP_H_
#define MEAP_H_

#include "MozziConfigValues.h"
#define ESP_SAMPLE_SIZE 4 // 2 * sizeof(int16_t)

#if MEAP_LEGACY == 3
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ // PT8211 uses LSBJ format

#define MOZZI_AUDIO_BITS 16
#define MOZZI_I2S_PIN_BCK 48
#define MOZZI_I2S_PIN_WS 47
#define MOZZI_I2S_PIN_DATA 21

#ifndef MOZZI_AUDIO_CHANNELS
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO
#endif
#include <Mozzi.h>
#include "legacy/Meap_classes_legacy.h"
#else

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

// #define ESP_SAMPLE_SIZE (2 * sizeof(int16_t))
// #define ESP_SAMPLE_SIZE 4 // 2 * sizeof(int16_t)

#include <Mozzi.h>

#include "driver/i2s_std.h"
// // I2S communication with SGTL5000 Audio Codec

#include "Meap_classes.h"
#endif

#include <MIDI.h> // Arduino Midi library https://github.com/FortySevenEffects/arduino_midi_library

// Attempts to include all Mozzi modules, though I may have missed them. Surely Mozzi has an option to include all modules in one line???
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

#include <meap_modules/midi_notes.h>

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
#include <meap_mozzi/mEad.h>

// meap instruments
#include <meap_modules/mSampler.h>
#include <meap_modules/mSamplerVoice.h>
#include <meap_modules/mDrumRack.h>
#include <meap_modules/mRompler.h>
#include <meap_modules/mSamplerPoly.h>

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

#include <meap_modules/mStringSynthVoice.h>
#include <meap_modules/mStringSynthPoly.h>

#include <meap_modules/mGrainGenerator.h>
#include <meap_modules/mGrainCloud.h>
#include <meap_modules/mSampleGrainGenerator.h>
#include <meap_modules/mSampleGrainCloud.h>

#include <meap_modules/mGuitarPoly.h>

#include <meap_modules/mFM2Voice.h>

#include <meap_modules/mFM2Poly.h>

// meap effects

#ifndef MEAP_LEGACY
#include <meap_modules/mChopper.h>

#include <dependencies/LinkedList/LinkedList.h>

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
#endif

#endif // MEAP_H_