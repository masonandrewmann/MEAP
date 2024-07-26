#ifndef MEAP_BRASS_H
#define MEAP_BRASS_H

// #include "Instrmnt.h"
#include "meap_delay_allpass.h"
#include "meap_biquad.h"
#include "meap_polezero.h"
#include "meap_adsr.h"
#include "meap_sinewave.h"

/***************************************************/
/*! \class stk::Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

template <class T = int16_t>
class MEAP_Brass
{
public:
    //! Class constructor, taking the lowest desired playing frequency.
    /*!
      An StkError will be thrown if the rawwave path is incorrectly set.
    */
    MEAP_Brass(float lowestFrequency = 8.0)
    {

        uint32_t nDelays = (uint32_t)(AUDIO_RATE / lowestFrequency);
        delayLine_.setMaximumDelay(nDelays + 1);

        lipFilter_.setGain(0.03);
        dcBlock_.setBlockZero();
        adsr_.setAllTimes(0.005, 0.001, 1.0, 0.010);

        vibrato_.setFrequency(6.137);
        vibratoGain_ = 0.0;
        maxPressure_ = 0.0;
        lipTarget_ = 0.0;

        this->clear();

        // This is necessary to initialize variables.
        this->setFrequency(220.0);
    };

    //! Class destructor.
    ~MEAP_Brass(){};

    //! Reset and clear all internal state.
    void clear()
    {
        delayLine_.clear();
        lipFilter_.clear();
        dcBlock_.clear();
    };

    //! Set instrument parameters for a particular frequency.
    void setFrequency(float frequency)
    {

        // Fudge correction for filter delays.
        slideTarget_ = (AUDIO_RATE / frequency * 2.0f) + 3.0f;
        delayLine_.setDelay(slideTarget_); // play a harmonic

        lipTarget_ = frequency;
        lipFilter_.setResonance(frequency, 0.997);
    };

    //! Set the lips frequency. greater than zero
    void setLip(float frequency)
    {
        lipFilter_.setResonance(frequency, 0.997);
    };

    //! Apply breath pressure to instrument with given amplitude and rate of increase. both arg greater than zero
    void startBlowing(float amplitude, float rate)
    {
        adsr_.setAttackRate(rate);
        maxPressure_ = amplitude;
        adsr_.keyOn();
    };

    //! Decrease breath pressure with given rate of decrease. rate greater than zero
    void stopBlowing(float rate)
    {
        adsr_.setReleaseRate(rate);
        adsr_.keyOff();
    };

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, float amplitude)
    {
        this->setFrequency(frequency);
        this->startBlowing(amplitude, amplitude * 0.001f);
    };

    //! Stop a note with the given amplitude (speed of decay).
    void noteOff(float amplitude)
    {
        this->stopBlowing(amplitude * 0.005f);
    };

    //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
    void controlChange(int number, float value)
    {
        float normalizedValue = value * 0.0078125; // 1/128
        if (number == 2)
        { // __SK_LipTension_
            float temp = lipTarget_ * pow(4.0f, (2.0f * normalizedValue) - 1.0f);
            this->setLip(temp);
        }
        else if (number == 4) // __SK_SlideLength_
            delayLine_.setDelay(slideTarget_ * (0.5f + normalizedValue));
        else if (number == 11) // __SK_ModFrequency_
            vibrato_.setFrequency(normalizedValue * 12.0f);
        else if (number == 1) // __SK_ModWheel_
            vibratoGain_ = normalizedValue * 0.4f;
        else if (number == 128) // __SK_AfterTouch_Cont_
            adsr_.setTarget(normalizedValue);
    };

    //! Compute and return one output sample.
    float next()
    {
        float breathPressure = maxPressure_ * adsr_.next();
        breathPressure += vibratoGain_ * vibrato_.next();

        float mouthPressure = 0.3f * breathPressure;
        float borePressure = 0.85f * delayLine_.lastOut();
        float deltaPressure = mouthPressure - borePressure; // Differential pressure.
        deltaPressure = lipFilter_.next(deltaPressure);     // Force - > position.
        deltaPressure *= deltaPressure;                     // Basic position to area mapping.
        if (deltaPressure > 1.0f)
            deltaPressure = 1.0f; // Non-linear saturation.

        // The following input scattering assumes the mouthPressure = area.
        last_output_ = deltaPressure * mouthPressure + (1.0f - deltaPressure) * borePressure;
        last_output_ = delayLine_.next(dcBlock_.next(last_output_));

        return last_output_;
    };

protected:
    MEAP_Delay_Allpass<int16_t> delayLine_;
    MEAP_BiQuad<int16_t> lipFilter_;
    MEAP_PoleZero<int16_t> dcBlock_;
    MEAP_ADSR adsr_;
    MEAP_SineWave vibrato_;

    float lipTarget_;
    float slideTarget_;
    float vibratoGain_;
    float maxPressure_;

    T last_output_;
};

#endif // MEAP_BRASS_H
