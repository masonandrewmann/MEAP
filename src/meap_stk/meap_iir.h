#ifndef MEAP_IIR_H
#define MEAP_IIR_H

/***************************************************/
/*! \class stk::Iir
    \brief STK general infinite impulse response filter class.

    This class provides a generic digital filter structure that can be
    used to implement IIR filters.  For filters containing only
    feedforward terms, the Fir class is slightly more efficient.

    In particular, this class implements the standard difference
    equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients are normalized
    by a[0].

    The \e gain parameter is applied at the filter input and does not
    affect the coefficient values.  The default gain value is 1.0.
    This structure results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/
template <class T = int16_t>
class MEAP_Iir
{
public:
    //! Default constructor creates a zero-order pass-through "filter".
    MEAP_Iir(void)
    {
        gain_ = 1.0;

        // The default constructor should setup for pass-through.
        b_ = (float *)calloc(1, sizeof(float));
        a_ = (float *)calloc(1, sizeof(float));

        b_[0] = 1.0;
        a_[0] = 1.0;

        inputs_ = (T *)calloc(1, sizeof(T));
        outputs_ = (T *)calloc(1, sizeof(T));
    };

    //! Class destructor.
    ~MEAP_Iir(void);

    //! Set filter coefficients.
    /*!
      An StkError can be thrown if either of the coefficient vector
      sizes is zero, or if the a[0] coefficient is equal to zero.  If
      a[0] is not equal to 1, the filter coeffcients are normalized by
      a[0].  The internal state of the filter is not cleared unless the
      \e clearState flag is \c true.
    */
    void setCoefficients(float *bCoefficients, uint16_t num_bCoefficients, float *aCoefficients, uint16_t num_aCoefficients, bool clearState = false)
    {

        this->setNumerator(bCoefficients, num_bCoefficients, false);
        this->setDenominator(aCoefficients, num_aCoefficients, false);

        if (clearState)
            this->clear();
    };

    //! Set numerator coefficients.
    /*!
      An StkError can be thrown if coefficient vector is empty.  Any
      previously set denominator coefficients are left unaffected.  Note
      that the default constructor sets the single denominator
      coefficient a[0] to 1.0.  The internal state of the filter is not
      cleared unless the \e clearState flag is \c true.
    */
    void setNumerator(float *bCoefficients, uint16_t num_bCoefficients, bool clearState = false)
    {
        if (b_size_ != num_bCoefficients)
        {
            b_size_ = num_bCoefficients;
            b_ = (float *)realloc(b_, b_size_ * sizeof(float));
            inputs_ = (T *)realloc(inputs_, b_size_ * sizeof(T));
        }
        memcpy(b_, bCoefficients, b_size_ * sizeof(float));

        if (clearState)
            this->clear();
    };

    //! Set denominator coefficients.
    /*!
      An StkError can be thrown if the coefficient vector is empty or
      if the a[0] coefficient is equal to zero.  Previously set
      numerator coefficients are unaffected unless a[0] is not equal to
      1, in which case all coeffcients are normalized by a[0].  Note
      that the default constructor sets the single numerator coefficient
      b[0] to 1.0.  The internal state of the filter is not cleared
      unless the \e clearState flag is \c true. size must be greater than 0 and a[0] cannot be 0
    */
    void setDenominator(float *aCoefficients, uint16_t num_aCoefficients, bool clearState = false)
    {

        if (a_size_ != num_aCoefficients)
        {
            a_size_ = num_aCoefficients;
            a_ = (float *)realloc(a_, a_size_ * sizeof(float));
            outputs_ = (T *)realloc(inputs_, a_size_ * sizeof(T));
        }
        memcpy(a_, aCoefficients, a_size_ * sizeof(float));

        if (clearState)
            this->clear();

        // Scale coefficients by a[0] if necessary
        if (a_[0] != 1.0f)
        {
            uint16_t i;
            for (i = 0; i < b_size_; i++)
                b_[i] /= a_[0];
            for (i = 1; i < a_size_; i++)
                a_[i] /= a_[0];
        }
    };

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    void clear(void)
    {
        uint16_t i;
        for (i = 0; i < b_size_; i++)
        {
            inputs_[i] = 0;
        }

        for (i = 0; i < a_size_; i++)
        {
            outputs_[i] = 0;
        }

        last_output_ = 0;
    };

    //! Input one sample to the filter and return one output.
    float next(float input)
    {
        size_t i;

        outputs_[0] = 0.0;
        inputs_[0] = gain_ * input;
        for (i = b_size_ - 1; i > 0; i--)
        {
            outputs_[0] += b_[i] * inputs_[i];
            inputs_[i] = inputs_[i - 1];
        }
        outputs_[0] += b_[0] * inputs_[0];

        for (i = a_size_ - 1; i > 0; i--)
        {
            outputs_[0] += -a_[i] * outputs_[i];
            outputs_[i] = outputs_[i - 1];
        }

        last_output_ = outputs_[0];
        return last_output_;
    };

protected:
    T *inputs_;
    T *outputs_;
    T last_output_;

    float *a_;
    float *b_;

    uint16_t a_size_;
    uint16_t b_size_;

    float gain_;
};

#endif // MEAP_IIR_H
