#ifndef MROMPLER_H_
#define MROMPLER_H_

#include <dependencies/LinkedList/LinkedList.h>

struct RomplerNote
{
    uint16_t note_num;
    uint16_t voice_num;
    uint16_t program_num;
};

template <uint64_t mMAX_SAMPLE_LENGTH, uint8_t mPOLYPHONY, class T = int8_t, meap_interpolation INTERP = mINTERP_NONE>
class mRompler
{
public:

    mRompler(const T **sample_list_, uint64_t *sample_lengths_)
    {
        sample_list = sample_list_;
        sample_lengths = sample_lengths_;

        curr_program = 0;

        attack_time = 1;
        decay_time = 1;
        sustain_level = 255;
        release_time = 100;

        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank[i].setTable(sample_list[curr_program], sample_lengths[curr_program]);
            free_voices_.unshift(i); // add all voices to voice queue
            sample_bank[i].setAttackTime(attack_time);
            sample_bank[i].setReleaseTime(release_time);
        }

        for (uint16_t i = 0; i < 127; i++)
        {

            sample_frequencies[i] = sample_bank[0].default_freq_ * pow(2.f, ((float)(i - 60)) / 12.f);
        }
    };

    void setAttackTime(uint32_t attack_time_)
    {
        attack_time = attack_time_;
    }

    void setReleaseTime(uint32_t release_time_)
    {
        release_time = release_time_;
    }

    void setEnd(int16_t program_num, uint64_t end_val)
    {
        sample_lengths[program_num] = end_val;
    }

    void setADSR(uint32_t attack_time_, uint32_t decay_time_, uint32_t sustain_level_, uint32_t release_time_)
    {
        attack_time = attack_time_;
        decay_time = decay_time_;
        sustain_level = sustain_level_;
        release_time = release_time_;
    }

    void setLoopingOn()
    {
        for (uint8_t j = 0; j < mPOLYPHONY; j++)
        {
            sample_bank[j].setLoopingOn();
        }
    }

    void setLoopingOff()
    {
        for (uint8_t j = 0; j < mPOLYPHONY; j++)
        {
            sample_bank[j].setLoopingOff();
        }
    }

    void setReverseOn()
    {
        for (uint8_t j = 0; j < mPOLYPHONY; j++)
        {
            sample_bank[j].setReverseOn();
        }
    }

    void setReverseOff()
    {
        for (uint8_t j = 0; j < mPOLYPHONY; j++)
        {
            sample_bank[j].setReverseOff();
        }
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        int16_t curr_voice_;
        if (free_voices_.size() > 0)
        {
            curr_voice_ = free_voices_.shift(); // remove head element and return it!
        }
        else
        {
            return; // no free voices, move along...
        }
        sample_bank[curr_voice_].setTableAndEnd(sample_list[curr_program], sample_lengths[curr_program]);
        sample_bank[curr_voice_].setTimes(attack_time, decay_time, 4294967295, release_time);
        sample_bank[curr_voice_].setADLevels(255, sustain_level);

        sample_bank[curr_voice_].noteOn(sample_frequencies[note], vel);

        // store note in pressed notes queue
        RomplerNote *my_note = new RomplerNote;
        my_note->note_num = note;
        my_note->voice_num = curr_voice_;
        my_note->program_num = curr_program;
        nonfree_voices_.add(my_note);
    }

    void noteOn(uint16_t note, uint16_t vel, int16_t program_override)
    {
        int16_t curr_voice_;
        if (free_voices_.size() > 0)
        {
            curr_voice_ = free_voices_.shift(); // remove head element and return it!
        }
        else
        {
            return; // no free voices, move along...
        }
        sample_bank[curr_voice_].setTableAndEnd(sample_list[program_override], sample_lengths[program_override]);
        sample_bank[curr_voice_].setTimes(attack_time, decay_time, 4294967295, release_time);
        sample_bank[curr_voice_].setADLevels(255, sustain_level);

        sample_bank[curr_voice_].noteOn(sample_frequencies[note], vel);

        // store note in pressed notes queue
        RomplerNote *my_note = new RomplerNote;
        my_note->note_num = note;
        my_note->voice_num = curr_voice_;
        my_note->program_num = program_override;
        nonfree_voices_.add(my_note);
    }

    void noteOff(uint16_t note)
    {

        uint8_t num_nonfree_voices = nonfree_voices_.size();
        for (uint8_t i = 0; i < num_nonfree_voices; i++)
        {
            if (nonfree_voices_.get(i)->note_num == note && nonfree_voices_.get(i)->program_num == curr_program)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                sample_bank[voice_num].noteOff();
                free_voices_.add(voice_num);     // re-add voice to free queue
                delete (nonfree_voices_.get(i)); // delete voice from pressed queue
                nonfree_voices_.remove(i);       // remove freed voice from pressed queue
                return;
            }
        }
    }

    void noteOff(uint16_t note, uint16_t program_override)
    {

        uint8_t num_nonfree_voices = nonfree_voices_.size();
        for (uint8_t i = 0; i < num_nonfree_voices; i++)
        {
            if (nonfree_voices_.get(i)->note_num == note && nonfree_voices_.get(i)->program_num == program_override)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                sample_bank[voice_num].noteOff();
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
            sample_bank[i].noteOff();
        }
    }

    void setProgram(int16_t p)
    {
        curr_program = p;
    }

    void setStart(uint64_t start)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank[i].setStart(start);
        }
    }

    void setEnd(uint64_t end)
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank[i].setEnd(end);
        }
    }

    void update()
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank[i].update();
        }
    }

    int32_t next()
    {
        uint32_t output_sample = 0;
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            output_sample += sample_bank[i].next();
        }
        return output_sample;
    }

    // CLASS VARIABLES

    mSampler<mMAX_SAMPLE_LENGTH, T, INTERP> sample_bank[mPOLYPHONY];
    const T **sample_list;
    uint64_t *sample_lengths;
    uint16_t curr_program;
    uint32_t attack_time;
    uint32_t decay_time;
    uint32_t release_time;
    uint32_t sustain_level;

    float sample_frequencies[127];

    LinkedList<int16_t> free_voices_;
    LinkedList<RomplerNote *> nonfree_voices_;
};

#endif // MROMPLER_H_
