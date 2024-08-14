#ifndef MEAP_EFFECT_H
#define MEAP_EFFECT_H

/***************************************************/
/*! \class stk::Effect
    \brief STK abstract effects parent class.

    This class provides common functionality for STK effects
    subclasses.  It is general enough to support both monophonic and
    polyphonic input/output classes.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

class MEAP_Effect : public Meap
{
public:
    //! Class constructor.
    MEAP_Effect(void) {};

    //! Set the mixture of input and "effected" levels in the output (0.0 = input only, 1.0 = effect only).
    void setEffectMix(float mix)
    {
        if (mix < 0.0f)
        {
            effectMix_ = 0.0f;
        }
        else if (mix > 1.0f)
        {
            effectMix_ = 1.0f;
        }
        else
            effectMix_ = mix;
    };

protected:
    // Returns true if argument value is prime.
    bool isPrime(uint16_t number)
    {
        if (number == 2)
            return true;
        if (number & 1)
        {
            for (int16_t i = 3; i < (int16_t)sqrt((float)number) + 1; i += 2)
                if ((number % i) == 0)
                    return false;
            return true; // prime
        }
        else
            return false; // even
    };

    float effectMix_;
};

#endif // MEAP_EFFECT_H
