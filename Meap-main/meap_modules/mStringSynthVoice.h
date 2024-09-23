#ifndef MSTRINGSYNTH_H_
#define MSTRINGSYNTH_H_

#include <tables/saw8192_int8.h> // loads saw wavetable
#include <tables/sin8192_int8.h> // loads sin wavetable

template <class T = int8_t>
class mStringSynthVoice
{
public:
    mStringSynthVoice()
    {
        init();
    };

    void init()
    {
        detune = 0;
        ensemble_mix = 0;
        phaser_freq = 0.63;

        my_freq = 220;
        osc_[0].setFreq(my_freq);
        osc_[0].setTable(SAW8192_DATA);
        osc_[1].setFreq(my_freq);
        osc_[1].setTable(SAW8192_DATA);
        adsr_.setTimes(10, 10, 4294967295, 500);
        adsr_.setADLevels(255, 255);

        phaser_lfo.setFreq(phaser_freq);
        phaser_lfo.setTable(SIN8192_DATA);

        filter_.setCutoffFreqAndResonance(255, 127);
    }

    // to be called in control loop
    void update()
    {
        adsr_.update();
        delay_samples = 128 + phaser_lfo.next();
        float my_detune_freq = my_freq + (my_freq / 50) * detune / 127.0;
        osc_[1].setFreq(my_detune_freq);
    }

    void setADSR(uint32_t a_t, uint32_t d_t, uint16_t s_l, uint32_t r_t)
    {
        adsr_.setTimes(a_t, d_t, 4294967295, r_t);
        adsr_.setADLevels(255, s_l);
    }

    void setCutoffFreqAndResonance(uint32_t c, uint32_t r)
    {
        filter_.setCutoffFreqAndResonance(c, r);
    }

    void setDetune(uint32_t d_v)
    {
        detune = d_v;
    }

    void setEnsembleMix(uint32_t e_v)
    {
        ensemble_mix = e_v;
    }

    void setPhaserFreq(float p_f)
    {
        phaser_freq = p_f;
        phaser_lfo.setFreq(phaser_freq);
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        my_freq = mtof(note);
        osc_[0].setFreq(my_freq);
        float my_detune_freq = my_freq + (my_freq / 50) * detune / 127.0;
        osc_[1].setFreq(my_detune_freq);
        phaser_lfo.setPhase(Meap::irand(0, 8191));
        osc_[1].setPhase(Meap::irand(0, 8191));

        adsr_.noteOn();
    }

    void noteOff()
    {
        adsr_.noteOff();
    }

    // to be called in audio loop
    int32_t next()
    {
        int32_t out_sample = 0;

        out_sample += (osc_[0].next() + osc_[1].next()) * adsr_.next(); // 17bit

        filter_.next(out_sample);
        out_sample = filter_.low();

        // SOMETHING ABOUT THE PHASER IS CAUSING CRAZY GLITCHES SO TAKING IT OUT FOR NOW
        // int sample_to8bit = out_sample >> 11;
        // int phaser_sample = delay_line.next(sample_to8bit, delay_samples) << 11;

        // // int32_t phaser_sample = delay_line.next(out_sample, delay_samples);
        // phaser_sample = (phaser_sample * ensemble_mix) >> 8;
        // out_sample += phaser_sample;

        return out_sample;
    }

protected:
    Oscil<8192, AUDIO_RATE> osc_[2];

    Oscil<8192, CONTROL_RATE> phaser_lfo;
    float my_freq;

    AudioDelay<512, int32_t> delay_line;
    int32_t delay_samples;

    ADSR<CONTROL_RATE, AUDIO_RATE> adsr_;
    MultiResonantFilter<uint8_t> filter_; /**< Filter for left channel */

    uint32_t detune;
    uint32_t ensemble_mix;
    float phaser_freq;
};

#endif // MSTRINGSYNTH_H_