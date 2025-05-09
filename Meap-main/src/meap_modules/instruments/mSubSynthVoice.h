#ifndef MSUBSYNTH_H_
#define MSUBSYNTH_H_

#include <Meap.h>

#include <Portamento.h> // from mozzi

template <uint32_t mNUM_CELLS, uint32_t mNUM_OSC = 1, class T = int8_t>
class mSubSynthVoice
{
public:
    mSubSynthVoice() {
    };

    mSubSynthVoice(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    void init(const T *TABLE_NAME)
    {
        cutoff_ = 64000;
        resonance_ = 10;

        for (uint16_t i = 0; i < mNUM_OSC; i++)
        {
            osc_[i].setTable(TABLE_NAME);
            osc_[i].setFreq(220);
            osc_gain_[i] = 255;
            osc_semitones_[i] = 0;
        }

        setNoiseGain(0);
        noise_gain_ = 0;

        amp_env_.setADLevels(255, 255);
        amp_env_.setTimes(1, 1, 4294967295, 1);
        filter_env_amount_ = 0;
        filter_sustain_level_ = 255;
        filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
        filter_env_.setTimes(1, 1, 4294967295, 1);
        filter_.setCutoffFreq(cutoff_);
        filter_.setResonance(resonance_);
        sustain_level_ = 255;
        filter_key_offset = 0;
        setPortamentoTime(0);
        note_is_on_ = false;

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 16; // 8 for env, 8 for gain
            noise_upper_ = 65535;
            noise_sub_ = 32768;
        }
        else
        {
            shift_val_ = 8;
            noise_upper_ = 255;
            noise_sub_ = 127;
        }

        phase_sync_ = true;
    }

    // 0-255
    void setFilterEnvAmount(uint32_t f_amt)
    {
        filter_env_amount_ = f_amt;
        filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
    }

    // 0-255
    void setFilterSustainLevel(uint32_t level)
    {
        filter_sustain_level_ = level;
        filter_env_.setADLevels(filter_env_amount_, (filter_env_amount_ * filter_sustain_level_) >> 8);
    }

    void setFilterEnvTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        filter_env_.setTimes(a_, d_, s_, r_);
    }

    void setFilterAttackTime(uint32_t f_a)
    {
        filter_env_.setAttackTime(f_a);
    }

    void setFilterDecayTime(uint32_t time)
    {
        filter_env_.setDecayTime(time);
    }

    void setFilterReleaseTime(uint32_t f_a)
    {
        filter_env_.setReleaseTime(f_a);
    }

    // 0-255
    void setOscGain(uint16_t osc_num, int16_t gain)
    {
        osc_gain_[osc_num] = gain;
    }

    // 0-255
    void setNoiseGain(uint16_t noise_gain)
    {
        noise_gain_ = noise_gain;
    }

    void setOscTable(uint16_t osc_num, const T *TABLE_NAME)
    {
        osc_[osc_num].setTable(TABLE_NAME);
    }

    void setOscSemitones(uint16_t osc_num, int16_t semitone_offset)
    {
        osc_semitones_[osc_num] = semitone_offset;
    }

    // detune: 0 is no detune 1 is 1 semitone
    void setOscDetune(uint16_t osc_num, float detune_amount)
    {
        osc_detune_[osc_num] = detune_amount;
    }

    void setPhaseSyncOn()
    {
        phase_sync_ = true;
    }

    void setPhaseSyncOff()
    {
        phase_sync_ = false;
    }

    // in ms
    void setAttackTime(uint32_t a_)
    {
        amp_env_.setAttackTime(a_);
    }

    // in ms
    void setDecayTime(uint32_t d_)
    {
        amp_env_.setDecayTime(d_);
    }

    // in ms
    void setSustainTime(uint32_t s_)
    {
        amp_env_.setSustainTime(s_);
    }

    // in ms
    void setReleaseTime(uint32_t r_)
    {
        amp_env_.setReleaseTime(r_);
    }

    // in ms
    void setTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        amp_env_.setTimes(a_, d_, s_, r_);
    }

    // 0-255
    void setSustainLevel(uint32_t s_l)
    {

        sustain_level_ = s_l;
    }

    // 0 - 255
    void setCutoff(uint16_t cutoff_val)
    {
        cutoff_ = cutoff_val;
    }

    // 0 - 255
    void setResonance(uint16_t resonance_val)
    {
        resonance_ = resonance_val;
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        for (uint16_t i = 0; i < mNUM_OSC; i++)
        {
            // osc_[i].setFreq(mtof((float)note + (float)osc_semitones_[i] + osc_detune_[i]));

            if (!note_is_on_)
            {
                int32_t port_time = portamento_[i].control_steps_per_portamento;
                portamento_[i].control_steps_per_portamento = 1;
                portamento_[i].startFreq(mtof((float)note + (float)osc_semitones_[i] + osc_detune_[i]));
                portamento_[i].next();
                portamento_[i].control_steps_per_portamento = port_time;
            }
            else
            {
                portamento_[i].startFreq(mtof((float)note + (float)osc_semitones_[i] + osc_detune_[i]));
            }

            if (phase_sync_)
            {
                osc_[i].setPhase(0);
            }
        }
        amp_env_.setADLevels(vel << 1, (vel * sustain_level_) >> 7);
        amp_env_.noteOn();
        filter_env_.noteOn();
        note_is_on_ = true;
        last_note_ = note;
        // if (note > 60)
        // {
        //     filter_key_offset = (note - 60) * 2;
        // }
    }

    void setPortamentoTime(uint32_t port_time_ms)
    {
        for (int i = mNUM_OSC; --i >= 0;)
        {
            portamento_[i].setTime(port_time_ms);
        }
    }

    void noteOff()
    {
        amp_env_.noteOff();
        filter_env_.noteOff();
        note_is_on_ = false;
    }

    void noteOff(int note)
    {
        if (note == last_note_)
        {
            amp_env_.noteOff();
            filter_env_.noteOff();
            note_is_on_ = false;
        }
    }

    // to be called in control loop
    void update()
    {
        for (int i = mNUM_OSC; --i >= 0;)
        {
            osc_[i].setFreq_Q16n16(portamento_[i].next());
        }
        filter_env_.update();

        // int32_t cutoff_val = cutoff_ + (filter_env_.next() << 8) + filter_key_offset; // 16bit  + offf,
        int32_t cutoff_val = cutoff_ + (filter_env_.next() << 8); // 16bit  + offf,

        if (cutoff_val > 64000)
        {
            cutoff_val = 64000;
        }
        else if (cutoff_val < 0)
        {
            cutoff_val = 0;
        }
        filter_.setCutoffFreqAndResonance(cutoff_val, resonance_);
        amp_env_.update();
    }

    // to be called in audio loop
    int32_t next()
    {
        int64_t output_sample = 0;

        for (uint16_t i = 0; i < mNUM_OSC; i++)
        {

            output_sample += (osc_[i].next() * osc_gain_[i]);
        }

        output_sample += Meap::irand(-32768, 32767) * noise_gain_; // noise sample with same bitrate as osc

        filter_.next(output_sample);
        output_sample = filter_.low();

        output_sample = (output_sample * amp_env_.next()) >> shift_val_; // down to 16 bits

        return output_sample;
    }

    // CLASS VARIABLES

    mPortamento<CONTROL_RATE> portamento_[mNUM_OSC];
    mOscil<mNUM_CELLS, AUDIO_RATE, T, mINTERP_NONE> osc_[mNUM_OSC];
    ADSR<CONTROL_RATE, AUDIO_RATE> amp_env_;
    MultiResonantFilter<uint16_t> filter_;
    ADSR<CONTROL_RATE, CONTROL_RATE> filter_env_;

    int8_t filter_key_offset;
    uint16_t noise_gain_;
    uint16_t osc_gain_[mNUM_OSC];
    int16_t osc_semitones_[mNUM_OSC];
    float osc_detune_[mNUM_OSC];
    uint16_t cutoff_;
    uint16_t resonance_;
    uint16_t filter_env_amount_;
    uint16_t filter_sustain_level_;
    uint16_t shift_val_;
    uint16_t sustain_level_;
    int32_t noise_upper_;
    int32_t noise_sub_;
    bool note_is_on_;
    int32_t last_note_;

    bool phase_sync_;
};

#endif // MSUBSYNTH_H_