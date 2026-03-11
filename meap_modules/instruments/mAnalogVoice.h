#ifndef MANALOGVOICE_H_
#define MANALOGVOICE_H_

#include "../generators/mEnvelope.h"
#include "../generators/mSyncOsc.h"
#include "../../meap_mozzi/mOscil.h"

#include "../../tables/sin8192_int16.h"
#include "../../tables/saw8192_int16.h"
#include "../../tables/tri8192_int16.h"
#include "../../tables/sq8192_int16.h"

enum AnalogWavetable
{
    kANALOG_TRI,
    kANALOG_SIN,
    kANALOG_SAW,
    kANALOG_SQU
};

enum AnalogFilterTypes
{
    kANALOG_HIGH,
    kANALOG_LOW,
    kANALOG_BAND
};

class mAnalogVoice
{
public:
    mAnalogVoice()
    {
        // ==== OSC 1 ====
        osc1.setTable(saw8192_int16_DATA);
        osc1.setFreq(mtof(48));
        osc1_pitch_env.init(0, 0.5, 1.0, mCURVE_EXPONENTIAL);

        // ==== FILTER 1 ====
        filter1.setCutoffFreqAndResonance(65530, 0);
        float env_levels_init[4] = {0.0, 1.0, 0.8, 0.0};
        int32_t env_times_init[3] = {10, 100, 1000};
        MEAP_ENV_CURVES env_curves_init[3] = {mCURVE_LINEAR, mCURVE_LINEAR, mCURVE_LINEAR};
        // MEAP_ENV_CURVES env_curves_init[3] = {mCURVE_EXPONENTIAL, mCURVE_EXPONENTIAL, mCURVE_EXPONENTIAL};
        filter1_env = new mEnvelope<CONTROL_RATE>(env_levels_init, env_times_init, env_curves_init, 3, 1);
        filter1_env->setRTZ(false);

        // ==== AMP 1 ====
        amp1_env = new mEnvelope<AUDIO_RATE>(env_levels_init, env_times_init, env_curves_init, 3, 1);
        amp1_env->setRTZ(false);

        // ==== LFO 1 ====
        lfo1.setTable(sin8192_int16_DATA);
        lfo1.setFreq(0.5);
        float lfo1_env_levels[3] = {0.0, 0.0, 1.0};
        int32_t lfo1_env_times[2] = {0, 0};
        MEAP_ENV_CURVES lfo1_curves[2] = {mCURVE_LINEAR, mCURVE_LINEAR};
        lfo1_fade = new mEnvelope<CONTROL_RATE>(lfo1_env_levels, lfo1_env_times, lfo1_curves, 2, 2);
        lfo1_fade->setRTZ(true);

        // ==== OSC 2 ====
        osc2.setTable(saw8192_int16_DATA);
        osc2.setFreq(mtof(48));
        osc2_pitch_env.init(0, 0.5, 1.0, mCURVE_EXPONENTIAL);

        // ==== FILTER 2 ====
        filter2.setCutoffFreqAndResonance(65530, 0);
        filter2_env = new mEnvelope<CONTROL_RATE>(env_levels_init, env_times_init, env_curves_init, 3, 1);
        filter2_env->setRTZ(false);

        // ==== AMP 1 ====
        amp2_env = new mEnvelope<AUDIO_RATE>(env_levels_init, env_times_init, env_curves_init, 3, 1);
        amp2_env->setRTZ(false);

        // ==== LFO 1 ====
        lfo2.setTable(sin8192_int16_DATA);
        lfo2.setFreq(0.5);
        lfo2_fade = new mEnvelope<CONTROL_RATE>(lfo1_env_levels, lfo1_env_times, lfo1_curves, 2, 2);
        lfo2_fade->setRTZ(true);
    };

    void noteOn(uint16_t note, uint16_t vel)
    {
        if (lfo1_retrig)
        {
            lfo1.setPhase(lfo1_phase);
        }
        last_note = note;
        last_vel = vel;
        amp1_env->noteOn();
        filter1_env->noteOn();
        lfo1_fade->noteOn();
        osc1_hz = mtof(note + 12 * osc1_octave + osc1_transpose + osc1_detune);
        osc1.setFreq(osc1_hz);
        osc1_pitch_env.trigger();

        if (lfo2_retrig)
        {
            lfo2.setPhase(lfo2_phase);
        }
        last_note = note;
        last_vel = vel;
        amp2_env->noteOn();
        filter2_env->noteOn();
        lfo2_fade->noteOn();
        osc2_hz = mtof(note + 12 * osc2_octave + osc2_transpose + osc2_detune);
        osc2.setFreq(osc2_hz);
        osc2_pitch_env.trigger();
    }

    void noteOff()
    {
        amp1_env->noteOff();
        filter1_env->noteOff();
        amp2_env->noteOff();
        filter2_env->noteOff();
    }

    // crossfades how much to send oscillator to each filter
    // at 0, oscillator will be only sent to filter 0, at 1 oscillator will only be sent to filter 1
    void setOscFilterSend(float filtersend, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            osc1_filtersend = filtersend;
            osc2_filtersend = filtersend;
            break;
        case 1: // voice 1 filter
            osc1_filtersend = filtersend;
            break;
        case 2: // voice 2 filter
            osc2_filtersend = filtersend;
            break;
        }
    }

    // sets oscillator's wavetable to one of the following
    //      kANALOG_TRI,
    //      kANALOG_SIN,
    //      kANALOG_SAW,
    //      kANALOG_SQU
    void setOscWavetable(AnalogWavetable wavetable, int voice_num = 0)
    {
        switch (wavetable)
        {
        case kANALOG_SIN:
            switch (voice_num)
            {
            case 0:
                osc1.setTable(sin8192_int16_DATA);
                osc2.setTable(sin8192_int16_DATA);
                break;
            case 1:
                osc1.setTable(sin8192_int16_DATA);
                break;
            case 2:
                osc2.setTable(sin8192_int16_DATA);
                break;
            }
            break;
        case kANALOG_TRI:
            switch (voice_num)
            {
            case 0:
                osc1.setTable(tri8192_int16_DATA);
                osc2.setTable(tri8192_int16_DATA);
                break;
            case 1:
                osc1.setTable(tri8192_int16_DATA);
                break;
            case 2:
                osc2.setTable(tri8192_int16_DATA);
                break;
            }
            break;
        case kANALOG_SAW:
            switch (voice_num)
            {
            case 0:
                osc1.setTable(saw8192_int16_DATA);
                osc2.setTable(saw8192_int16_DATA);
                break;
            case 1:
                osc1.setTable(saw8192_int16_DATA);
                break;
            case 2:
                osc2.setTable(saw8192_int16_DATA);
                break;
            }
            break;
        case kANALOG_SQU:
            switch (voice_num)
            {
            case 0:
                osc1.setTable(sq8192_int16_DATA);
                osc2.setTable(sq8192_int16_DATA);
                break;
            case 1:
                osc1.setTable(sq8192_int16_DATA);
                break;
            case 2:
                osc2.setTable(sq8192_int16_DATA);
                break;
            }
            break;
        default:
            switch (voice_num)
            {
            case 0:
                osc1.setTable(sin8192_int16_DATA);
                osc2.setTable(sin8192_int16_DATA);
                break;
            case 1:
                osc1.setTable(sin8192_int16_DATA);
                break;
            case 2:
                osc2.setTable(sin8192_int16_DATA);
                break;
            }
            break;
        }
    }

    // Set oscillator's octave
    // octave: octave of oscillator -3 to 3
    // which voice to modify: 0 for both
    void setOscOctave(int octave, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            osc1_octave = octave;
            osc2_octave = octave;
            osc1_pitch_flag = true;
            osc2_pitch_flag = true;
            break;
        case 1: // voice 1 filter
            osc1_octave = octave;
            osc1_pitch_flag = true;
            break;
        case 2: // voice 2 filter
            osc2_octave = octave;
            osc2_pitch_flag = true;
            break;
        }
    }

    // Set oscillator's octave
    // octave: octave of oscillator -12 to 12
    // which voice to modify: 0 for both
    void setOscTranspose(int transpose, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            osc1_transpose = transpose;
            osc2_transpose = transpose;
            osc1_pitch_flag = true;
            osc2_pitch_flag = true;
            break;
        case 1: // voice 1 filter
            osc1_transpose = transpose;
            osc1_pitch_flag = true;
            break;
        case 2: // voice 2 filter
            osc2_transpose = transpose;
            osc2_pitch_flag = true;
            break;
        }
    }

    // Set oscillator's detune
    // detune: detune of oscillator -3 to 3 (in semitones)
    // which voice to modify: 0 for both
    void setOscDetune(float detune, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            osc1_detune = detune;
            osc2_detune = detune;
            osc1_pitch_flag = true;
            osc2_pitch_flag = true;
            break;
        case 1: // voice 1 filter
            osc1_detune = detune;
            osc1_pitch_flag = true;
            break;
        case 2: // voice 2 filter
            osc2_detune = detune;
            osc2_pitch_flag = true;
            break;
        }
    }

    // sets sync percent of oscillator betwee 0 and 1
    void setOscSync(float sync_percent, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            osc1.setSync(sync_percent);
            osc2.setSync(sync_percent);
            break;
        case 1: // voice 1 filter
            osc1.setSync(sync_percent);
            break;
        case 2: // voice 2 filter
            osc2.setSync(sync_percent);
            break;
        }
    }

    // Set filter's parameters
    // cutoff_hz: cutoff frequency in Hz
    // resonance: resonance between 0 and 1
    // voice_num: which voice to modify: 0 for both, 1 for voice 1, 2 for voice 2
    void setFilterParameters(int32_t cutoff_hz, float resonance, int voice_num = 0)
    {
        // cutoff_hz is between 0 and 16384
        switch (voice_num)
        {
        case 0: // both filters
            filter1_cutoff = cutoff_hz << 2;
            filter1_resonance = resonance * 65535;
            filter2_cutoff = filter1_cutoff;
            filter2_resonance = filter1_resonance;
            break;
        case 1: // voice 1 filter
            filter1_cutoff = cutoff_hz << 2;
            filter1_resonance = resonance * 65535;
            break;
        case 2: // voice 2 filter
            filter2_cutoff = filter1_cutoff;
            filter2_resonance = filter1_resonance;
            break;
        }
    }

    // sets filter type to one of the following
    //    kANALOG_HIGH,
    //    kANALOG_LOW,
    //    kANALOG_BAND
    void setFilterType(AnalogFilterTypes filter_type, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            filter1_type = filter_type;
            filter2_type = filter_type;
            break;
        case 1: // voice 1 filter
            filter1_type = filter_type;
            break;
        case 2: // voice 2 filter
            filter2_type = filter_type;
            break;
        }
    }

    // sets how much filter envelope will effect cutoff and resonance (0 to 1)
    void setFilterEnvAmt(float cutoff_amt, float res_amt, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            filter1_env_cutoff_amt = cutoff_amt;
            filter1_env_res_amt = res_amt;
            filter2_env_cutoff_amt = cutoff_amt;
            filter2_env_res_amt = res_amt;
            break;
        case 1: // voice 1 filter
            filter1_env_cutoff_amt = cutoff_amt;
            filter1_env_res_amt = res_amt;
            break;
        case 2: // voice 2 filter
            filter2_env_cutoff_amt = cutoff_amt;
            filter2_env_res_amt = res_amt;
            break;
        }
    }

    // sets LFO frequency in Hz
    void setLfoHz(float hz, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1.setFreq(hz);
            lfo2.setFreq(hz);
            break;
        case 1: // voice 1 filter
            lfo1.setFreq(hz);
            break;
        case 2: // voice 2 filter
            lfo2.setFreq(hz);
            break;
        }
    }

    // sets LFO period as a number of sixteenth notes at a bpm
    void setLfoSixteenths(float num_sixteenths, float bpm, int voice_num = 0)
    {
        float hz = 1 / (num_sixteenths * (15.f / bpm) * ((float)AUDIO_RATE));
        switch (voice_num)
        {
        case 0: // both filters
            lfo1.setFreq(hz);
            lfo2.setFreq(hz);
            break;
        case 1: // voice 1 filter
            lfo1.setFreq(hz);
            break;
        case 2: // voice 2 filter
            lfo2.setFreq(hz);
            break;
        }
    }

    // sets starting phase of LFO between zero and one
    void setLfoPhase(float phase, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1.setPhase(8192 * phase);
            lfo2.setPhase(8192 * phase);
            break;
        case 1: // voice 1 filter
            lfo1.setPhase(8192 * phase);
            break;
        case 2: // voice 2 filter
            lfo2.setPhase(8192 * phase);
            break;
        }
    }

    // how long to wait after starting a note for LFO to begin fading in
    void setLfoDelay(int delay_ms, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_fade->_env_sections[0]->setLength(delay_ms);
            lfo2_fade->_env_sections[0]->setLength(delay_ms);
            break;
        case 1: // voice 1 filter
            lfo1_fade->_env_sections[0]->setLength(delay_ms);
            break;
        case 2: // voice 2 filter
            lfo2_fade->_env_sections[0]->setLength(delay_ms);
            break;
        }
    }

    // how long does it take LFO to complete its fade-in
    void setLfoFade(int delay_ms, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_fade->_env_sections[1]->setLength(delay_ms);
            lfo2_fade->_env_sections[1]->setLength(delay_ms);
            break;
        case 1: // voice 1 filter
            lfo1_fade->_env_sections[1]->setLength(delay_ms);
            break;
        case 2: // voice 2 filter
            lfo2_fade->_env_sections[1]->setLength(delay_ms);
            break;
        }
    }

    // if true, LFO will restart to same position every time a new note is triggered
    void setLfoRetrig(bool retrig, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_retrig = retrig;
            lfo2_retrig = retrig;
            break;
        case 1: // voice 1 filter
            lfo1_retrig = retrig;
            break;
        case 2: // voice 2 filter
            lfo2_retrig = retrig;
            break;
        }
    }

    // sets amount that LFO influences oscillator (0 to 1)
    void setLfoOscAmt(float amt, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_osc_amt = amt;
            lfo2_osc_amt = amt;
            break;
        case 1: // voice 1 filter
            lfo1_osc_amt = amt;
            break;
        case 2: // voice 2 filter
            lfo2_osc_amt = amt;
            break;
        }
    }

    // sets amount that LFO influences filter cutoff (0 to 1)
    void setLfoCutoffAmt(float amt, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_filter_cutoff_amt = amt;
            lfo2_filter_cutoff_amt = amt;
            break;
        case 1: // voice 1 filter
            lfo1_filter_cutoff_amt = amt;
            break;
        case 2: // voice 2 filter
            lfo2_filter_cutoff_amt = amt;
            break;
        }
    }

    // sets amount that LFO influences resonance (0 to 1)
    void setLfoResAmt(float amt, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_filter_res_amt = amt;
            lfo2_filter_res_amt = amt;
            break;
        case 1: // voice 1 filter
            lfo1_filter_res_amt = amt;
            break;
        case 2: // voice 2 filter
            lfo2_filter_res_amt = amt;
            break;
        }
    }

    // sets amount that LFO influences amplitude (0 to 1)
    void setLfoLevelAmt(float amt, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            lfo1_level_amt = amt;
            lfo2_level_amt = amt;
            break;
        case 1: // voice 1 filter
            lfo1_level_amt = amt;
            break;
        case 2: // voice 2 filter
            lfo2_level_amt = amt;
            break;
        }
    }

    // sets parameters of amplitude envelope: ADR in ms, sustain between 0 and 1
    void setADSR(int attack, int decay, float sustain, int release, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            amp1_env->_env_sections[0]->setLength(attack);
            amp1_env->_env_sections[1]->setLength(decay);
            amp1_env->_env_sections[1]->setEndLevel(sustain);
            amp1_env->_env_sections[2]->setLength(release);
            amp2_env->_env_sections[0]->setLength(attack);
            amp2_env->_env_sections[1]->setLength(decay);
            amp2_env->_env_sections[1]->setEndLevel(sustain);
            amp2_env->_env_sections[2]->setLength(release);
            break;
        case 1: // voice 1 filter
            amp1_env->_env_sections[0]->setLength(attack);
            amp1_env->_env_sections[1]->setLength(decay);
            amp1_env->_env_sections[1]->setEndLevel(sustain);
            amp1_env->_env_sections[2]->setLength(release);
            break;
        case 2: // voice 2 filter
            amp2_env->_env_sections[0]->setLength(attack);
            amp2_env->_env_sections[1]->setLength(decay);
            amp2_env->_env_sections[1]->setEndLevel(sustain);
            amp2_env->_env_sections[2]->setLength(release);
            break;
        }
    }

    // sets parameters of filter envelope: ADR in ms, sustain between 0 and 1
    void setFilterADSR(int attack, int decay, float sustain, int release, int voice_num = 0)
    {
        switch (voice_num)
        {
        case 0: // both filters
            filter1_env->_env_sections[0]->setLength(attack);
            filter1_env->_env_sections[1]->setLength(decay);
            filter1_env->_env_sections[1]->setEndLevel(sustain);
            filter1_env->_env_sections[2]->setLength(release);
            filter2_env->_env_sections[0]->setLength(attack);
            filter2_env->_env_sections[1]->setLength(decay);
            filter2_env->_env_sections[1]->setEndLevel(sustain);
            filter2_env->_env_sections[2]->setLength(release);
            break;
        case 1: // voice 1 filter
            filter1_env->_env_sections[0]->setLength(attack);
            filter1_env->_env_sections[1]->setLength(decay);
            filter1_env->_env_sections[1]->setEndLevel(sustain);
            filter1_env->_env_sections[2]->setLength(release);
            break;
        case 2: // voice 2 filter
            filter2_env->_env_sections[0]->setLength(attack);
            filter2_env->_env_sections[1]->setLength(decay);
            filter2_env->_env_sections[1]->setEndLevel(sustain);
            filter2_env->_env_sections[2]->setLength(release);
            break;
        }
    }

    // must be called in updateControl
    void update()
    {
        int lfo1_val = lfo1.next();
        float lfo1_val_norm = lfo1_val * 0.00003051850948; // scaled to -1 to 1
        lfo1_level_val = 1.0 - (lfo1_val_norm * 0.5 + 0.5) * lfo1_level_amt;

        float filter_env_val = filter1_env->next();
        int cutoff_val = filter1_cutoff + (filter_env_val * filter1_env_cutoff_amt * 32768 + (32768.f * lfo1_val_norm * lfo1_filter_cutoff_amt));
        if (cutoff_val > 65535)
        {
            cutoff_val = 65535;
        }
        else if (cutoff_val < 0)
        {
            cutoff_val = 0;
        }

        int res_val = filter1_resonance + (filter_env_val * filter1_env_res_amt * 32768 + (32768.f * lfo1_val_norm * lfo1_filter_cutoff_amt));
        if (res_val > 65535)
        {
            res_val = 65535;
        }
        else if (res_val < 0)
        {
            res_val = 0;
        }

        filter1.setCutoffFreqAndResonance(cutoff_val, res_val);
        osc1_hz = mtof(last_note + 12 * osc1_octave + osc1_transpose + osc1_detune + lfo1_val_norm * 12 * lfo1_osc_amt * lfo1_fade->next());
        osc1.setFreq(osc1_hz * osc1_pitch_env.next());

        // ==== OSC 2 ====

        int lfo2_val = lfo2.next();
        float lfo2_val_norm = lfo2_val * 0.00003051850948; // scaled to -1 to 1
        lfo2_level_val = 1.0 - (lfo2_val_norm * 0.5 + 0.5) * lfo2_level_amt;

        filter_env_val = filter2_env->next();
        cutoff_val = filter2_cutoff + (filter_env_val * filter2_env_cutoff_amt * 32768 + (32768.f * lfo2_val_norm * lfo2_filter_cutoff_amt));
        if (cutoff_val > 65535)
        {
            cutoff_val = 65535;
        }
        else if (cutoff_val < 0)
        {
            cutoff_val = 0;
        }

        res_val = filter2_resonance + (filter_env_val * filter2_env_res_amt * 32768 + (32768.f * lfo2_val_norm * lfo2_filter_cutoff_amt));
        if (res_val > 65535)
        {
            res_val = 65535;
        }
        else if (res_val < 0)
        {
            res_val = 0;
        }

        filter2.setCutoffFreqAndResonance(cutoff_val, res_val);
        osc2_hz = mtof(last_note + 12 * osc2_octave + osc2_transpose + osc2_detune + lfo2_val_norm * 12 * lfo2_osc_amt * lfo2_fade->next());
        osc2.setFreq(osc2_hz * osc2_pitch_env.next());
    }

    // to be called in updateAudio
    int32_t next()
    {
        int32_t out_sample = 0;
        int32_t osc1_sample = osc1.next();
        int32_t osc2_sample = osc2.next();

        filter1.next((1.0 - osc1_filtersend) * osc1_sample + (1.0 - osc2_filtersend) * osc2_sample);

        int32_t filter1_sample;
        switch (filter1_type)
        {
        case kANALOG_LOW:
            filter1_sample = filter1.low();
            break;
        case kANALOG_BAND:
            filter1_sample = filter1.band();
            break;
        case kANALOG_HIGH:
            filter1_sample = filter1.high();
            break;
        }

        filter2.next(osc1_filtersend * osc1_sample + osc2_filtersend * osc2_sample + filter_1_to_2_send * filter1_sample);

        int32_t filter2_sample;
        switch (filter2_type)
        {
        case kANALOG_LOW:
            filter2_sample = filter2.low();
            break;
        case kANALOG_BAND:
            filter2_sample = filter2.band();
            break;
        case kANALOG_HIGH:
            filter2_sample = filter2.high();
            break;
        }

        out_sample = lfo1_level_val * filter1_sample * amp1_level * amp1_env->next() + lfo2_level_val * filter2_sample * amp2_level * amp2_env->next();
        out_sample = (out_sample * last_vel) >> 7;
        return out_sample;
    }

    // CLASS VARIABLES

    // ==== GLOBAL ====
    // int last_note;
    // int current_note;

    int last_note = 48;
    int last_vel = 127;

    // ==== OSC 1 ====
    mSyncOsc<saw8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc1;
    float osc1_octave = 0;     // -3 to +3
    float osc1_transpose = 0;  // -12 to +12
    float osc1_detune = 0;     // -3 to 3 (in semitones)
    float osc1_filtersend = 0; // crossfade between sending to filter 1 (at 0) and filter 2 (at 1)
    mEnvSection<CONTROL_RATE> osc1_pitch_env;
    float osc1_sync_percent = 0;
    float osc1_sub_level = 0;
    bool osc1_pitch_flag = false;
    int osc1_note = 0;
    float osc1_hz = 220;

    float last_osc1_hz = 220;

    // ==== FILTER 1 ====
    MultiResonantFilter<uint16_t> filter1;
    AnalogFilterTypes filter1_type = kANALOG_LOW;
    int32_t filter1_cutoff = 65500;
    int32_t filter1_resonance = 0;
    float filter_1_to_2_send = 0;
    mEnvelope<CONTROL_RATE> *filter1_env;
    float filter1_env_cutoff_amt = 1.0;
    float filter1_env_res_amt = 0.0;

    // ==== AMP 1 ====
    float amp1_level = 1.0;
    float amp1_pan = 0.0; // -1 to 1
    mEnvelope<AUDIO_RATE> *amp1_env;

    // ==== LFO 1 ====
    mOscil<saw8192_int16_NUM_CELLS, CONTROL_RATE, int16_t> lfo1;
    int32_t lfo1_phase = 0;
    mEnvelope<CONTROL_RATE> *lfo1_fade;
    float lfo1_osc_amt = 0.0;
    float lfo1_osc_pw_amt = 0.0;
    float lfo1_filter_cutoff_amt = 0.0;
    float lfo1_filter_res_amt = 0.0;
    float lfo1_pan_amt = 0.0;
    float lfo1_level_amt = 0.0;
    bool lfo1_retrig = true;
    float lfo1_level_val = 1.0;

    // ==== OSC 2 ====
    mSyncOsc<saw8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> osc2;
    float osc2_octave = 0;     // -3 to +3
    float osc2_transpose = 0;  // -12 to +12
    float osc2_detune = 0;     // -3 to 3 (in semitones)
    float osc2_filtersend = 1; // crossfade between sending to filter 1 (at 0) and filter 2 (at 1)
    mEnvSection<CONTROL_RATE> osc2_pitch_env;
    float osc2_sync_percent = 0;
    float osc2_sub_level = 0;
    bool osc2_pitch_flag = false;
    int osc2_note = 0;
    float osc2_hz = 220;

    float last_osc2_hz = 220;

    // ==== FILTER 2 ====
    MultiResonantFilter<uint16_t> filter2;
    AnalogFilterTypes filter2_type = kANALOG_LOW;
    int32_t filter2_cutoff = 65500;
    int32_t filter2_resonance = 0;
    mEnvelope<CONTROL_RATE> *filter2_env;
    float filter2_env_cutoff_amt = 1.0;
    float filter2_env_res_amt = 0.0;

    // ==== AMP 2 ====
    float amp2_level = 1.0;
    float amp2_pan = 0.0; // -1 to 1
    mEnvelope<AUDIO_RATE> *amp2_env;

    // ==== LFO 2 ====
    mOscil<saw8192_int16_NUM_CELLS, CONTROL_RATE, int16_t> lfo2;
    int32_t lfo2_phase = 0;
    mEnvelope<CONTROL_RATE> *lfo2_fade;
    float lfo2_osc_amt = 0.0;
    float lfo2_osc_pw_amt = 0.0;
    float lfo2_filter_cutoff_amt = 0.0;
    float lfo2_filter_res_amt = 0.0;
    float lfo2_pan_amt = 0.0;
    float lfo2_level_amt = 0.0;
    bool lfo2_retrig = true;
    float lfo2_level_val = 1.0;
};

#endif // MANALOGVOICE_H_
