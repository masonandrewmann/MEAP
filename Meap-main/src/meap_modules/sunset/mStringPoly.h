#ifndef mSTRING_POLY_H_
#define mSTRING_POLY_H_

#include <dependencies/LinkedList/LinkedList.h>
#include <meap_modules/instruments/mString.h>

template <uint16_t mPOLYPHONY = 4, class T = int8_t>
class mStringPoly
{
public:
    mStringPoly(float lowest_freq, uint8_t *midi_table_name)
    {
        init(lowest_freq, midi_table_name);
    };

    mStringPoly(float lowest_freq)
    {
        init(lowest_freq, NULL);
    };

    mStringPoly()
    {
        init(NULL, NULL, NULL);
    }

    void init(float lowest_freq, uint8_t *midi_table_name)
    {
        curr_voice_ = 0;
        midi_table_name_ = midi_table_name;
        playing_ = false;

        message_type_ = 0;
        data1_ = 0;
        data2_ = 0;
        time_ = 0;

        transpose = 0;

        pulse_counter_ = 0;

        for (int i = mPOLYPHONY; --i >= 0;)
        {
            free_voices_.unshift(i); // add all voices to voice queue
            voices[i].init(lowest_freq, lowest_freq);
        }
    }

    void begin()
    {
        playing_ = true;
        current_midi_address_ = midi_table_name_;
        pulse_counter_ = 0;
        message_type_ = current_midi_address_[0];
        data1_ = current_midi_address_[1];
        data2_ = current_midi_address_[2];
        time_ = (current_midi_address_[3] << 8) + current_midi_address_[4];
    }

    void stop()
    {
        // send noteoffs to everything
        playing_ = false;
    }

    void setTranspose(int transpose_)
    {
        transpose = transpose_;
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
                    noteOff(data1_ + transpose);
                    break;
                case 0x90: // note on
                    if (data1_ != 127)
                    {
                        noteOn(data1_ + transpose, data2_);
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

    void noteOn(uint16_t note, uint16_t vel)
    {
        if (free_voices_.size() > 0)
        {
            curr_voice_ = free_voices_.shift(); // remove head element and return it!
        }
        else
        {
            uint8_t voice_num = nonfree_voices_.get(0)->voice_num; // voice num of note to turn off
            // voices[voice_num].noteOff();                           // turn note off
            free_voices_.add(voice_num);        // re-add voice to free queue
            delete (nonfree_voices_.get(0));    // delete voice from pressed queue
            nonfree_voices_.remove(0);          // remove freed voice from pressed queue
            curr_voice_ = free_voices_.shift(); // remove head element
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
        for (int i = num_nonfree_voices; --i >= 0;)
        {
            if (nonfree_voices_.get(i)->note_num == note)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                // samples[voice_num].stop();
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

        for (int i = mPOLYPHONY; --i >= 0;)
        {
            free_voices_.unshift(i); // add all voices to voice queue
            // samples[i].stop();
        }
    }

    void setLoopGain(float loop_gain)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setLoopGain(loop_gain);
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            out_sample += voices[i].next();
        }
        return out_sample;
    }

    // CLASS VARIABLES

    uint8_t *current_midi_address_;

    mString<int32_t> voices[mPOLYPHONY];
    LinkedList<int16_t> free_voices_;
    LinkedList<MeapNoteAndVoice *> nonfree_voices_;

    uint16_t curr_voice_;
    int8_t shift_val_;

    int transpose;

    float default_freq_;
    float m_freq_table[128];

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

#endif // mSTRING_POLY_H_
