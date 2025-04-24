#ifndef MEAP_BITCRUSHER_H_
#define MEAP_BITCRUSHER_H_

class mBitcrusher
{
public:
    //! Class constructor.
    mBitcrusher(void)
    {
        last_sample_ = 0;
        setSampleRate(32768);
        setBits(0);
    };

    // set number of bits to reduce signal by
    void setBits(uint8_t bits)
    {
        bit_reduction_ = bits;
    }

    // target sample rate (0-32768)
    void setSampleRate(float sample_rate)
    {
        sample_step_ = sample_rate / (float)AUDIO_RATE;
    }

    int32_t next(int32_t in_sample)
    {
        // shift bits
        in_sample = (in_sample >> bit_reduction_) << bit_reduction_;

        // shift sample rate
        sample_index_ = sample_index_ + sample_step_;
        if (sample_index_ >= 1)
        {
            while (sample_index_ > 1)
            {
                sample_index_--;
            }
            last_sample_ = in_sample;
        }
        return last_sample_;
    }

    // CLASS VARIABLES
    uint8_t bit_reduction_; // 16 = off
    float sample_step_;
    float sample_index_;
    int32_t last_sample_;
};

#endif // MEAP_BITCRUSHER_H_
