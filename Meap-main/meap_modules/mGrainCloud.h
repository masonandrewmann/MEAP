#ifndef MGRAINCLOUD_H_
#define MGRAINCLOUD_H_

#include <tables/sin8192_int8.h> // table for Oscils to play

template <uint32_t mNUM_CELLS = SIN8192_NUM_CELLS, uint32_t mAUDIO_RATE = AUDIO_RATE, uint32_t mCONTROL_RATE = CONTROL_RATE, uint16_t mPOLYPHONY = 8, class T = int8_t>
class mGrainCloud
{
public:
    mGrainCloud(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    mGrainCloud()
    {
        init(SIN8192_DATA);
    };

    void init(const T *TABLE_NAME)
    {
        for (int i = 0; i < mPOLYPHONY; i++)
        {
            grains[i].init(TABLE_NAME);
        }
        cloud_end_time_ = 0;
        cloud_active_ = false;
        continuous_mode_ = false;

        start_time_ = 0;
        cloud_length_ = 2000;
        setDensities(10, 60);
        density_difference_ = grain_density_[1] - grain_density_[0];
        curr_grain_ = 0;

        grain_amplitude_[0] = 100; // initial
        grain_amplitude_[1] = 255; // ending
        amplitude_difference_ = grain_amplitude_[1] - grain_amplitude_[0];

        grain_freq_min_[0] = 100; // initial
        grain_freq_min_[1] = 500; // ending
        freq_min_difference_ = grain_freq_min_[1] - grain_freq_min_[0];

        grain_freq_max_[0] = 300;  // initial
        grain_freq_max_[0] = 2000; // ending
        freq_max_difference_ = grain_freq_max_[1] - grain_freq_max_[0];

        grain_duration_[0] = 8;
        grain_duration_[1] = 30;
        duration_difference_ = grain_duration_[1] - grain_duration_[0];
    }

    // takes in densities in terms of grains per second, and converts them to thresholds to be used internally
    void setDensities(uint16_t starting_density, uint16_t ending_density)
    {
        grain_density_[0] = (float)starting_density / (float)mCONTROL_RATE * 1000;
        grain_density_[1] = (float)ending_density / (float)mCONTROL_RATE * 1000;
        density_difference_ = grain_density_[1] - grain_density_[0];
    }

    void setFreqMins(uint16_t starting_min, uint16_t ending_min)
    {
        grain_freq_min_[0] = starting_min;
        grain_freq_min_[1] = ending_min;
        freq_min_difference_ = grain_freq_min_[1] - grain_freq_min_[0];
    }

    void setFreqMaxs(uint16_t starting_max, uint16_t ending_max)
    {
        grain_freq_max_[0] = starting_max;
        grain_freq_max_[1] = ending_max;
        freq_max_difference_ = grain_freq_max_[1] - grain_freq_max_[0];
    }

    void setDurations(uint16_t starting_duration, uint16_t ending_duration)
    {
        grain_duration_[0] = starting_duration;
        grain_duration_[1] = ending_duration;
        duration_difference_ = grain_duration_[1] - grain_duration_[0];
    }

    void setAmplitudes(uint16_t starting_amplitude, uint16_t ending_amplitude)
    {
        grain_amplitude_[0] = starting_amplitude;
        grain_amplitude_[1] = ending_amplitude;
        amplitude_difference_ = grain_amplitude_[1] - grain_amplitude_[0];
    }

    void setTable(const T *TABLE_NAME)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            grains[i].setTable(TABLE_NAME);
        }
    }

    void setLength(uint32_t cloud_length)
    {
        cloud_length_ = cloud_length;
    }

    void noteOn()
    {
        cloud_active_ = true;
        start_time_ = millis();
        cloud_end_time_ = start_time_ + cloud_length_;
    }

    void update()
    {
        if (continuous_mode_)
        {
        }
        else
        {
            if (millis() > cloud_end_time_)
            {
                cloud_active_ = false;
                return;
            }
            else
            {
                uint64_t curr_time = millis();
                uint64_t elapsed = curr_time - start_time_;
                float elapsed_over_cloud_length = (float)elapsed / (float)cloud_length_;
                // update density
                int32_t curr_density = grain_density_[0] + elapsed_over_cloud_length * density_difference_;

                if (Meap::irand(0, 1000) < curr_density)
                {
                    // update other grain parameters
                    grains[curr_grain_].setAmplitude(grain_amplitude_[0] + elapsed_over_cloud_length * amplitude_difference_);
                    grains[curr_grain_].setDuration(grain_duration_[0] + elapsed_over_cloud_length * duration_difference_);

                    // trigger a grain
                    grains[curr_grain_].noteOn(Meap::irand(grain_freq_min_[0] + elapsed_over_cloud_length * freq_min_difference_, grain_freq_max_[0] + elapsed_over_cloud_length * freq_max_difference_));
                    curr_grain_ = (curr_grain_ + 1) % mPOLYPHONY;
                }
            }
        }
    }

    // to be called in audio loop
    int32_t next()
    {
        int32_t out_sample = 0;
        for (int i = 0; i < mPOLYPHONY; i++)
        {
            out_sample += grains[i].next();
        }

        return out_sample;
    }

protected:
    mGrainGenerator<mNUM_CELLS, mAUDIO_RATE> grains[mPOLYPHONY];

    uint64_t cloud_end_time_;
    bool cloud_active_;

    uint32_t cloud_length_;
    int32_t grain_density_[2];   // initial, ending
    int32_t density_difference_; // so we don't need to calculate this every loop
    int32_t grain_amplitude_[2]; // initial, ending
    int32_t amplitude_difference_;
    int32_t grain_freq_min_[2]; // initial, ending
    int32_t freq_min_difference_;
    int32_t grain_freq_max_[2]; // initial, ending
    int32_t freq_max_difference_;
    int32_t grain_duration_[2];
    int32_t duration_difference_;

    int32_t cont_density_;
    int32_t cont_amplitude_;
    int32_t cont_freq_min_;
    int32_t cont_freq_max_;
    int32_t cont_duration_;

    uint64_t start_time_;

    int curr_grain_;

    // waveform table?
    // panning,
    // randomization???

    bool continuous_mode_;

    int32_t l_sample_;
    int32_t r_sample_;
};

#endif // MGRAINCLOUD_H_