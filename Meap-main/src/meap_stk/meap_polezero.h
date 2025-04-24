 #ifndef MEAP_POLEZERO_H
#define MEAP_POLEZERO_H

/***************************************************/
/*! \class stk::PoleZero
    \brief STK one-pole, one-zero filter class.

    This class implements a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass filter with a given
    coefficient.  Another method is provided to create a DC blocking
    filter.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_PoleZero
{
public:
    //! Default constructor creates a first-order pass-through filter.
    MEAP_PoleZero()
    {
        gain_ = 1.0;
        // Default setting for pass-through.
        b_ = (float *)calloc(2, sizeof(float));
        a_ = (float *)calloc(2, sizeof(float));

        b_[0] = 1.0;
        a_[0] = 1.0;

        inputs_ = (T *)calloc(2, sizeof(T));
        outputs_ = (T *)calloc(2, sizeof(T));
    };

    //! Class destructor.
    ~MEAP_PoleZero(){};

    //! Set the b[0] coefficient value.
    void setB0(float b0) { b_[0] = b0; };

    //! Set the b[1] coefficient value.
    void setB1(float b1) { b_[1] = b1; };

    //! Set the a[1] coefficient value.
    void setA1(float a1) { a_[1] = a1; };

    //! Set all filter coefficients. abs(a1) needs to be less that 1.0
    void setCoefficients(float b0, float b1, float a1, bool clearState = false)
    {
        b_[0] = b0;
        b_[1] = b1;
        a_[1] = a1;

        if (clearState)
            this->clear();
    };

    //! Set the filter for allpass behavior using \e coefficient.
    /*!
      This method uses \e coefficient to create an allpass filter,
      which has unity gain at all frequencies.  Note that the
      \e coefficient magnitude must be less than one to maintain
      filter stability.
    */
    void setAllpass(float coefficient)
    {
        b_[0] = coefficient;
        b_[1] = 1.0;
        a_[0] = 1.0; // just in case
        a_[1] = coefficient;
    };

    //! Create a DC blocking filter with the given pole position in the z-plane.
    /*!
      This method sets the given pole position, together with a zero
      at z=1, to create a DC blocking filter.  The argument magnitude
      should be close to (but less than) one to minimize low-frequency
      attenuation.
    */
    void setBlockZero(float thePole = 0.99)
    {
        b_[0] = 1.0;
        b_[1] = -1.0;
        a_[0] = 1.0; // just in case
        a_[1] = -thePole;
    };

    void clear(void)
    {
        inputs_[0] = 0;
        inputs_[1] = 0;

        outputs_[0] = 0;
        outputs_[1] = 0;

        last_output_ = 0;
    };

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    //! Input one sample to the filter and return one output.
    float next(float input)
    {
        inputs_[0] = gain_ * input;
        last_output_ = b_[0] * inputs_[0] + b_[1] * inputs_[1] - a_[1] * outputs_[1];
        inputs_[1] = inputs_[0];
        outputs_[1] = last_output_;

        return last_output_;
    };

protected:
    float gain_;

    float *a_;
    float *b_;

    T *inputs_;
    T *outputs_;

    T last_output_;
};

#endif // MEAP_POLEZERO_H
