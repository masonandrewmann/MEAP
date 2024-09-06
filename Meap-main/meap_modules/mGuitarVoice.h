#ifndef MGUITARVOICE_H_
#define MGUITARVOICE_H_

#include "meap_twang.h"
#include "meap_one_pole.h"
#include "meap_one_zero.h"
#include "meap_noise.h"

template <class T = int16_t>
class mGuitarVoice
{
public:
    mGuitarVoice()
    {
        init();
    };

    void init()
    {
        excitation_ = (int16_t *)malloc(200 * sizeof(int16_t));
        sustain_ = 0.5;
    }

    ~mGuitarVoice()
    {
        free(excitation_);
    }

    //! Reset and clear all internal state.
    void clear(void)
    {

        string_.clear();
        string_state_ = 0;
        file_pointer_ = 0;
    };

    void pickFilterClear()
    {
        pick_filter_.clear();
    }

    //! Set the string excitation, using either a soundfile or computed noise.
    /*!
       If no argument is provided, the std::string is empty, or an error
       occurs reading the file data, an enveloped noise signal will be
       generated for use as the pluck excitation.
     */
    void setBodyFile()
    {

        uint16_t M = 200; // arbitrary value
        excitation_length_ = M;

        MEAP_Noise noise;

        for (uint16_t i; i < M; i++)
        {
            excitation_[i] = noise.next();
        }
        // Smooth the start and end of the noise.
        uint16_t N = (uint16_t)M * 0.2; // arbitrary value
        for (uint16_t n = 0; n < N; n++)
        {
            float weight = 0.5f * (1.0f - cos(n * 3.14159265358979 / (N - 1)));
            excitation_[n] *= weight;
            excitation_[M - n - 1] *= weight;
        }

        for (uint16_t i; i < M; i++)
        {
            excitation_[i] = pickFilter_.next(excitation_[i]);
        }

        // Compute file mean and remove (to avoid DC bias).
        float mean = 0.0;
        for (uint16_t i = 0; i < M; i++)
            mean += excitation_[i];
        mean /= M;

        for (uint16_t i = 0; i < M; i++)
            excitation_[i] -= mean;

        // Reset all the file pointers.
        for (uint16_t i = 0; i < num_strings_; i++)
            filePointer_[i] = 0;
    }

    //! Set the pluck position for one or all strings.
    /*!
        Position between 0 and 1
    */
    void setPluckPosition(float position)
    {
        string_.setPluckPosition(position);
    };

    //! Set the loop gain for one or all strings.
    /*!
        Gain between 0 and 1
    */
    void setLoopGain(float gain)
    {
        string_.setLoopGain(gain);
    }

    //! Set instrument parameters for a particular frequency.
    void setFrequency(float frequency)
    {

        string_.setFrequency(frequency);
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        string_.setFrequency(mtof(note));
        stringState_ = 2;
        filePointer_ = 0;
        strings_.setLoopGain(0.995);
        pluck_gain_ = ((float)vel) / 127.0;
    }

    void noteOff()
    {
        string_.setLoopGain((1.0f - sustain_) * 0.9f);
        string_state = 1;
    }

    // to be called in audio loop
    int32_t next()
    {
        int32_t output = 0;
        // last_output_ /= num_strings_; // evenly spread coupling across strings
        if (string_state_)
        {
            // If pluckGain < 0.2, let string ring but don't pluck it.
            if (file_pointer_ < excitation_length_ && gain_ > 0.2f)
                output += gain_ * excitation_[file_pointer_++];
            output += coupling_gain_ * coupling_filter_.next(last_output_); // bridge coupling
            output += string_.next(temp);
            // Check if string energy has decayed sufficiently to turn it off.
            if (stringState_[i] == 1)
            {
                if (fabs(strings_[i].lastOut()) < 0.001f)
                    decayCounter_[i]++;
                else
                    decayCounter_[i] = 0;
                if (decayCounter_[i] > (uint16_t)floor(0.1f * AUDIO_RATE))
                {
                    stringState_[i] = 0;
                    decayCounter_[i] = 0;
                }
            }
        }

        return last_output_ = output;
    }

protected:
    MEAP_Twang<T> string_;

    int16_t string_state_;
    uint16_t decay_counter_;
    float gain_;
    float sustain_;

    uint32_t file_pointer_;

    MEAP_OnePole<T> pickFilter_;
    MEAP_OnePole<T> couplingFilter_;
    float coupling_gain_;
    int32_t last_output_;
    int16_t *excitation_;
    uint16_t excitation_length_;
};

#endif // MGUITARVOICE_H_