#ifndef MSUBSYNTHPOLY_H_
#define MSUBSYNTHPOLY_H_

#include <tables/saw2048_int8.h> // table for Oscils to play
#include <dependencies/LinkedList/LinkedList.h>

template <uint32_t mNUM_CELLS = SAW2048_NUM_CELLS, uint32_t mNUM_OSC = 1, uint16_t mPOLYPHONY = 4, class T = int8_t>
class mSubSynthPoly
{
public:
    mSubSynthPoly(const int8_t *TABLE_NAME, uint8_t *midi_table_name)
    {
        init(TABLE_NAME, midi_table_name);
    };

    mSubSynthPoly(const int8_t *TABLE_NAME)
    {
        init(TABLE_NAME, NULL);
    };

    mSubSynthPoly()
    {
        init(SAW2048_DATA, NULL);
    }

    void init(const int8_t *TABLE_NAME, uint8_t *midi_table_name)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].init(TABLE_NAME);
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

    void flush()
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
            free_voices_.unshift(i); // add all voices to voice queue
        }
        nonfree_voices_.clear();
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
        return;
    }

    void setOscGain(uint16_t osc_num, uint16_t osc_gain)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setOscGain(osc_num, osc_gain);
        }
    }

    void setOscSemitones(uint16_t osc_num, uint16_t semitone_offset)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setOscSemitones(osc_num, semitone_offset);
        }
    }

    void setOscDetune(uint16_t osc_num, float amount)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setOscDetune(osc_num, amount);
        }
    }

    // 0 - 255
    void setCutoff(uint16_t cutoff_val)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setCutoff(cutoff_val);
        }
    }

    // 0 - 255
    void setResonance(uint16_t resonance_val)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setResonance(resonance_val);
        }
    }

    void setAttackTime(uint32_t a_)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setAttackTime(a_);
        }
    }

    void setDecayTime(uint32_t d_)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setDecayTime(d_);
        }
    }

    void setSustainLevel(uint32_t s_)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setSustainLevel(s_);
        }
    }

    void setReleaseTime(uint32_t r_)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setReleaseTime(r_);
        }
    }

    void setNoiseGain(uint32_t noise_gain)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setNoiseGain(noise_gain);
        }
    }

    void setFilterEnvAmount(uint32_t amount)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setFilterEnvAmount(amount);
        }
    }

    void setFilterAttackTime(uint32_t time)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setFilterAttackTime(time);
        }
    }

    void setFilterReleaseTime(uint32_t time)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setFilterReleaseTime(time);
        }
    }

    void setFilterDecayTime(uint32_t time)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setFilterDecayTime(time);
        }
    }

    void setFilterSustainLevel(uint32_t level)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setFilterSustainLevel(level);
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

    mSubSynthVoice<mNUM_CELLS, mNUM_OSC, T> voices[mPOLYPHONY];

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

#endif // MSUBSYNTHPOLY_H_
