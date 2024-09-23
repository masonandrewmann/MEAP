#ifndef MSTRINGSYNTHPOLY_H_
#define MSTRINGSYNTHPOLY_H_

#include <dependencies/LinkedList/LinkedList.h>

template <uint16_t mPOLYPHONY = 4, class T = int8_t>
class mStringSynthPoly
{
public:
    mStringSynthPoly()
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].init();
            free_voices_.unshift(i); // add all voices to voice queue
        }
        curr_voice_ = 0;
        midi_table_name_ = NULL;
        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;
    }

    mStringSynthPoly(uint8_t *midi_table_name)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].init();
            free_voices_.unshift(i); // add all voices to voice queue
        }
        curr_voice_ = 0;
        midi_table_name_ = midi_table_name;
        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        pulse_counter_ = 0;
    }

    void begin()
    {
        // start_time = millis();
        playing_ = true;
        current_midi_address_ = midi_table_name_;
        pulse_counter_ = 0;
        message_type_ = current_midi_address_[0];
        data1_ = current_midi_address_[1];
        data2_ = current_midi_address_[2];
        time_ = (current_midi_address_[3] << 8) + current_midi_address_[4];
    }

    void setADSR(uint32_t a_t, uint32_t d_t, uint16_t s_l, uint32_t r_t)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setADSR(a_t, d_t, s_l, r_t);
        }
    }

    void setCutoffFreqAndResonance(uint32_t c, uint32_t r)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setCutoffFreqAndResonance(c, r);
        }
    }

    void setDetune(uint32_t d_v)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setDetune(d_v);
        }
    }

    void setEnsembleMix(uint32_t e_v)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setEnsembleMix(e_v);
        }
    }

    void setPhaserFreq(float p_f)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setPhaserFreq(p_f);
        }
    }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
        }
    }

    void updateMidi()
    {
        if (playing_)
        {
            pulse_counter_ += 1;
            while (pulse_counter_ >= time_)
            {
                switch (message_type_) // notes are indexed from 0 on sample_bank starting from C-1 (0)
                {
                case 0x80: // note off
                    noteOff(data1_);
                    break;
                case 0x90: // note on
                    if (data1_ != 127)
                    {
                        noteOn(data1_, data2_);
                    }
                    break;
                case 255: // end of file
                    playing_ = false;
                    return;
                    break;
                }
                current_midi_address_ += 5;
                pulse_counter_ = 0;
                message_type_ = current_midi_address_[0];
                data1_ = current_midi_address_[1];
                data2_ = current_midi_address_[2];
                time_ = (current_midi_address_[3] << 8) + current_midi_address_[4];
            }
        }
    }

    bool isPlaying()
    {
        return playing_;
    }

    void noteOn(uint16_t note, float vel)
    {
        if (free_voices_.size() > 0)
        {
            curr_voice_ = free_voices_.shift(); // remove head element and return it!
        }
        else
        {
            return; // no free voices, move along...
        }

        voices[curr_voice_].noteOn(note, vel);

        // store note in pressed notes queue
        MeapNoteAndVoice *my_note = new MeapNoteAndVoice;
        my_note->note_num = note;
        my_note->voice_num = curr_voice_;
        nonfree_voices_.add(my_note);
    }

    void noteOff(uint16_t note)
    {

        uint8_t num_nonfree_voices = nonfree_voices_.size();
        for (uint8_t i = 0; i < num_nonfree_voices; i++)
        {
            if (nonfree_voices_.get(i)->note_num == note)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                voices[voice_num].noteOff();
                free_voices_.add(voice_num);     // re-add voice to free queue
                delete (nonfree_voices_.get(i)); // delete voice from pressed queue
                nonfree_voices_.remove(i);       // remove freed voice from pressed queue
                return;
            }
        }
    }

    void flush()
    {
        nonfree_voices_.clear();
        free_voices_.clear();
        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            free_voices_.unshift(i); // add all voices to voice queue
            voices[i].noteOff();
        }
    }

    void update()
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].update();
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            out_sample += voices[i].next();
        }

        return out_sample;
    }

    uint8_t *current_midi_address_;

    mStringSynthVoice<T> voices[mPOLYPHONY];

protected:
    LinkedList<int16_t> free_voices_;
    LinkedList<MeapNoteAndVoice *> nonfree_voices_;

    uint16_t curr_voice_;

    // reading midi file
    uint8_t *midi_table_name_;

    uint64_t pulse_counter_;

    bool playing_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;
};

#endif // MSTRINGSYNTHPOLY_H_
