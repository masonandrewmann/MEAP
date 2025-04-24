#ifndef MEAP_BIQUAD_H
#define MEAP_BIQUAD_H

#include "meap_filter.h"

/***************************************************/
/*! \class stk::BiQuad
    \brief STK biquad (two-pole, two-zero) filter class.

    This class implements a two-pole, two-zero digital filter.
    Methods are provided for creating a resonance or notch in the
    frequency response while maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/
template <class T = int16_t>
class MEAP_BiQuad : public MEAP_Filter
{
public:
    //! Default constructor creates a second-order pass-through filter.
    MEAP_BiQuad()
    {
        gain_ = 1;

        b_ = (float *)calloc(3, sizeof(float));
        a_ = (float *)calloc(3, sizeof(float));

        b_[0] = 1.0;
        a_[0] = 1.0;

        inputs_ = (T *)calloc(3, sizeof(T));
        outputs_ = (T *)calloc(3, sizeof(T));
    };

    //! Class destructor.
    ~MEAP_BiQuad()
    {
        free(a_);
        free(b_);
        free(inputs_);
        free(outputs_);
    };

    //! Set all filter coefficients.
    void setCoefficients(float b0, float b1, float b2, float a1, float a2, bool clearState = false)
    {
        b_[0] = b0;
        b_[1] = b1;
        b_[2] = b2;
        a_[1] = a1;
        a_[2] = a2;

        if (clearState)
            this->clear();
    };

    //! Set the b[0] coefficient value.
    void setB0(float b0) { b_[0] = b0; };

    //! Set the b[1] coefficient value.
    void setB1(float b1) { b_[1] = b1; };

    //! Set the b[2] coefficient value.
    void setB2(float b2) { b_[2] = b2; };

    //! Set the a[1] coefficient value.
    void setA1(float a1) { a_[1] = a1; };

    //! Set the a[2] coefficient value.
    void setA2(float a2) { a_[2] = a2; };

    //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
    /*!
      This method determines the filter coefficients corresponding to
      two complex-conjugate poles with the given \e frequency (in Hz)
      and \e radius from the z-plane origin.  If \e normalize is true,
      the filter zeros are placed at z = 1, z = -1, and the coefficients
      are then normalized to produce a constant unity peak gain
      (independent of the filter \e gain parameter).  The resulting
      filter frequency response has a resonance at the given \e
      frequency.  The closer the poles are to the unit-circle (\e radius
      close to one), the narrower the resulting resonance width.
      An unstable filter will result for \e radius >= 1.0.  The
      \e frequency value should be between zero and half the sample rate.
    */
    void setResonance(float frequency, float radius, bool normalize = false)
    {
        a_[2] = radius * radius;
        a_[1] = -2.0f * radius * cos(6.2831853072 * frequency / (float)AUDIO_RATE);

        if (normalize)
        {
            // Use zeros at +- 1 and normalize the filter peak gain.
            b_[0] = 0.5f - 0.5f * a_[2];
            b_[1] = 0.0;
            b_[2] = -b_[0];
        }
    };

    //! Set the filter coefficients for a notch at \e frequency (in Hz).
    /*!
      This method determines the filter coefficients corresponding to
      two complex-conjugate zeros with the given \e frequency (in Hz)
      and \e radius from the z-plane origin.  No filter normalization is
      attempted.  The \e frequency value should be between zero and half
      the sample rate.  The \e radius value should be positive.
    */
    void setNotch(float frequency, float radius)
    {
        // This method does not attempt to normalize the filter gain.
        b_[2] = radius * radius;
        b_[1] = (float)-2.0 * radius * std::cos(6.2831853072 * (float)frequency / (float)AUDIO_RATE);
    };

    //! Sets the filter zeroes for equal resonance gain.
    /*!
      When using the filter as a resonator, zeroes places at z = 1, z
      = -1 will result in a constant gain at resonance of 1 / (1 - R),
      where R is the pole radius setting.

    */
    void setEqualGainZeroes(void)
    {
        b_[0] = 1.0;
        b_[1] = 0.0;
        b_[2] = -1.0;
    }

    void clear(void)
    {
        for (int16_t i = 0; i < 3; i++)
        {
            inputs_[i] = 0;
            outputs_[i] = 0;
        }
        last_output_ = 0;
    }

    //! Return the last computed output value.
    T lastOut(void) const { return last_output_; };

    //! Input one sample to the filter and return a reference to one output.
    T next(T input)
    {
        inputs_[0] = gain_ * input;
        last_output_ = b_[0] * inputs_[0] + b_[1] * inputs_[1] + b_[2] * inputs_[2];
        last_output_ -= a_[2] * outputs_[2] + a_[1] * outputs_[1];
        inputs_[2] = inputs_[1];
        inputs_[1] = inputs_[0];
        outputs_[2] = outputs_[1];
        outputs_[1] = last_output_;

        return last_output_;
    };

protected:
    float *a_;
    float *b_;

    T *inputs_;
    T *outputs_;

    T last_output_;
};

#endif // MEAP_BIQUAD_H
