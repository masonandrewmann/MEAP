#ifndef MROMPLER_H_
#define MROMPLER_H_

#include <dependencies/LinkedList/LinkedList.h>

struct RomplerNote
{
    uint16_t note_num;
    uint16_t voice_num;
    uint16_t program_num;
};

template <uint64_t mMAX_SAMPLE_LENGTH, uint8_t mPOLYPHONY, class T = int8_t>
class mRompler
{
public:
    mRompler() {
    };

    void init(const T **sample_list, uint64_t *sample_lengths)
    {
        sample_list_ = sample_list;
        sample_lengths_ = sample_lengths;

        default_freq_ = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;
        curr_program_ = 0;

        attack_time_ = 1;
        decay_time_ = 1;
        release_time_ = 100;
        sustain_level_ = 255;

        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            velocity_[i] = 127;
            sample_bank_[i].setTable(sample_list_[curr_program_], sample_lengths[curr_program_]);
            free_voices_.unshift(i); // add all voices to voice queue
            sample_bank_[i].setAttackTime(attack_time_);
            sample_bank_[i].setReleaseTime(release_time_);
        }

        for (uint16_t i = 0; i < 127; i++)
        {

            sample_frequencies[i] = default_freq_ * pow(2.f, ((float)(i - 60)) / 12.f);
        }
    }

    void setAttackTime(uint32_t at)
    {
        attack_time_ = at;
    }

    void setReleaseTime(uint32_t rt)
    {
        release_time_ = rt;
    }

    void setADSR(uint32_t at, uint32_t dt, uint32_t sl, uint32_t rt)
    {
        attack_time_ = at;
        decay_time_ = dt;
        sustain_level_ = sl;
        release_time_ = rt;
    }

    void setLoopingOn()
    {
        for (uint8_t j = 0; j < mPOLYPHONY; j++)
        {
            sample_bank_[j].setLoopingOn();
        }
    }

    void setLoopingOff()
    {
        for (uint8_t j = 0; j < mPOLYPHONY; j++)
        {
            sample_bank_[j].setLoopingOff();
        }
    }

    void noteOn(uint16_t note, float vel)
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
        sample_bank_[curr_voice_].setTableAndEnd(sample_list_[curr_program_], sample_lengths_[curr_program_]);
        sample_bank_[curr_voice_].setTimes(attack_time_, decay_time_, 4294967295, release_time_);
        sample_bank_[curr_voice_].setADLevels(255, sustain_level_);

        sample_bank_[curr_voice_].noteOn(sample_frequencies[note], vel);

        // store note in pressed notes queue
        RomplerNote *my_note = new RomplerNote;
        my_note->note_num = note;
        my_note->voice_num = curr_voice_;
        my_note->program_num = curr_program_;
        nonfree_voices_.add(my_note);
    }

    void noteOn(uint16_t note, float vel, int16_t program_override)
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
        sample_bank_[curr_voice_].setTableAndEnd(sample_list_[program_override], sample_lengths_[program_override]);
        sample_bank_[curr_voice_].setTimes(attack_time_, decay_time_, 4294967295, release_time_);
        sample_bank_[curr_voice_].setADLevels(255, sustain_level_);

        sample_bank_[curr_voice_].noteOn(sample_frequencies[note], vel);

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
            if (nonfree_voices_.get(i)->note_num == note && nonfree_voices_.get(i)->program_num == curr_program_)
            {
                uint8_t voice_num = nonfree_voices_.get(i)->voice_num; // voice num of note to turn off
                sample_bank_[voice_num].noteOff();
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
                sample_bank_[voice_num].noteOff();
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
            sample_bank_[i].noteOff();
        }
    }

    void update()
    {
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            sample_bank_[i].update();
        }
    }

    void setProgram(int16_t p)
    {
        curr_program_ = p;
    }

    int32_t next()
    {
        uint32_t output_sample = 0;
        for (int16_t i = mPOLYPHONY; --i >= 0;)
        {
            output_sample += sample_bank_[i].next();
        }
        return output_sample;
    }

protected:
    // sampler voices
    mSampler<mMAX_SAMPLE_LENGTH, T> sample_bank_[mPOLYPHONY];
    const T **sample_list_;
    uint64_t *sample_lengths_;
    uint16_t velocity_[mPOLYPHONY];
    uint16_t curr_program_;
    uint32_t attack_time_;
    uint32_t decay_time_;
    uint32_t release_time_;
    uint32_t sustain_level_;

    float sample_frequencies[127];

    float default_freq_;

    LinkedList<int16_t> free_voices_;
    LinkedList<RomplerNote *> nonfree_voices_;
};

#endif // MROMPLER_H_
