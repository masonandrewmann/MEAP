#ifndef MEAP_WAVESHAPER_H_
#define MEAP_WAVESHAPER_H_

template <class TABLE_TYPE = int8_t, class AUDIO_TYPE = int32_t>
class mWaveshaper
{
public:
    //! Class constructor.
    mWaveshaper(void)
    {
        shift_amount = sizeof(TABLE_TYPE) - sizeof(AUDIO_TYPE);
        if (shift_amount > 0)
        {
            negative_shift = true;
            shift_amount *= -1;
        }
        else
        {
            negative_shift = false;
        }
    };

    AUDIO_TYPE next(AUDIO_TYPE in_sample)
    {
        TABLE_TYPE audio_index;
        if (negative_shift)
        {
            audio_index = in_sample >> shift_amount;
        }
        else
        {
            audio_index = in_sample << shift_amount;
        }
    }

    // CLASS VARIABLES
    const TABLE_TYPE *shaper_table;
    int8_t shift_amount;
    bool negative_shift;
};

#endif // MEAP_WAVESHAPER_H_
