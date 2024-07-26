#ifndef MEAP_SPARKLER_H_
#define MEAP_SPARKLER_H_

template <uint64_t mMAX_SAMPLE_LENGTH, uint8_t POLYPHONY, uint64_t mAUDIO_RATE>
class mDrumRack
{
public:
    mDrumRack(const int8_t **sample_list, uint32_t *sample_lengths, uint8_t *base_address)
    {
        sample_list_ = sample_list;
        sample_lengths_ = sample_lengths;

        base_address_ = base_address;

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;

        default_freq_ = (float)mAUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

        for (uint8_t i; i < mNUM_SAMPLES; i++)
        {
            // sample_bank_[i].setTable(base_address_[i]);
            sample_bank_[i].setTable(sample_list_[i], sample_lengths[i]);
        }
    };

    void begin()
    {
        // start_time = millis();
        playing_ = true;
        current_address_ = base_address_;
        pulse_counter_ = 0;
        message_type_ = current_address_[0];
        data1_ = current_address_[1];
        data2_ = current_address_[2];
        time_ = (current_address_[3] << 8) + current_address_[4];
    }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        for (uint16_t i = 0; i < mNUM_SAMPLES; i++)
        {
            sample_bank_[i].noteOff(255);
        }
    }

    void updateMidi()
    {
        if (playing_)
        {
            pulse_counter_ += 1;
            // Serial.println(pulse_counter_);

            while (pulse_counter_ >= time_)
            {

                Serial.print(message_type_);
                Serial.print(" ");
                Serial.print(data1_);
                Serial.print(" ");
                Serial.print(data2_);
                Serial.print(" ");
                Serial.print(current_address_[3]);
                Serial.print(" ");
                Serial.print(current_address_[4]);
                Serial.print(" ");
                Serial.println(time_);

                switch (message_type_) // notes are indexed from 0 on sample_bank starting from C-1 (0)
                {
                case 0x80: // note off
                    sample_bank_[data1_].noteOff(255);
                    break;
                case 0x90: // note on
                    sample_bank_[data1_].noteOn(default_freq_, 255);
                    break;
                case 255: // end of file
                    playing_ = false;
                    break;
                }
                pulse_counter_ = 0;

                message_type_ = current_address_[0];
                data1_ = current_address_[1];
                data2_ = current_address_[2];
                time_ = (current_address_[3] << 8) + current_address_[4];
                current_address_ += 5;
            }
        }
    }

    bool isPlaying()
    {
        return playing_;
    }

    void noteOn(uint16_t sample_num, float freq)
    {
        sample_bank_[sample_num].noteOn(freq, 255);
    }

    void trig(uint16_t sample_num)
    {
        sample_bank_[sample_num].setLoopingOn();
        sample_bank_[sample_num].noteOn(default_freq_, 255);
    }

    void untrig(uint16_t sample_num)
    {
        sample_bank_[sample_num].setLoopingOff();
        sample_bank_[sample_num].noteOff(255);
    }

    void noteOff(uint16_t sample_num)
    {
        sample_bank_[sample_num].noteOff(255);
    }

    int32_t next()
    {
        // updateMidi();
        uint32_t output_sample = 0;
        for (uint8_t i = 0; i < mNUM_SAMPLES; i++)
        {
            output_sample += sample_bank_[i].next();
        }
        return output_sample;
    }

protected:
    // sampler voices
    mSampler<mMAX_SAMPLE_LENGTH, mAUDIO_RATE> sample_bank_[POLYPHONY];
    const int8_t **sample_list_;
    uint32_t *sample_lengths_;

    float default_freq_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;
};

#endif // MEAP_SPARKLER_H_
