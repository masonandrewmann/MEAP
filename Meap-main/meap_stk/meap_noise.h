#ifndef MEAP_NOISE_H
#define MEAP_NOISE_H

/***************************************************/
/*! \class stk::Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

class MEAP_Noise
{
public:
    //! Default constructor that can also take a specific seed value.
    /*!
      If the seed value is zero (the default value), the random number generator is
      seeded with the system time.
    */
    MEAP_Noise() {};

    //! Return the last computed output value.
    int16_t lastOut(void) const { return last_output_; };

    //! Compute and return one 8-bit signed (-128 to 127) output sample.
    int16_t next(void)
    {
        last_output_ = xorshift96() % 65535 - 32768;
        return last_output_;
    };

protected:
    int16_t last_output_;
};

#endif // MEAP_NOISE_H
