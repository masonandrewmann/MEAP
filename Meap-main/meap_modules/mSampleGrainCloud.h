#ifndef MSAMPLEGRAINCLOUD_H_
#define MSAMPLEGRAINCLOUD_H_

template <uint32_t mSAMPLE_LENGTH, uint32_t mAUDIO_RATE = AUDIO_RATE, uint32_t mCONTROL_RATE = CONTROL_RATE, uint16_t mPOLYPHONY = 8, class T = int8_t>
class mSampleGrainCloud
{
public:
    mSampleGrainCloud(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    mSampleGrainCloud() {
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

        grain_freq_min_[0] = 0.5; // initial
        grain_freq_min_[1] = 0.5; // ending
        freq_min_difference_ = grain_freq_min_[1] - grain_freq_min_[0];

        grain_freq_max_[0] = 2.0; // initial
        grain_freq_max_[0] = 2.0; // ending
        freq_max_difference_ = grain_freq_max_[1] - grain_freq_max_[0];

        grain_duration_[0] = 8;
        grain_duration_[1] = 30;
        duration_difference_ = grain_duration_[1] - grain_duration_[0];

        cont_tables[0] = TABLE_NAME;
        cont_tables[1] = TABLE_NAME;
        cont_ends[0] = mSAMPLE_LENGTH;
        cont_ends[1] = mSAMPLE_LENGTH;
        cont_density_ = 5;
        cont_amplitude_ = 255;
        cont_freq_min_ = 0.5;
        cont_freq_max_ = 2.0;
        cont_duration_ = 10;
        cont_pos_ = 0;
        cont_rand_ = 0;
        cont_pan_ = 127;
        cont_table_fader = 0; // 0-255

        l_sample = 0;
        r_sample = 0;
    }

    // takes in densities in terms of grains per second, and converts them to thresholds to be used internally
    void setDensities(uint16_t starting_density, uint16_t ending_density)
    {
        grain_density_[0] = (float)starting_density / (float)mCONTROL_RATE * 1000;
        grain_density_[1] = (float)ending_density / (float)mCONTROL_RATE * 1000;
        density_difference_ = grain_density_[1] - grain_density_[0];
    }

    void setFreqMins(float starting_min, float ending_min)
    {
        grain_freq_min_[0] = starting_min;
        grain_freq_min_[1] = ending_min;
        freq_min_difference_ = grain_freq_min_[1] - grain_freq_min_[0];
    }

    void setFreqMaxs(float starting_max, float ending_max)
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

    void enableContinuousMode()
    {
        continuous_mode_ = true;
    }

    void disableContinuousMode()
    {
        continuous_mode_ = false;
    }

    void update()
    {
        if (continuous_mode_)
        {
            // update density
            if (grains[curr_grain_].playing())
            {
                return;
            }
            if (Meap::irand(0, 255) < cont_density_)
            {
                // update other grain parameters
                grains[curr_grain_].setAmplitude(cont_amplitude_);
                grains[curr_grain_].setDuration(cont_duration_);

                grains[curr_grain_].setPan(cont_pan_);

                float curr_freq_min = cont_freq_min_;
                float curr_freq_max = cont_freq_max_;

                float my_freq = ((float)Meap::irand((int)(curr_freq_min * 10000), (int)(curr_freq_max * 10000))) / 10000.f; // this is so scuffed lmao

                int table_num;
                if (Meap::irand(0, 255) >= cont_table_fader)
                {
                    table_num = 0;
                }
                else
                {
                    table_num = 1;
                }

                grains[curr_grain_].setTableAndEnd(cont_tables[table_num], cont_ends[table_num]);

                uint64_t grain_pos = map(cont_pos_, 0, 4095, 0, cont_ends[table_num]) + map(Meap::irand(0, cont_rand_), 0, 4095, 0, cont_ends[table_num] >> 1);
                // Serial.println(grain_pos);
                grains[curr_grain_].noteOn(my_freq, grain_pos); // moves through sample in real time

                // grains[curr_grain_].noteOn(my_freq, cont_pos_ + map(Meap::irand(0, cont_rand_), 0, 4095, 0, mSAMPLE_LENGTH >> 1)); // moves through sample in real time

                curr_grain_ = (curr_grain_ + 1) % mPOLYPHONY;
            }
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
                uint64_t samples_elapsed = ((float)elapsed / 1000.0) * mAUDIO_RATE; // rlly inefficient!
                float elapsed_over_cloud_length = (float)elapsed / (float)cloud_length_;
                // update density
                int32_t curr_density = grain_density_[0] + elapsed_over_cloud_length * density_difference_;

                if (Meap::irand(0, 1000) < curr_density)
                {
                    // update other grain parameters
                    grains[curr_grain_].setAmplitude(grain_amplitude_[0] + elapsed_over_cloud_length * amplitude_difference_);
                    grains[curr_grain_].setDuration(grain_duration_[0] + elapsed_over_cloud_length * duration_difference_);

                    float curr_freq_min = grain_freq_min_[0] + elapsed_over_cloud_length * freq_min_difference_;
                    float curr_freq_max = grain_freq_max_[0] + elapsed_over_cloud_length * freq_max_difference_;

                    float my_freq = ((float)Meap::irand((int)(curr_freq_min * 10000), (int)(curr_freq_max * 10000))) / 10000.f; // this is so scuffed lmao
                    // trigger a grain
                    grains[curr_grain_].noteOn(my_freq, samples_elapsed % mSAMPLE_LENGTH); // moves through sample in real time
                    curr_grain_ = (curr_grain_ + 1) % mPOLYPHONY;
                }
            }
        }
    }

    // to be called in audio loop
    int32_t next()
    {
        l_sample = 0;
        r_sample = 0;
        for (int i = 0; i < mPOLYPHONY; i++)
        {
            l_sample += grains[i].next();
            r_sample += grains[i].r_sample;
        }

        return l_sample;
    }

    bool continuous_mode_;
    int32_t cont_density_;
    int32_t cont_amplitude_;
    float cont_freq_min_;
    float cont_freq_max_;
    int32_t cont_duration_;
    int32_t cont_pos_;
    int32_t cont_rand_;
    int32_t cont_pan_;
    const T *cont_tables[2];
    int32_t cont_ends[2];
    int16_t cont_table_fader;

    int32_t l_sample;
    int32_t r_sample;

protected:
    mSampleGrainGenerator<mSAMPLE_LENGTH, mAUDIO_RATE, T> grains[mPOLYPHONY];

    uint64_t cloud_end_time_;
    bool cloud_active_;

    uint32_t cloud_length_;
    int32_t grain_density_[2];   // initial, ending
    int32_t density_difference_; // so we don't need to calculate this every loop
    int32_t grain_amplitude_[2]; // initial, ending
    int32_t amplitude_difference_;
    float grain_freq_min_[2]; // initial, ending
    float freq_min_difference_;
    float grain_freq_max_[2]; // initial, ending
    float freq_max_difference_;
    int32_t grain_duration_[2];
    int32_t duration_difference_;

    uint64_t start_time_;

    int curr_grain_;

    // waveform table?
    // panning,
    // randomization???
};

#endif // MSAMPLEGRAINCLOUD_H_