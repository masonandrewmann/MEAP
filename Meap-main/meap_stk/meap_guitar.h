#ifndef MEAP_GUITAR_H
#define MEAP_GUITAR_H

#include "meap_twang.h"
#include "meap_one_pole.h"
#include "meap_one_zero.h"
#include "meap_noise.h"

/***************************************************/
/*! \class stk::Guitar
    \brief STK guitar model class.

    This class implements a guitar model with an arbitrary number of
    strings (specified during instantiation).  Each string is
    represented by an stk::Twang object.  The model supports commuted
    synthesis, as discussed by Smith and Karjalainen.  It also includes
    a basic body coupling model and supports feedback.

    This class does not attempt voice management.  Rather, most
    functions support a parameter to specify a particular string
    number and string (voice) management is assumed to occur
    externally.  Note that this class does not inherit from
    stk::Instrmnt because of API inconsistencies.

    This is a digital waveguide model, making its use possibly subject
    to patents held by Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Bridge Coupling Gain = 2
       - Pluck Position = 4
       - Loop Gain = 11
       - Coupling Filter Pole = 1
       - Pick Filter Pole = 128

    by Gary P. Scavone, 2012.
*/
/***************************************************/

#define BASE_COUPLING_GAIN 0.01f
template <int16_t NUM_STRINGS>
class MEAP_Guitar
{
public:
    //! Class constructor, specifying an arbitrary number of strings (default = 6).
    MEAP_Guitar() : couplingFilter_(0.9), pickFilter_(0.95)
    {
        num_strings_ = NUM_STRINGS;

        stringState_ = (int16_t *)malloc(num_strings_ * sizeof(int16_t));
        decayCounter_ = (uint16_t *)malloc(num_strings_ * sizeof(uint16_t));
        filePointer_ = (uint32_t *)malloc(num_strings_ * sizeof(uint32_t));

        pluckGains_ = (float *)malloc(num_strings_ * sizeof(float));

        excitation_ = (int16_t *)malloc(200 * sizeof(int16_t));

        couplingGain_ = BASE_COUPLING_GAIN;

        // couplingFilter_.setPole(0.9);
        // pickFilter_.setPole(0.95);

        couplingFilter_.clear();
        // pickFilter_.clear(); // CANNOT FIGURE OUT WHY THIS CLEAR BREAKS EVERYTHING IT MAKES EXCITATION SIGNAL REALLY QUIET. the actual line that does it is setting outputs[1] to 0 in the onepole clear function, which for some reason affects the inputs,,, makes no sense! im leaving it like this for now because it does work fine without clearing the filter

        this->clear();
        this->setBodyFile(); // filtered noise excitation
    };

    ~MEAP_Guitar()
    {
        free(stringState_);
        free(decayCounter_);
        free(filePointer_);
        free(pluckGains_);
        free(excitation_);
    }

    //! Reset and clear all internal state.
    void clear(void)
    {
        for (unsigned int i = 0; i < num_strings_; i++)
        {
            strings_[i].clear();
            stringState_[i] = 0;
            filePointer_[i] = 0;
        }
    };

    void pickFilterClear()
    {
        pickFilter_.clear();
    }

    void printExcitation()
    {
        for (int i = 0; i < 200; i++)
        {
            Serial.println(excitation_[i]);
        }
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

        for (uint16_t i; i < M; i++)
        {
            Serial.println(excitation_[i]);
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
       If the \c string argument is < 0, the pluck position is set
       for all strings. Position between 0 and 1
    */
    void setPluckPosition(float position, int16_t string = -1)
    {

        if (string < 0) // set all strings
            for (uint16_t i = 0; i < num_strings_; i++)
                strings_[i].setPluckPosition(position);
        else
            strings_[string].setPluckPosition(position);
    };

    //! Set the loop gain for one or all strings.
    /*!
       If the \c string argument is < 0, the loop gain is set for all
       strings. Gain between 0 and 1
    */
    void setLoopGain(float gain, int16_t string = -1)
    {
        if (string < 0) // set all strings
            for (uint16_t i = 0; i < num_strings_; i++)
                strings_[i].setLoopGain(gain);
        else
            strings_[string].setLoopGain(gain);
    }

    //! Set instrument parameters for a particular frequency.
    void setFrequency(float frequency, uint16_t string = 0)
    {

        strings_[string].setFrequency(frequency);
    }

    //! Start a note with the given frequency and amplitude.
    /*!
       If the \c amplitude parameter is less than 0.2, the string will
       be undamped but it will not be "plucked." Amp between 0 and 1
     */
    void noteOn(float frequency, float amplitude, uint16_t string = 0)
    {
        this->setFrequency(frequency, string);
        stringState_[string] = 2;
        filePointer_[string] = 0;
        strings_[string].setLoopGain(0.995);
        pluckGains_[string] = amplitude;
    }

    //! Stop a note with the given amplitude (speed of decay, between 0 and 1).
    void noteOff(float amplitude, uint16_t string = 0)
    {
        strings_[string].setLoopGain((1.0f - amplitude) * 0.9f);
        stringState_[string] = 1;
    };

    //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
    /*!
       If the \c string argument is < 0, then the control change is
       applied to all strings (if appropriate).
    */
    void controlChange(int16_t number, float value, int16_t string = -1)
    {
        float normalizedValue = value * 0.0078125; // * 1/128 to normalize
        if (number == 2)
            couplingGain_ = 1.5f * BASE_COUPLING_GAIN * normalizedValue;
        else if (number == 4) // __SK_PickPosition_
            this->setPluckPosition(normalizedValue, string);
        else if (number == 11) // __SK_StringDamping_
            this->setLoopGain(0.97f + (normalizedValue * 0.03f), string);
        else if (number == 1) // __SK_ModWheel_
            couplingFilter_.setPole(0.98f * normalizedValue);
        else if (number == 128) // __SK_AfterTouch_Cont_
            pickFilter_.setPole(0.95f * normalizedValue);
    };

    //! Return the last computed output value.
    int32_t lastOut(void) { return last_output_; };

    //! Take an optional input sample and compute one output sample.
    int32_t next(float input = 0.0)
    {
        int32_t temp, output = 0.0;
        last_output_ /= num_strings_; // evenly spread coupling across strings
        for (uint16_t i = 0; i < num_strings_; i++)
        {
            if (stringState_[i])
            {
                temp = input;
                // If pluckGain < 0.2, let string ring but don't pluck it.
                if (filePointer_[i] < excitation_length_ && pluckGains_[i] > 0.2f)
                    temp += pluckGains_[i] * excitation_[filePointer_[i]++];
                temp += couplingGain_ * couplingFilter_.next(last_output_); // bridge coupling
                output += strings_[i].next(temp);
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
        }

        return last_output_ = output;
    };

protected:
    // std::vector<stk::Twang> strings_;

    // MEAP_Twang<int16_t> *strings_;
    // MySample<MAX_SAMPLE_LENGTH, AUDIO_RATE> sample[POLYPHONY];
    MEAP_Twang<int16_t> strings_[NUM_STRINGS];

    int16_t *stringState_; // 0 = off, 1 = decaying, 2 = on
    uint16_t *decayCounter_;
    float *pluckGains_;
    uint32_t *filePointer_;

    uint16_t num_strings_;
    uint16_t excitation_length_;

    MEAP_OnePole<int16_t> pickFilter_;
    MEAP_OnePole<int16_t> couplingFilter_;
    float couplingGain_;
    int16_t *excitation_;
    int32_t last_output_;
};

#endif // MEAP_GUITAR_H
