#ifndef MGUITARPOLY_H_
#define MGUITARPOLY_H_

#include <dependencies/LinkedList/LinkedList.h>

template <uint16_t mPOLYPHONY = 4>
class mGuitarPoly
{
public:
    mGuitarPoly()
    {
        sustain_ = 0.5;

        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices.setPluckPosition(0.5, i);
            voices.setLoopGain(0.5, i);
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

    //! Set the pluck position for one or all strings.
    /*!
        Position between 0 and 1
    */
    void setPluckPosition(float position)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
            voices.setPluckPosition(position, i);
    };

    //! Set the loop gain for one or all strings.
    /*!
    Gain between 0 and 1
    */
    void setLoopGain(float gain)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
            voices.setLoopGain(gain, i);
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
            voices.noteOff(0.5, i);
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
                    noteOff(data1_, sustain_);
                    break;
                case 0x90: // note on
                    if (data1_ != 127)
                    {
                        noteOn(data1_, ((float)data2_) * 0.007874015748); // vel *1/127
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

    void setSustain(float sus)
    {
        sustain_ = sus;
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

        voices.noteOn(mtof(note), ((float)vel) / 127.f, curr_voice_);

        // store note in pressed notes queue
        MeapNoteAndVoice *my_note = new MeapNoteAndVoice;
        my_note->note_num = note;
        my_note->voice_num = curr_voice_;
        nonfree_voices_.add(my_note);
    }

    void noteOff(uint16_t note, float sus)
    {

        uint8_t num_nonfree_voices = nonfree_voices_.size();
        for (uint8_t i = 0; i < num_nonfree_voices; i++)
        {
            if (nonfree_voices_.get(i)->note_num == note)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                voices.noteOff(sus, voice_num);
                free_voices_.add(voice_num);     // re-add voice to free queue
                delete (nonfree_voices_.get(i)); // delete voice from pressed queue
                nonfree_voices_.remove(i);       // remove freed voice from pressed queue
                return;
            }
        }
    }

    void noteOff(uint16_t note)
    {

        uint8_t num_nonfree_voices = nonfree_voices_.size();
        for (uint8_t i = 0; i < num_nonfree_voices; i++)
        {
            if (nonfree_voices_.get(i)->note_num == note)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                voices.noteOff(voice_num, sustain_);
                free_voices_.add(voice_num);     // re-add voice to free queue
                delete (nonfree_voices_.get(i)); // delete voice from pressed queue
                nonfree_voices_.remove(i);       // remove freed voice from pressed queue
                return;
            }
        }
    }

    int32_t next()
    {
        int32_t out_sample = voices.next();

        return out_sample;
    }

    uint8_t *current_midi_address_;

    MEAP_Guitar<mPOLYPHONY> voices;

protected:
    int16_t stringState_[mPOLYPHONY]; // 0 = off, 1 = decaying, 2 = on
    uint16_t decayCounter_[mPOLYPHONY];
    float pluckGains_[mPOLYPHONY];
    // uint32_t *filePointer_;

    LinkedList<int16_t> free_voices_;
    LinkedList<MeapNoteAndVoice *> nonfree_voices_;

    float sustain_;

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

#endif // MGUITARPOLY_H_
