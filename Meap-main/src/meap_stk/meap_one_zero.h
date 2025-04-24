#ifndef MEAP_ONEZERO_H
#define MEAP_ONEZERO_H

/***************************************************/
/*! \class stk::OneZero
  \brief STK one-zero filter class.

  This class implements a one-zero digital filter.  A method is
  provided for setting the zero position along the real axis of the
  z-plane while maintaining a constant filter gain.

  by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_OneZero
{
public:
    //! The default constructor creates a low-pass filter (zero at z = -1.0).
    MEAP_OneZero(float theZero = -1.0)
    {
        gain_ = 1.0;
        b_ = (float *)malloc(2 * sizeof(float));
        inputs_ = (T *)malloc(2 * sizeof(T));

        this->setZero(theZero);
        // this->clear();
    }

    //! Class destructor.
    ~MEAP_OneZero()
    {
        free(b_);
        free(inputs_);
    }

    //! Set the b[0] coefficient value.
    void setB0(float b0) { b_[0] = b0; };

    //! Set the b[1] coefficient value.
    void setB1(float b1) { b_[1] = b1; };

    //! Set all filter coefficients.
    void setCoefficients(float b0, float b1, bool clearState = false)
    {
        b_[0] = b0;
        b_[1] = b1;

        if (clearState)
            this->clear();
    }

    //! Set the zero position in the z-plane.
    /*!
      This method sets the zero position along the real-axis of the
      z-plane and normalizes the coefficients for a maximum gain of one.
      A positive zero value produces a high-pass filter, while a
      negative zero value produces a low-pass filter.  This method does
      not affect the filter \e gain value.
    */
    void setZero(float theZero)
    {
        // Normalize coefficients for unity gain.
        if (theZero > 0.0f)
            b_[0] = 1.0f / ((float)1.0f + theZero);
        else
            b_[0] = 1.0f / ((float)1.0f - theZero);

        b_[1] = -theZero * b_[0];
    }

    void clear(void)
    {
        inputs_[0] = 0;
        inputs_[1] = 0;

        last_output_ = 0;
    };

    //! Set the filter gain.
    /*!
      The gain is applied at the filter input and does not affect the
      coefficient values.  The default gain value is 1.0.
     */
    void setGain(float gain) { gain_ = gain; };

    //! Return the current filter gain.
    float getGain(void) { return gain_; };

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    //! Input one sample to the filter and return one output.
    float next(float input)
    {
        inputs_[0] = gain_ * input;
        last_output_ = b_[1] * inputs_[1] + b_[0] * inputs_[0];
        inputs_[1] = inputs_[0];

        return last_output_;
    }

    float phaseDelay(float frequency)
    {
        float omegaT = 6.2831853072 * frequency / AUDIO_RATE;
        float real = 0.0, imag = 0.0;
        for (uint16_t i = 0; i < 2; i++)
        {
            real += b_[i] * cos(i * omegaT);
            imag -= b_[i] * sin(i * omegaT);
        }
        real *= gain_;
        imag *= gain_;

        float phase = atan2(imag, real);

        phase = fmod(-phase, 6.2831853072);
        return phase / omegaT;
    }

protected:
    float *b_;
    float *a_;
    float gain_;
    T last_output_;
    T *inputs_;
    T *outputs_;
};

#endif // MEAP_ONEZERO_H
