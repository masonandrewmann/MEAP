#ifndef MREALSAMPLERPOLY_H_
#define MREALSAMPLERPOLY_H_

#include <Meap.h>
#include <dependencies/LinkedList/LinkedList.h>

template <uint32_t mNUM_CELLS, uint16_t mPOLYPHONY = 4, class T = int8_t, meap_interpolation INTERP = mINTERP_NONE>

class mRealSamplerPoly
{
public:
    mRealSamplerPoly(const T *TABLE_NAME, uint8_t *midi_table_name)
    {
        init(TABLE_NAME, midi_table_name);
    };

    mRealSamplerPoly(const T *TABLE_NAME)
    {
        init(TABLE_NAME, NULL);
    };

    mRealSamplerPoly() {
    };

    void init(const T *TABLE_NAME, uint8_t *midi_table_name)
    {

        default_freq_ = (float)AUDIO_RATE / (float)mNUM_CELLS;
        for (int i = 0; i < 128; i++)
        {
            m_freq_table[i] = default_freq_ * (float)pow(2.f, ((float)(60 - i) * -1.0) / 12.f);
        }

        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].setTable(TABLE_NAME, mNUM_CELLS);
            // set up voices with table
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
        for (int16_t i = mPOLYPHONY; --i >= 0;)
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
            // steal a voice
            uint8_t voice_num = nonfree_voices_.get(0)->voice_num; // voice num of note to turn off
            voices[voice_num].noteOff();                           // turn note off
            free_voices_.add(voice_num);                           // re-add voice to free queue
            delete (nonfree_voices_.get(0));                       // delete voice from pressed queue
            nonfree_voices_.remove(0);                             // remove freed voice from pressed queue
            curr_voice_ = free_voices_.shift();                    // remove head element
        }

        voices[curr_voice_].noteOn(m_freq_table[note], vel);

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

    void setADSR(uint32_t attack, uint32_t decay, uint32_t sustain, uint32_t release)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setTimes(attack, decay, 4294967295, release);
            voices[i].setADLevels(255, sustain);
        }
    }

    void update()
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].update();
        }
    }

    int32_t next()
    {
        uint32_t output_sample = 0;
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            output_sample += voices[i].next();
        }
        return output_sample;
    }

    mSampler<mNUM_CELLS, T, INTERP> voices[mPOLYPHONY];

    // CLASS VARIABLES
    LinkedList<int16_t> free_voices_;
    LinkedList<MeapNoteAndVoice *> nonfree_voices_;

    uint16_t curr_voice_;

    // reading midi file
    uint8_t *midi_table_name_;
    uint8_t *current_midi_address_;

    uint64_t pulse_counter_;

    bool playing_;

    // last midi message
    uint8_t message_type_;
    uint8_t data1_;
    uint8_t data2_;
    uint16_t time_;

    float default_freq_;
    float m_freq_table[128];
};
#endif // MREALSAMPLERPOLY_H_