// recreation of fors pop max for live device

#ifndef MPOP_VOICE
#define MPOP_VOICE

#include <tables/sin8192_int16.h>

#include <Meap.h>

enum MEAP_pop_spectra_labels
{
    mPOP_WHOLE,
    mPOP_DETUNE,
    mPOP_HALVES,
    mPOP_ODD,
    mPOP_EVEN,
    mPOP_MIXED,
};

float MEAP_pop_spectra_[6][16] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},                        // whole
    {1, 1.01, 2, 2.02, 3, 3.03, 4, 4.04, 5, 5.05, 6, 6.06, 7, 7.07, 8, 8.08},       // detune
    {1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5},               // halves
    {1, 1.01, 3, 3.03, 5, 5.05, 7, 7.07, 9, 9.09, 11, 11.11, 13, 13.13, 15, 15.15}, // odd
    {1, 1.01, 2, 2.02, 4, 4.04, 6, 6.06, 8, 8.08, 10, 10.10, 12, 12.12, 14, 14.14}, // even
    {1, 3, 5, 7, 9, 11, 13, 15, 17, 14, 12, 10, 8, 6, 4, 2}                         // mixed
};

template <class T = int32_t>
class mPopVoice
{
public:
    mPopVoice()
    {
        init();
    }

    void init()
    {
        carrier_.setTable(sin8192_int16_DATA);
        mod1_.setTable(sin8192_int16_DATA);
        mod2_.setTable(sin8192_int16_DATA);

        carrier_.setFreq(220);
        mod1_.setFreq(220);
        mod2_.setFreq(220);

        spectrum = mPOP_WHOLE;
        setADSR(1, 400, 255, 400);
        mod_env_.setADLevels(255, 100);
        mod_env_.setTimes(1, 200, 4294967295, 1);
        setModAttack(0);
        setModDecay(400);
        setModSustain(0);
        setModAmount(0.1);
        mod_env_.setReleaseTime(4294967295);
        setSpectra(0);

        feedback_amount_ = 0.1;
    }

    void setADSR(uint32_t attack_time, uint32_t decay_time, uint32_t sustain_level, uint32_t release_time)
    {
        carrier_env_.setADLevels(255, sustain_level);
        carrier_env_.setTimes(attack_time, decay_time, 4294967295, release_time);
    }

    void setReleaseTime(uint32_t release_time)
    {
        carrier_env_.setReleaseTime(release_time);
    }

    void setModAttack(uint32_t attack_time)
    {
        mod_env_.setAttackTime(attack_time);
    }

    void setModDecay(uint32_t decay_time)
    {
        mod_env_.setDecayTime(decay_time);
    }

    void setModSustain(uint32_t sustain_level)
    {
        mod_env_.setADLevels(255, sustain_level);
    }

    void setModAmount(float modulation_amount)
    {
        modulation_amount_ = modulation_amount;
    }

    void setFeedback(float fdbk)
    {
        feedback_amount_ = fdbk;
    }

    // 0-6
    void setSpectraMap(int index)
    {
        spectrum = (MEAP_pop_spectra_labels)index;
        setSpectra(ratio_);
    }

    // 0-15
    void setSpectra(int32_t index)
    {
        ratio_ = index;
        int32_t b_index = index + 1;
        if (b_index >= 16)
        {
            b_index = 14;
        }
        mod1_ratio_ = MEAP_pop_spectra_[(int)spectrum][index];
        mod2_ratio_ = MEAP_pop_spectra_[(int)spectrum][b_index];
    }

    void noteOn(int8_t note, int8_t velocity)
    {
        float my_freq = mtof(note);
        carrier_.setFreq(my_freq);
        mod1_.setFreq(my_freq * mod1_ratio_);
        mod2_.setFreq(my_freq * mod2_ratio_);

        carrier_env_.noteOn();
        mod_env_.noteOn();
    }

    void noteOff()
    {
        carrier_env_.noteOff();
        mod_env_.noteOff();
    }

    T next()
    {
        carrier_env_.update();
        mod_env_.update();

        int32_t mod_env_val = mod_env_.next();
        int32_t feedback_env_val = (mod_env_val * 0.75) + 64;

        UFix<16, 16> mod1_dev = 1.0;
        UFix<16, 16> mod2_dev = 1.0;

        auto mod1_val = mod1_dev * toSFraction((int16_t)(((feedback_val_1_ * feedback_env_val) >> 8) * feedback_amount_));
        auto mod2_val = mod2_dev * toSFraction((int16_t)(((feedback_val_2_ * feedback_env_val) >> 8) * feedback_amount_));

        feedback_val_1_ = mod1_.phMod(mod1_val); // envelope should control feedback amount here, may need to ditch the operators and do it manually
        feedback_val_2_ = mod2_.phMod(mod2_val);

        // T out_sample = carrier_.next(feedback_val_A_ + feedback_val_B_);

        int32_t mod_input = (((feedback_val_1_ + feedback_val_2_) * mod_env_val) >> 8) * modulation_amount_;

        UFix<16, 16> deviation = 1.0;
        auto modulation = deviation * toSFraction((int16_t)(mod_input));

        T out_sample = ((carrier_.phMod(modulation) * carrier_env_.next()) >> 8);
        // T out_sample = (carrier_.next() * carrier_env_.next()) >> 8;
        // T out_sample = carrier_.next();
        // return (last_out_ * env_val_ * gain_) >> shift_val_; // returns a 16 bit sample

        return out_sample;
    }

    // CLASS VARIABLES

    mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> carrier_;

    ADSR<AUDIO_RATE, AUDIO_RATE> carrier_env_;

    mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> mod1_;
    mOscil<sin8192_int16_NUM_CELLS, AUDIO_RATE, int16_t> mod2_;

    ADSR<AUDIO_RATE, AUDIO_RATE> mod_env_;

    float mod1_ratio_;
    float mod2_ratio_;

    T feedback_val_1_;
    T feedback_val_2_;

    float feedback_amount_;

    float modulation_amount_;

    MEAP_pop_spectra_labels spectrum;
    int ratio_;
};

#endif // MPOP_VOICE