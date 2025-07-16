#ifndef MEAP_BANDEDWG_H
#define MEAP_BANDEDWG_H

#include "meap_delay_linear.h"
#include "meap_bowtable.h"
#include "meap_adsr.h"
#include "meap_biquad.h"

/***************************************************/
/*! \class stk::BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers:
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999--2004.
    Modified for STK 4.0 by Gary Scavone.
*/
/***************************************************/

#define WG_MAX_BANDED_MODES 20

template <class T = int16_t>
class MEAP_BandedWG
{
public:
    //! Class constructor.
    MEAP_BandedWG(void)
    {
        doPluck_ = true;

        bowTable_.setSlope(3.0);
        adsr_.setAllTimes(0.02, 0.005, 0.9, 0.01);

        frequency_ = 220.0;
        this->setPreset(0);

        bowPosition_ = 0;
        baseGain_ = 0.999;

        integrationConstant_ = 0.0;
        trackVelocity_ = false;

        bowVelocity_ = 0.0;
        bowTarget_ = 0.0;

        strikeAmp_ = 0.0;
    };

    //! Class destructor
    ~MEAP_BandedWG(){};

    //! Reset and clear all internal state.
    void clear(void)
    {
        for (int16_t i = 0; i < nModes_; i++)
        {
            delay_[i].clear();
            bandpass_[i].clear();
        }
    };

    //! Set strike position (0.0 - 1.0).
    void setStrikePosition(float position)
    {
        strikePosition_ = (int16_t)(delay_[0].getDelay() * position / 2.0f);
    };

    //! Select a preset.
    void setPreset(int preset)
    {

        // doPluck_ = true;

        // bowTable_.setSlope(3.0);
        // adsr_.setAllTimes(0.02, 0.005, 0.9, 0.01);

        // frequency_ = 220.0;
        // // this->setPreset(0);

        // bowPosition_ = 0;
        // baseGain_ = 0.999;

        // integrationConstant_ = 0.0;
        // trackVelocity_ = false;

        // bowVelocity_ = 0.0;
        // bowTarget_ = 0.0;

        // strikeAmp_ = 0.0;
        int i;
        switch (preset)
        {

        case 1: // Tuned Bar
            presetModes_ = 4;
            modes_[0] = (float)1.0;
            modes_[1] = (float)4.0198391420;
            modes_[2] = (float)10.7184986595;
            modes_[3] = (float)18.0697050938;

            for (i = 0; i < presetModes_; i++)
            {
                basegains_[i] = (float)pow(0.999, (float)i + 1);
                excitation_[i] = 322768; // changed from 1.0
            }

            break;

        case 2: // Glass Harmonica
            presetModes_ = 5;
            modes_[0] = (float)1.0;
            modes_[1] = (float)2.32;
            modes_[2] = (float)4.25;
            modes_[3] = (float)6.63;
            modes_[4] = (float)9.38;
            // modes_[5] = (float) 12.22;

            for (i = 0; i < presetModes_; i++)
            {
                basegains_[i] = (float)pow(0.999999, (float)i + 1);
                excitation_[i] = 32768;
            }
            /*
              baseGain_ = (float) 0.99999;
              for (i=0; i<presetModes_; i++)
              gains_[i]= (float) std::pow(baseGain_, delay_[i].getDelay()+i);
            */

            break;

        case 3: // Tibetan Prayer Bowl (ICMC'02)
            presetModes_ = 12;
            modes_[0] = 0.996108344;
            basegains_[0] = 0.999925960128219;
            excitation_[0] = 11.900357 / 10.0;
            modes_[1] = 1.0038916562;
            basegains_[1] = 0.999925960128219;
            excitation_[1] = 11.900357 / 10.;
            modes_[2] = 2.979178;
            basegains_[2] = 0.999982774366897;
            excitation_[2] = 10.914886 / 10.;
            modes_[3] = 2.99329767;
            basegains_[3] = 0.999982774366897;
            excitation_[3] = 10.914886 / 10.;
            modes_[4] = 5.704452;
            basegains_[4] = 1.0; // 0.999999999999999999987356406352;
            excitation_[4] = 42.995041 / 10.;
            modes_[5] = 5.704452;
            basegains_[5] = 1.0; // 0.999999999999999999987356406352;
            excitation_[5] = 42.995041 / 10.;
            modes_[6] = 8.9982;
            basegains_[6] = 1.0; // 0.999999999999999999996995497558225;
            excitation_[6] = 40.063034 / 10.;
            modes_[7] = 9.01549726;
            basegains_[7] = 1.0; // 0.999999999999999999996995497558225;
            excitation_[7] = 40.063034 / 10.;
            modes_[8] = 12.83303;
            basegains_[8] = 0.999965497558225;
            excitation_[8] = 7.063034 / 10.;
            modes_[9] = 12.807382;
            basegains_[9] = 0.999965497558225;
            excitation_[9] = 7.063034 / 10.;
            modes_[10] = 17.2808219;
            basegains_[10] = 0.9999999999999999999965497558225;
            excitation_[10] = 57.063034 / 10.;
            modes_[11] = 21.97602739726;
            basegains_[11] = 0.999999999999999965497558225;
            excitation_[11] = 57.063034 / 10.;

            break;

        default: // Uniform Bar
            presetModes_ = 4;
            modes_[0] = (float)1.0;
            modes_[1] = (float)2.756;
            modes_[2] = (float)5.404;
            modes_[3] = (float)8.933;

            for (i = 0; i < presetModes_; i++)
            {
                basegains_[i] = (float)pow(0.9, (float)i + 1);
                excitation_[i] = 32768; // changed from 1.0
            }

            break;
        }

        nModes_ = presetModes_;
        setFrequency(frequency_);
    };

    //! Set instrument parameters for a particular frequency. (greater than 0)
    void setFrequency(float frequency)
    {
        if (frequency > 1568.0f)
            frequency = 1568.0f;

        float radius;
        float base = ((float)AUDIO_RATE) / frequency;
        float length;
        for (int16_t i = 0; i < presetModes_; i++)
        {
            // Calculate the delay line lengths for each mode.
            length = (int16_t)(base / modes_[i]);
            if (length > 2.0f)
            {
                delay_[i].setDelay(length);
                gains_[i] = basegains_[i];
                //	  gains_[i]=(float) std::pow(basegains_[i], 1/((float)delay_[i].getDelay()));
                //	  std::cerr << gains_[i];
            }
            else
            {
                nModes_ = i;
                break;
            }
            //	std::cerr << std::endl;

            // Set the bandpass filter resonances
            radius = 1.0f - 100.5309649149 / (float)AUDIO_RATE; // frequency_ * modes_[i] / Stk::sampleRate()/32; 100.etc is 32 * pi
            if (radius < 0.0f)
                radius = 0.0f;
            bandpass_[i].setResonance(frequency * modes_[i], radius, true);

            delay_[i].clear();
            bandpass_[i].clear();
        }

        // int olen = (int)(delay_[0].getDelay());
        // strikePosition_ = (int)(strikePosition_*(length/modes_[0])/olen);
    };

    //! Apply bow velocity/pressure to instrument with given amplitude and rate of increase.
    void startBowing(float amplitude, float rate)
    {
        adsr_.setAttackRate(rate);
        adsr_.keyOn();
        maxVelocity_ = 0.03f + (0.1f * amplitude);
    };

    //! Decrease bow velocity/breath pressure with given rate of decrease.
    void stopBowing(float rate)
    {
        adsr_.setReleaseRate(rate);
        adsr_.keyOff();
    };

    //! Pluck the instrument with given amplitude.
    void pluck(float amplitude)
    {
        int16_t j;
        float min_len = delay_[nModes_ - 1].getDelay();
        for (int16_t i = 0; i < nModes_; i++)
            for (j = 0; j < (int16_t)(delay_[i].getDelay() / min_len); j++)
                delay_[i].next(excitation_[i] * amplitude / nModes_);

        //	strikeAmp_ += amplitude;
    };

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, float amplitude)
    {
        this->setFrequency(frequency);

        if (doPluck_)
            this->pluck(amplitude);
        else
            this->startBowing(amplitude, amplitude * 0.001f);
    };

    //! Stop a note with the given amplitude (speed of decay).
    void noteOff(float amplitude)
    {
        if (!doPluck_)
            this->stopBowing((1.0f - amplitude) * 0.005f);
    };

    //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
    void controlChange(int16_t number, float value)
    {
        float normalizedValue = value * 0.0078125; // 1/128
        if (number == 2)                           // __SK_BowPressure_
        {
            if (normalizedValue == 0.0f)
                doPluck_ = true;
            else
            {
                doPluck_ = false;
                bowTable_.setSlope(10.0f - (9.0f * normalizedValue));
            }
        }
        else if (number == 4)
        { // 4
            if (!trackVelocity_)
                trackVelocity_ = true;
            bowTarget_ += 0.005f * (normalizedValue - bowPosition_);
            bowPosition_ = normalizedValue;
            // adsr_.setTarget(bowPosition_);
        }
        else if (number == 8) // 8
            this->setStrikePosition(normalizedValue);
        else if (number == 128)
        { // __SK_AfterTouch_Cont_
            // bowTarget_ += 0.02 * (normalizedValue - bowPosition_);
            // bowPosition_ = normalizedValue;
            if (trackVelocity_)
                trackVelocity_ = false;
            maxVelocity_ = 0.13f * normalizedValue;
            adsr_.setTarget(normalizedValue);
        }
        else if (number == 1)
        { // __SK_ModWheel_
            //    baseGain_ = 0.9989999999 + (0.001 * normalizedValue );
            baseGain_ = 0.8999999999999999f + (0.1f * normalizedValue);
            //	std::cerr << "Yuck!" << std::endl;
            for (int16_t i = 0; i < nModes_; i++)
                gains_[i] = (float)basegains_[i] * baseGain_;
            //      gains_[i]=(float) std::pow(baseGain_, (int)((float)delay_[i].getDelay()+i));
        }
        else if (number == 11) // __SK_ModFrequency_
            integrationConstant_ = normalizedValue;
        else if (number == 64)
        { // __SK_Sustain_
            if (value < 65)
                doPluck_ = true;
            else
                doPluck_ = false;
        }
        else if (number == 65)
        { // __SK_Portamento_
            if (value < 65)
                trackVelocity_ = false;
            else
                trackVelocity_ = true;
        }
        else if (number == 16) // __SK_ProphesyRibbon_
            this->setPreset((int)value);
    };

    //! Compute and return one output sample.
    float next()
    {
        int16_t k;

        float input = 0.0;
        if (doPluck_)
        {
            input = 0.0;
            //  input = strikeAmp_/nModes_;
            //  strikeAmp_ = 0.0;
        }
        else
        {
            if (integrationConstant_ == 0.0f)
                velocityInput_ = 0.0f;
            else
                velocityInput_ = integrationConstant_ * velocityInput_;

            for (k = 0; k < nModes_; k++)
                velocityInput_ += baseGain_ * delay_[k].lastOut();

            if (trackVelocity_)
            {
                bowVelocity_ *= 0.9995f;
                bowVelocity_ += bowTarget_;
                bowTarget_ *= 0.995;
            }
            else
                bowVelocity_ = adsr_.next() * maxVelocity_;

            input = bowVelocity_ - velocityInput_;
            input = input * bowTable_.next(input);
            input = input / (float)nModes_;
        }

        float data = 0.0;
        for (k = 0; k < nModes_; k++)
        {
            bandpass_[k].next((int16_t)input + (int16_t)(gains_[k] * delay_[k].lastOut()));
            delay_[k].next(bandpass_[k].lastOut());
            data += bandpass_[k].lastOut();
        }

        // last_output_ = data * nModes_;
        last_output_ = data * 4;
        return last_output_;
    };

protected:
    bool doPluck_;
    bool trackVelocity_;
    int nModes_;
    int presetModes_;

    MEAP_BowTable<int16_t> bowTable_;
    MEAP_ADSR adsr_;
    MEAP_BiQuad<int16_t> bandpass_[WG_MAX_BANDED_MODES];
    MEAP_Delay_Linear<int16_t> delay_[WG_MAX_BANDED_MODES];

    float maxVelocity_;
    float modes_[WG_MAX_BANDED_MODES];
    float frequency_;
    float baseGain_;
    float gains_[WG_MAX_BANDED_MODES];
    float basegains_[WG_MAX_BANDED_MODES];
    T excitation_[WG_MAX_BANDED_MODES];
    float integrationConstant_;
    float velocityInput_;
    float bowVelocity_;
    float bowTarget_;
    float bowPosition_;
    float strikeAmp_;
    int strikePosition_;

    T last_output_;
};

#endif // MEAP_BANDEDWG_H
