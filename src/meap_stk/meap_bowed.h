#ifndef MEAP_BOWED_H
#define MEAP_BOWED_H

// #include "Instrmnt.h"
#include "meap_delay_linear.h"
#include "meap_bowtable.h"
#include "meap_one_pole.h"
#include "meap_biquad.h"
#include "meap_sinewave.h"
#include "meap_adsr.h"

/***************************************************/
/*! \class stk::Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Bow Velocity = 100
       - Frequency = 101
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
    Contributions by Esteban Maestre, 2011.
*/
/***************************************************/

class MEAP_Bowed
{
public:
    //! Class constructor, taking the lowest desired playing frequency.
    MEAP_Bowed(float lowestFrequency = 8.0)
    {
        uint32_t nDelays = (uint32_t)((float)AUDIO_RATE / lowestFrequency);

        neckDelay_.setMaximumDelay(nDelays + 1);
        neckDelay_.setDelay(100.0);

        bridgeDelay_.setMaximumDelay(nDelays + 1);
        bridgeDelay_.setDelay(29.0);

        bowTable_.setSlope(3.0);
        bowTable_.setOffset(0.001);
        bowDown_ = false;
        maxVelocity_ = 0.25;

        vibrato_.setFrequency(6.12723);
        vibratoGain_ = 0.0;

        stringFilter_.setPole(0.75f - (0.2f * 22050.0f / (float)AUDIO_RATE));
        stringFilter_.setGain(0.95);

        // Old single body filter
        // bodyFilter_.setResonance( 500.0, 0.85, true );
        // bodyFilter_.setGain( 0.2 );

        // New body filter provided by Esteban Maestre (cascade of second-order sections)
        bodyFilters_[0].setCoefficients(1.0, 1.5667, 0.3133, -0.5509, -0.3925);
        bodyFilters_[1].setCoefficients(1.0, -1.9537, 0.9542, -1.6357, 0.8697);
        bodyFilters_[2].setCoefficients(1.0, -1.6683, 0.8852, -1.7674, 0.8735);
        bodyFilters_[3].setCoefficients(1.0, -1.8585, 0.9653, -1.8498, 0.9516);
        bodyFilters_[4].setCoefficients(1.0, -1.9299, 0.9621, -1.9354, 0.9590);
        bodyFilters_[5].setCoefficients(1.0, -1.9800, 0.9888, -1.9867, 0.9923);

        adsr_.setAllTimes(0.02, 0.005, 0.9, 0.01);

        betaRatio_ = 0.127236;

        // Necessary to initialize internal variables.
        this->setFrequency(220.0);
        this->clear();
    };

    //! Class destructor.
    ~MEAP_Bowed(void){};

    //! Reset and clear all internal state.
    void clear(void)
    {
        neckDelay_.clear();
        bridgeDelay_.clear();
        stringFilter_.clear();
        for (int i = 0; i < 6; i++)
            bodyFilters_[i].clear();
    };

    //! Set instrument parameters for a particular frequency.
    void setFrequency(float frequency)
    {
        // Delay = length - approximate filter delay.
        baseDelay_ = ((float)AUDIO_RATE) / frequency - 4.0f;
        if (baseDelay_ <= 0.0f)
            baseDelay_ = 0.3;
        bridgeDelay_.setDelay(baseDelay_ * betaRatio_);        // bow to bridge length
        neckDelay_.setDelay(baseDelay_ * (1.0f - betaRatio_)); // bow to nut (finger) length
    };

    //! Set vibrato gain.
    void setVibrato(float gain) { vibratoGain_ = gain; };

    //! Apply breath pressure to instrument with given amplitude and rate of increase. args greater than zero
    void startBowing(float amplitude, float rate)
    {
        adsr_.setAttackRate(rate);
        adsr_.keyOn();
        maxVelocity_ = 0.03f + (0.2f * amplitude);
        bowDown_ = true;
    };

    //! Decrease breath pressure with given rate of decrease. rate greater than zero
    void stopBowing(float rate)
    {
        adsr_.setReleaseRate(rate);
        adsr_.keyOff();
    };

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, float amplitude)
    {
        this->startBowing(amplitude, amplitude * 0.001f);
        this->setFrequency(frequency);
    };

    //! Stop a note with the given amplitude (speed of decay).
    void noteOff(float amplitude)
    {
        this->stopBowing((1.0f - amplitude) * 0.005f);
    };

    //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
    void controlChange(int number, float value)
    {
        float normalizedValue = value * 0.0078125; // 1/128
        if (number == 2)
        { // __SK_BowPressure_
            if (normalizedValue > 0.0f)
                bowDown_ = true;
            else
                bowDown_ = false;
            bowTable_.setSlope(5.0f - (4.0f * normalizedValue));
        }
        else if (number == 4)
        { // __SK_BowPosition_
            betaRatio_ = normalizedValue;
            bridgeDelay_.setDelay(baseDelay_ * betaRatio_);
            neckDelay_.setDelay(baseDelay_ * (1.0f - betaRatio_));
        }
        else if (number == 11) // __SK_ModFrequency_
            vibrato_.setFrequency(normalizedValue * 12.0f);
        else if (number == 1) // __SK_ModWheel_
            vibratoGain_ = (normalizedValue * 0.4f);
        else if (number == 100) // 100: set instantaneous bow velocity
            adsr_.setTarget(normalizedValue);
        else if (number == 101) // 101: set instantaneous value of frequency
            this->setFrequency(value);
        else if (number == 128) // __SK_AfterTouch_Cont_
            adsr_.setTarget(normalizedValue);
    };

    //! Compute and return one output sample.
    float next()
    {
        float bowVelocity = maxVelocity_ * adsr_.next();
        float bridgeReflection = -stringFilter_.next(bridgeDelay_.lastOut());
        float nutReflection = -neckDelay_.lastOut();
        float stringVelocity = bridgeReflection + nutReflection;
        float deltaV = bowVelocity - stringVelocity; // Differential velocity

        float newVelocity = 0.0;
        if (bowDown_)
            newVelocity = deltaV * bowTable_.next(deltaV); // Non-Linear bow function
        neckDelay_.next(bridgeReflection + newVelocity);   // Do string propagations
        bridgeDelay_.next(nutReflection + newVelocity);

        if (vibratoGain_ > 0.0f)
        {
            neckDelay_.setDelay((baseDelay_ * (1.0f - betaRatio_)) +
                                (baseDelay_ * vibratoGain_ * vibrato_.next()));
        }

        last_output_ = 0.1248f * bodyFilters_[5].next(bodyFilters_[4].next(bodyFilters_[3].next(bodyFilters_[2].next(bodyFilters_[1].next(bodyFilters_[0].next(bridgeDelay_.lastOut()))))));

        return last_output_;
    };

protected:
    MEAP_Delay_Linear<int16_t> neckDelay_;
    MEAP_Delay_Linear<int16_t> bridgeDelay_;
    MEAP_BowTable<int16_t> bowTable_;
    MEAP_OnePole<int16_t> stringFilter_;
    MEAP_BiQuad<int16_t> bodyFilters_[6];
    MEAP_SineWave vibrato_;
    MEAP_ADSR adsr_;

    bool bowDown_;
    float maxVelocity_;
    float baseDelay_;
    float vibratoGain_;
    float betaRatio_;

    float last_output_;
};
#endif // MEAP_BOWED_H
