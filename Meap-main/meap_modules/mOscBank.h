#ifndef MEAP_OSCBANK_H
#define MEAP_OSCBANK_H

#include <dependencies/LinkedList/LinkedList.h>

template <uint32_t mNUM_CELLS, uint32_t mAUDIO_RATE, uint16_t mCONTROL_RATE, uint8_t mPOLYPHONY>
class mOscBank
{
public:
    mOscBank(const int8_t *TABLE_NAME, uint8_t *base_address = NULL)
    {
        a_ = 1;
        d_ = 100;
        s_ = 4294967295;
        r_ = 1;

        a_l_ = 255;
        d_l_ = 255;

        table_ = TABLE_NAME;
        base_address_ = base_address;

        transpose_ = 0;
        detune_amount_ = 1.0;
        detune_enable_ = false;

        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            osc_[i].setTable(table_);
            osc_detune_[i].setTable(table_);
            freq_[i] = 220.0;
            osc_[i].setFreq(freq_[i]);
            adsr_[i].setTimes(a_, d_, s_, r_);
            adsr_[i].setADLevels(a_l_, d_l_);
            gain_[i] = 0;
            // set up voice queue
            free_queue_.unshift(i);
        }

        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;
    };

    void setTimes(uint64_t a, uint64_t d, uint64_t s, uint64_t r)
    {
        // adsr_.setTimes(a, d, s, r);
        a_ = a;
        d_ = d;
        s_ = s;
        r_ = r;
    }

    void setAttackTime(uint64_t a)
    {
        a_ = a;
    }

    void setReleaseTime(uint64_t r)
    {
        r_ = r;
    }

    void setADLevels(int a_l, int d_l)
    {
        // adsr_.setADLevels(a_t, d_t);
        a_l_ = a_l;
        d_l_ = d_l;
    }

    void setTranspose(int8_t transpose)
    {
        transpose_ = transpose;
    }

    void enableDetune()
    {
        detune_enable_ = true;
    }

    void disableDetune()
    {
        detune_enable_ = false;
    }

    void setDetuneAmount(float detune_amount)
    {
        detune_amount_ = detune_amount;
    }

    void setTable(const int8_t table)
    {
        table_ = table;
    }

    void begin()
    {
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
        adsr_.noteOff();
    }

    void updateMidi()
    {
        if (playing_)
        {
            pulse_counter_ += 1;

            while (pulse_counter_ >= time_)
            {
                switch (message_type_)
                {
                case 0x80: // note off
                    noteOff(data1_);
                    break;
                case 0x90: // note on
                    noteOn(data1_);
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

    void noteOn(int note_num)
    {
        note_num += transpose_;
        // find a free voice
        int curr_voice = -1;
        if (free_queue_.size() > 0)
        {
            curr_voice = free_queue_.shift(); // remove head element
        }
        else
        {
            return; // no free voices, move along...
        }

        // store note in pressed notes queue
        MeapNoteAndVoice *my_note = new MeapNoteAndVoice;
        my_note->note_num = note_num;
        my_note->voice_num = curr_voice;
        pressed_queue_.add(my_note);

        // update parameters?
        osc_[curr_voice].setTable(table_);
        osc_detune_[curr_voice].setTable(table_);
        // osc_[curr_voice].setFrame(frame_num_);
        adsr_[curr_voice].setTimes(a_, d_, s_, r_);

        // start the note
        freq_[curr_voice] = mtof(note_num);
        osc_[curr_voice].setFreq(freq_[curr_voice]);
        adsr_[curr_voice].noteOn();
    }

    void noteOff(int note_num)
    {
        note_num += transpose_;

        // find the voice to turn off
        int8_t curr_voice = -1;
        uint8_t queue_size = pressed_queue_.size();
        for (uint8_t i = 0; i < queue_size; i++)
        {
            if (pressed_queue_.get(i)->note_num == note_num)
            {
                uint8_t voice_num = pressed_queue_.get(i)->voice_num; // voice num of note to turn off
                adsr_[voice_num].noteOff();                           // turn note off
                free_queue_.add(voice_num);                           // re-add voice to free queue
                delete (pressed_queue_.get(i));                       // delete voice from pressed queue
                pressed_queue_.remove(i);                             // remove freed voice from pressed queue
                return;
            }
        }
    }

    void setTable(const int16_t *TABLE_NAME)
    {
        table_ = TABLE_NAME;
    }

    void flush()
    {
        uint8_t queue_size = pressed_queue_.size();

        while (queue_size > 0)
        {
            uint8_t voice_num = pressed_queue_.get(0)->voice_num; // voice num of note to turn off
            adsr_[voice_num].noteOff();                           // turn note off
            free_queue_.add(voice_num);                           // re-add voice to free queue
            delete (pressed_queue_.get(0));                       // delete voice from pressed queue
            pressed_queue_.remove(0);                             // remove freed voice from pressed queue
            queue_size = pressed_queue_.size();
        }
        // uint8_t queue_size = pressed_queue_.size();
        // for (uint8_t i = queue_size; --i >= 0;)
        // {
        //     uint8_t voice_num = pressed_queue_.get(i)->voice_num; // voice num of note to turn off
        //     adsr_[voice_num].noteOff();                           // turn note off
        //     free_queue_.add(voice_num);                           // re-add voice to free queue
        //     delete (pressed_queue_.get(i));                       // delete voice from pressed queue
        //     pressed_queue_.remove(i);                             // remove freed voice from pressed queue
        // }
    }

    void update()
    {
        for (int8_t i = 0; i < mPOLYPHONY; i++)
        {
            adsr_[i].update();
            detune_freq_[i] = freq_[i] * detune_amount_;
        }
    }

    int32_t next()
    {
        int32_t output_sample = 0;

        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            gain_[i] = adsr_[i].next();
            osc_detune_[i].setFreq(detune_freq_[i]);
            if (detune_enable_)
            {
                output_sample += (osc_[i].next() + osc_detune_[i].next()) * gain_[i];
            }
            else
            {
                output_sample += (osc_[i].next() << 1) * gain_[i];
            }

            // output_sample += osc_detune_[i].next() * gain_[i];
            // }
        }

        return output_sample;
    }

protected:
    Oscil<mNUM_CELLS, mAUDIO_RATE> osc_[mPOLYPHONY];
    Oscil<mNUM_CELLS, mAUDIO_RATE> osc_detune_[mPOLYPHONY];

    float detune_amount_;
    bool detune_enable_;

    ADSR<mCONTROL_RATE, mAUDIO_RATE> adsr_[mPOLYPHONY];
    uint16_t gain_[mPOLYPHONY];
    float freq_[mPOLYPHONY];
    float detune_freq_[mPOLYPHONY];

    uint32_t a_, d_, s_, r_;
    uint8_t a_l_, d_l_;

    int8_t transpose_;

    // uint16_t frame_num_;
    const int8_t *table_;

    // voice queue
    LinkedList<int16_t> free_queue_;
    LinkedList<MeapNoteAndVoice *> pressed_queue_;

    // reading midi file
    uint8_t *base_address_;
    uint8_t *current_address_;
    uint64_t pulse_counter_;

    bool playing_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;
};

#endif // MEAP_OSCBANK_H
