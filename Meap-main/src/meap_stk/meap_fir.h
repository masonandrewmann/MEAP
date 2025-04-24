#ifndef MEAP_FIR_H
#define MEAP_FIR_H

/***************************************************/
/*! \class stk::Fir
    \brief STK general finite impulse response filter class.

    This class provides a generic digital filter structure that can be
    used to implement FIR filters.  For filters with feedback terms,
    the Iir class should be used.

    In particular, this class implements the standard difference
    equation:

    y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb]

    The \e gain parameter is applied at the filter input and does not
    affect the coefficient values.  The default gain value is 1.0.
    This structure results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_fir
{
public:
    MEAP_fir()
    {
        // want to add a default initialization here, call setCoefficients
        num_coefficients_ = -2; // dummy length
        gain_ = 1.0;
        // this->clear();
    };

    //! Class destructor.
    ~MEAP_fir(void)
    {
        free(b_);
        free(inputs_);
    };

    void setGain(float gain) { gain_ = gain; };

    //! Return the current filter gain.
    float getGain(void) { return gain_; };

    //! Set filter coefficients.
    /*!
      An StkError can be thrown if the coefficient vector size is
      zero.  The internal state of the filter is not cleared unless the
      \e clearState flag is \c true.
    */
    void setCoefficients(float coefficients[], int num_coefficients, bool clearState = false)
    {

        if (num_coefficients_ != num_coefficients)
        {
            num_coefficients_ = num_coefficients;
            b_ = (float *)realloc(b_, num_coefficients * sizeof(float));
            inputs_ = (T *)realloc(inputs_, num_coefficients * sizeof(T));
        }
        memcpy(b_, coefficients, num_coefficients * sizeof(float));
        if (clearState)
        {
            this->clear();
        }
    };

    void clear(void)
    {
        uint16_t i;
        for (i = 0; i < num_coefficients_; i++)
        {
            inputs_[i] = 0;
        }
        last_output_ = 0;
    };

    //! Return the last computed output value.
    T lastOut(void) { return last_output_; };

    //! Input one sample to the filter and return one output.
    T next(float input)
    {
        last_output_ = 0;
        inputs_[0] = gain_ * input;

        for (uint16_t i = (num_coefficients_)-1; i > 0; i--)
        {
            last_output_ += b_[i] * inputs_[i];
            inputs_[i] = inputs_[i - 1];
        }
        last_output_ += b_[0] * inputs_[0];

        return last_output_;
    };

    float phaseDelay(float frequency)
    {

        float omegaT = 6.2831853072 * frequency / AUDIO_RATE;
        float real = 0.0, imag = 0.0;
        for (uint32_t i = 0; i < num_coefficients_; i++)
        {
            real += b_[i] * cos(i * omegaT);
            imag -= b_[i] * sin(i * omegaT);
        }
        real *= gain_;
        imag *= gain_;

        float phase = std::atan2(imag, real);

        // real = 0.0, imag = 0.0;
        // for (unsigned int i = 0; i < a_.size(); i++)
        // {
        //     real += a_[i] * cos(i * omegaT);
        //     imag -= a_[i] * sin(i * omegaT);
        // }

        // phase -= atan2(imag, real);
        phase = fmod(-phase, 2 * 3.14159265358979);
        return phase / omegaT;
    }

protected:
    float *b_;
    float *a_;
    float gain_;
    T last_output_;
    T *inputs_;
    uint16_t num_coefficients_;
};

#endif // MEAP_FIR_H
