#ifndef MEAP_BITCRUSHER_H_
#define MEAP_BITCRUSHER_H_

class mBitcrusher
{
public:
    //! Class constructor.
    mBitcrusher(int sample_rate = 32768, int bits = 0)
    {
        last_sample_ = 0;
        setSampleRate(sample_rate);
        setBits(bits);
        repeat_counter = 0;
        repeat_target = 0;
    };

    // set number of bits to reduce signal by
    void setBits(uint8_t bits)
    {
        bit_reduction_ = bits;
    }

    // target sample rate (0-32768)
    void setSampleRate(float sample_rate)
    {
        repeat_target = round((float)AUDIO_RATE / sample_rate);
    }

    int32_t next(int32_t in_sample)
    {
        // likely the worst "sample rate conversion" ever achieved wow sounds alright though
        if (repeat_counter >= repeat_target)
        {
            repeat_counter = 0;
            last_sample_ = in_sample;
        }
        else
        {
            repeat_counter++;
            in_sample = last_sample_;
        }

        // shift bits
        in_sample = (in_sample >> bit_reduction_) << bit_reduction_;

        return in_sample;
    }

    // CLASS VARIABLES
    uint8_t bit_reduction_; // 16 = off
    int32_t last_sample_;
    int32_t repeat_target;
    int32_t repeat_counter;
};

#endif // MEAP_BITCRUSHER_H_
