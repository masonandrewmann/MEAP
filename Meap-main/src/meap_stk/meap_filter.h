#ifndef MEAP_FILTER_H
#define MEAP_FILTER_H

/***************************************************/
/*! \class stk::Filter
    \brief STK abstract filter class.

    This class provides limited common functionality for STK digital
    filter subclasses.  It is general enough to support both
    monophonic and polyphonic input/output classes.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

class MEAP_Filter
{
public:
    //! Class constructor.
    MEAP_Filter(void)
    {
        gain_ = 1.0;
    };

    //! Set the filter gain.
    /*!
      The gain is applied at the filter input and does not affect the
      coefficient values.  The default gain value is 1.0.
     */
    void setGain(float gain) { gain_ = gain; };

    //! Return the current filter gain.
    float getGain(void) const { return gain_; };

protected:
    float gain_;
};

#endif // MEAP_FILTER_H
