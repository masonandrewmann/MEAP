#ifndef MEAP_BOWTABLE_H
#define MEAP_BOWTABLE_H

/***************************************************/
/*! \class stk::BowTable
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith
    (1986).  The output is an instantaneous
    reflection coefficient value.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/
template <class T = int16_t>
class MEAP_BowTable
{
public:
    //! Default constructor.
    MEAP_BowTable(void) : offset_(0.0), slope_(0.1), minOutput_(0.01), maxOutput_(0.98){};

    //! Set the table offset value.
    /*!
      The table offset is a bias which controls the
      symmetry of the friction.  If you want the
      friction to vary with direction, use a non-zero
      value for the offset.  The default value is zero.
    */
    void setOffset(float offset) { offset_ = offset; };

    //! Set the table slope value.
    /*!
     The table slope controls the width of the friction
     pulse, which is related to bow force.
    */
    void setSlope(float slope) { slope_ = slope; };

    //! Set the minimum table output value (0.0 - 1.0).
    void setMinOutput(float minimum) { minOutput_ = minimum; };

    //! Set the maximum table output value (0.0 - 1.0).
    void setMaxOutput(float maximum) { maxOutput_ = maximum; };

    //! Take one sample input and map to one sample of output.
    float next(float input)
    {
        // The input represents differential string vs. bow velocity.
        float sample = input + offset_; // add bias to input
        sample *= slope_;               // then scale it
        last_output_ = (float)fabs((float)sample) + (float)0.75;
        last_output_ = (float)pow(last_output_, (float)-4.0);

        // Set minimum threshold
        if (last_output_ < minOutput_)
            last_output_ = minOutput_;

        // Set maximum threshold
        if (last_output_ > maxOutput_)
            last_output_ = maxOutput_;

        return last_output_;
    };

protected:
    float offset_;
    float slope_;
    float minOutput_;
    float maxOutput_;

    T last_output_;
};

#endif // MEAP_BOWTABLE_H
