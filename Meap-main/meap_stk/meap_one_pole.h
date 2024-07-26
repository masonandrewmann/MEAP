#ifndef STK_ONEPOLE_H
#define STK_ONEPOLE_H

/***************************************************/
/*! \class stk::OnePole
    \brief STK one-pole filter class.

    This class implements a one-pole digital filter.  A method is
    provided for setting the pole position along the real axis of the
    z-plane while maintaining a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_OnePole
{
public:
    //! The default constructor creates a low-pass filter (pole at z = 0.9).
    MEAP_OnePole(float thePole = 0.9)
    {
        gain_ = 1.0;
        b_ = (float *)malloc(sizeof(float));
        a_ = (float *)malloc(2 * sizeof(float));

        a_[0] = 1.0;

        outputs_ = (T *)malloc(2 * sizeof(T));
        inputs_ = (T *)malloc(sizeof(T));

        this->setPole(thePole);
        // this->clear();
    };

    //! Class destructor.
    ~MEAP_OnePole()
    {
        free(a_);
        free(b_);
        free(inputs_);
        free(outputs_);
    };

    //! Set the b[0] coefficient value.
    void setB0(float b0) { b_[0] = b0; };

    //! Set the a[1] coefficient value. a1 should be less than 1
    void setA1(float a1) { a_[1] = a1; };

    //! Set all filter coefficients. a1 should be less than 1
    void setCoefficients(float b0, float a1, bool clearState = false)
    {
        b_[0] = b0;
        a_[1] = a1;

        if (clearState)
            this->clear();
    };

    //! Set the pole position in the z-plane. thePole should be less than 1.0
    /*!
      This method sets the pole position along the real-axis of the
      z-plane and normalizes the coefficients for a maximum gain of one.
      A positive pole value produces a low-pass filter, while a negative
      pole value produces a high-pass filter.  This method does not
      affect the filter \e gain value. The argument magnitude should be
      less than one to maintain filter stability.
    */
    void setPole(float thePole)
    {
        // Normalize coefficients for peak unity gain.
        if (thePole > 0.0f)
            b_[0] = (float)(1.0f - thePole);
        else
            b_[0] = (float)(1.0f + thePole);

        a_[1] = -thePole;
    };

    //! Set the filter gain.
    /*!
      The gain is applied at the filter input and does not affect the
      coefficient values.  The default gain value is 1.0.
     */
    void setGain(float gain) { gain_ = gain; };

    //! Return the current filter gain.
    float getGain(void) { return gain_; };

    void clear(void)
    {
        inputs_[0] = 0;
        outputs_[0] = 0;
        outputs_[1] = 0;
        last_output_ = 0;
    };

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    //! Input one sample to the filter and return one output.
    T next(T input)
    {
        inputs_[0] = gain_ * input;
        // Serial.println(inputs_[0]);
        last_output_ = b_[0] * inputs_[0] - a_[1] * outputs_[1];
        outputs_[1] = last_output_;

        return last_output_;
    };

protected:
    float *b_;
    float *a_;
    float gain_;
    T last_output_;
    T *inputs_;
    T *outputs_;
};

#endif // STK_ONEPOLE_H
