#ifndef MEAP_CHOPPER_H_
#define MEAP_CHOPPER_H_

template <uint32_t mAUDIO_RATE>

class mChopper
{
public:
    //! Class constructor.
    mChopper(void)
    {
        lower_ = 10;
        higher_ = 100;
        sustain_ = 50;
        adsr_.setTimes(1, 1, sustain_, 1);
        adsr_.setADLevels(255, 255);
        active_ = false;
        end_time_ = 0;
        pan_ = 127;
    };

    void trigger()
    {
        sustain_ = Meap::irand(lower_, higher_);
        adsr_.setSustainTime(sustain_);
        // if (adsr_.playing())
        // {
        //     return;
        // }
        adsr_.noteOn();
        end_time_ = millis() + sustain_;
        pan_ = Meap::irand(0, 255);
    }

    void setLengthBounds(uint64_t lower, uint64_t higher)
    {
        lower_ = lower;
        higher_ = higher;
    }

    void enable()
    {
        active_ = true;
    }

    void disable()
    {
        active_ = false;
    }

    uint64_t next(uint64_t in_sample)
    {
        if (active_)
        {
            if (millis() > end_time_)
            {
                adsr_.noteOff();
            }
            adsr_.update();
            gain_ = adsr_.next();
            in_sample = (in_sample * gain_) >> 8;
            pan_out_ = Meap::pan2(in_sample, pan_);
            l_sample_ = pan_out_.l;
            r_sample_ = pan_out_.r;

            // if(pan_){
            //     l_sample_ = in_sample;
            //     r_sample_ = 0;
            // } else {
            //     l_sample_ = 0;
            //     r_sample_ = in_sample;
            // }
            return in_sample; // 8 bit gain * 8 bit sample = 16bit result
        }
        else
        {
            l_sample_ = in_sample >> 1;
            r_sample_ = in_sample >> 1;
            return in_sample;
        }
    }

    uint64_t getChannel(uint8_t channel_num)
    {
        if (channel_num == 0)
        {
            return l_sample_;
        }
        else if (channel_num == 1)
        {
            return r_sample_;
        }
    }

protected:
    uint64_t gain_;
    uint64_t lower_;
    uint64_t higher_;
    uint64_t sustain_;
    ADSR<mAUDIO_RATE, mAUDIO_RATE> adsr_;
    bool active_;
    uint64_t end_time_;
    uint8_t pan_;
    uint64_t l_sample_;
    uint64_t r_sample_;
    StereoSample pan_out_;
};

#endif // MEAP_CHOPPER_H_
