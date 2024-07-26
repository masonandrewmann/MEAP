// I NEED HELP!!!!!!

#ifndef MEAP_RECORDER_H
#define MEAP_RECORDER_H

#include "meap_iir.h"
#include "meap_delay_linear.h"
#include "meap_noise.h"
#include "meap_sinewave.h"
#include "meap_adsr.h"

/***************************************************/
/*! \class stk::Recorder
    \brief A recorder / flute physical model.

    This class implements a physical model of a recorder /
    flute instrument, based on the paper "Sound production
    in recorderlike instruments. II. A simulation model."
    by M.P. Verge, A. Hirschberg and R. Causse, Journal of
    the Acoustical Society of America, 1997.

    Control Change Numbers:
       - Softness = 2
       - Noise Gain = 4
       - Noise Cutoff = 16
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Mathias Bredholt, McGill University.
    Formatted for STK by Gary Scavone, 2019.
*/
/***************************************************/

// Air constants
const float rho = 1.2041; // density of air
const float c0 = 343.21;  // speed of sound in air

// Flute constants
const float lc = 0.02;           // length of flue canal
const float h = 0.001;           // height of flue exit
const float H = 0.02;            // pipe diameter
const float W = 0.004;           // 4 * h;       // width of mouth
const float Sp = 0.0004;         // H * H;      // cross-section of pipe
const float Sm = 0.00008;        // W * H;      // cross-section of mouth
const float din = 0.0030;        // end correction
const float dout = 0.0063;       // end correction
const float dm = 0.0093;         // din + dout;     // end correction of mouth
const float dd = 0.0035;         // acoustic distance between Q1 and Q2
const float rp = 0.000225675833; // std::sqrt(Sp / stk::STK_PI);
const float b = 0.0004;          // 0.4f * h; // jet width

// Calculation coefficients
const float b2 = 0.000000000387166; // Sp / (rho * c0)

class MEAP_Recorder
{
public:
    //! Class constructor.
    MEAP_Recorder(void)
    {
        vibratoGain_ = 0.0;
        noiseGain_ = 0.2;
        breathCutoff_ = 0.0;
        outputGain_ = 0.0;
        psi_ = 1.0;
        poutL_ = 0;
        pout_ = 0;
        poutm1_ = 0;
        poutm2_ = 0;
        pin_ = 0;
        pinm1_ = 0;
        pinm2_ = 0;
        Uj_ = 0;
        Ujm1_ = 0;
        Qj_ = 0;
        Qjm1_ = 0;
        Qjm2_ = 0;
        Q1_ = 0;
        Q1m1_ = 0;
        Q1m2_ = 0;
        Qp_ = 0;
        Qpm1_ = 0;
        pm_ = 0;

        pinDelay_.next(0);
        poutDelay_.next(0);
        jetDelay_.next(0);

        jetDelay_.setDelay(200);
        vibrato_.setFrequency(4);

        // Calculation coefficients ... would need to be recalculated if sample rate changes
        float T = 1.0f / (float)AUDIO_RATE;
        b1 = rho / (4.0f * 3.14159265358979 * c0 * T * T);
        b3 = dm * Sp / (T * Sm * c0);
        b4 = rho * dout / (Sm * T);

        // Radiation loss filter
        float A = rp * rp / (4 * c0 * c0 * T * T);
        float B = 0.82f * rp / (c0 * T);
        float b_rad[3] = {1 + A - B, B - 2 * A, A};
        float a_rad[3] = {A - B - 1, B - 2 * A, A};
        // IDK WHY THESE LINES ARE GRABBING ADDRESSES, THAT FEELS WRONG SO IM NOT GONNA DO IT
        // SMTH STILL SEEMS FISHY HERE, DEF INVESTIGATE WHN IT DOESNT WORK
        // MAYBE WHAT THEY WERE DOING IS JUST A WEIRD WAY OF COPYING THAT ARRAY INTO A VECTOR
        // AND I SHOULD JSUT SEND (b_rad, 3) in directly
        // std::vector<float> b_coeffs(&b_rad[0], &b_rad[0] + 3);
        // std::vector<float> a_coeffs(&a_rad[0], &a_rad[0] + 3);
        float b_coeffs[2] = {b_rad[0], b_rad[0] + 3};
        float a_coeffs[2] = {a_rad[0], a_rad[0] + 3};

        radiation_filter_.setCoefficients(b_coeffs, 2, a_coeffs, 2);

        // Visco-thermal loss filter
        float b_visco[4] = {0.83820223947141, -0.16888603248373, -0.64759781930259, 0.07424498608506};
        float a_visco[4] = {1.0, -0.33623476246554, -0.71257915055968, 0.14508304017256};
        // b_coeffs.clear();
        // b_coeffs.assign(&b_visco[0], &b_visco[0] + 4);
        // a_coeffs.clear();
        // a_coeffs.assign(&a_visco[0], &a_visco[0] + 4);
        b_coeffs[0] = b_visco[0];
        b_coeffs[1] = b_visco[0] + 4;

        a_coeffs[0] = a_visco[0];
        a_coeffs[1] = a_visco[0] + 4;

        visco_in_filter_.setCoefficients(b_coeffs, 2, a_coeffs, 2);
        visco_out_filter_.setCoefficients(b_coeffs, 2, a_coeffs, 2);

        setBreathCutoff(500);
        setFrequency(880);
    };

    //! Class destructor.
    ~MEAP_Recorder(void){};

    //! Reset and clear all internal state.
    void clear(void)
    {
        pinDelay_.clear();
        poutDelay_.clear();
        jetDelay_.clear();
        radiation_filter_.clear();
        visco_in_filter_.clear();
        visco_out_filter_.clear();
        turbFilter_.clear();
    };

    //! Set instrument parameters for a particular frequency.
    void setFrequency(float val)
    {
        float M = ((float)AUDIO_RATE) / val - 4 - 3;
        pinDelay_.setDelay(M);
        poutDelay_.setDelay(M);
    };

    //! Apply breath velocity to instrument with given amplitude and rate of increase. args greater than zero
    void startBlowing(float amplitude, float rate)
    {

        adsr_.setAttackRate(rate);
        // maxPressure_ = amplitude / (float) 0.8;
        maxPressure_ = 35.0f * amplitude;
        adsr_.keyOn();
    }

    //! Decrease breath velocity with given rate of decrease. rate greater than zero
    void
    stopBlowing(float rate)
    {
        adsr_.setReleaseRate(rate);
        adsr_.keyOff();
    };

    //! Start a note with the given frequency and amplitude.
    void noteOn(float frequency, float amplitude)
    {
        this->setFrequency(frequency);
        this->startBlowing(1.1f + (amplitude * 0.20f), amplitude * 0.02f);
        outputGain_ = amplitude / 40.0f;
    };

    //! Stop a note with the given amplitude (speed of decay).
    void noteOff(float amplitude)
    {
        this->stopBlowing(amplitude * 0.02f);
    };

    //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
    void controlChange(int16_t number, float value)
    {
        float normalizedValue = value * 0.0078125; // 1/128
        if (number == 2)                           // 2
            psi_ = 2.0f * normalizedValue;
        else if (number == 16)
            setBreathCutoff(normalizedValue * 2000);
        else if (number == 4) // __SK_NoiseLevel_
            noiseGain_ = normalizedValue;
        else if (number == 11) // __SK_ModFrequency_
            vibrato_.setFrequency(normalizedValue * 12.0f);
        else if (number == 1) // __SK_ModWheel_
            vibratoGain_ = (normalizedValue * 0.4f);
        else if (number == 128) // __SK_AfterTouch_Cont_
            maxPressure_ = 35.0f * normalizedValue;
    };

    //! Compute and return one output sample.
    float next()
    {
        // Read in from delay lines
        pinm2_ = pinm1_;
        pinm1_ = pin_;
        pin_ = pinDelay_.lastOut();

        poutm2_ = poutm1_;
        poutm1_ = pout_;
        poutL_ = poutDelay_.lastOut();

        // Filter wave components for visco-thermal losses
        pin_ = visco_in_filter_.next(pin_);
        poutL_ = visco_out_filter_.next(poutL_);

        // Get input blow pressure
        float pf = maxPressure_ * adsr_.next() * (vibrato_.next() * vibratoGain_ + (1.0f - vibratoGain_));

        float T = 1.0f / ((float)AUDIO_RATE);

        // Jet velocity at flue exit
        Ujm1_ = Uj_;
        Uj_ = Ujm1_ + T / (rho * lc) * (pf - pm_ - 0.5f * rho * Ujm1_ * Ujm1_);

        // Jet flow at flue exit
        Qjm2_ = Qjm1_;
        Qjm1_ = Qj_;
        Qj_ = h * H * Uj_;

        // Jet drive
        float Uj_steady = max(sqrt(2.0f * pf / rho), 0.1f);
        float fc_jet = 0.36f / W * Uj_steady;
        float g_jet = 0.002004f * exp(-0.06046f * Uj_steady);
        float r_jet = 0.95f - Uj_steady * 0.015f;
        float b0_jet = g_jet * (1 - r_jet * r_jet) / 2;

        // Calculate coefficients for resonant filter
        float b_jet[3] = {b0_jet, 0, -b0_jet};
        float a_jet[3] = {1, -2 * r_jet * cos(2 * 3.14159265358979 * fc_jet * T), r_jet * r_jet};

        // AGAIN THIS SYNTAX FOR SETTING THE COEFFICENTS SEEMS WEIRD !!!!!!!!!
        // std::vector<float> b_jetcoeffs(&b_jet[0], &b_jet[0] + 3);
        // std::vector<float> a_jetcoeffs(&a_jet[0], &a_jet[0] + 3);
        float b_jetcoeffs[2] = {b_jet[0], b_jet[0] + 3};
        float a_jetcoeffs[2] = {a_jet[0], a_jet[0] + 3};

        jetFilter_.setCoefficients(b_jetcoeffs, 2, a_jetcoeffs, 2);
        float eta = jetFilter_.next(jetDelay_.lastOut());

        // Calculate flow source Q1
        Q1m1_ = Q1_;
        Q1_ = b * H * Uj_ * (1 + tanh(eta / (psi_ * b)));

        // Calculate pressure pulse modeling the jet drive
        float pjd = -rho * dd / Sm * (Q1_ - Q1m1_) / T;

        // Vortex shedding
        int Qp_sign = 0;
        if (Qp_ < 0)
            Qp_sign = -1;
        else if (Qp_ > 0)
            Qp_sign = 1;

        float pa = -0.5f * rho * (Qp_ / (0.6f * Sm)) * (Qp_ / (0.6f * Sm)) * Qp_sign;

        // Turbulence
        float pt = turbFilter_.next(noiseGain_ * turb_.next() * 0.5f * rho * Uj_ * Uj_);

        // Pressure pulse delta p
        float dp = pjd + pa + pt;

        // Calculate outgoing pressure pout
        pout_ = ((b3 - b1 * b2 - 1) * pin_ +
                 (2 * b1 * b2 - b3) * (pinm1_ - poutm1_) +
                 b1 * b2 * (poutm2_ - pinm2_) -
                 b1 * (Qj_ - 2 * Qjm1_ + Qjm2_) +
                 b4 * (Qj_ - Qjm1_) + dp) /
                (1 - b1 * b2 + b3);

        // Flow in the pipe
        Qpm1_ = Qp_;
        Qp_ = Sp / (rho * c0) * (pout_ - pin_);

        // Mouth pressure
        pm_ = pout_ + pin_ - dp + rho * din / Sm * (Qp_ - Qpm1_) / T;

        // Calculate transverse acoustic velocity
        float Q1d = Q1_ - 0.5f * b * H * Uj_;
        float Vac = 2.0f / 3.14159265358979 * Qp_ / Sm - 0.38f * Q1d / Sm;
        jetDelay_.next(Vac);

        // Calculate new jet delay line length
        // jet_.setDelay(fmin(W / (0.6 * Uj_steady) * sampleRate(), 200.0));
        jetDelay_.setDelay(fmin(W / (0.6f * Uj_steady * T), 200.0f));

        // Radiation loss filtering
        float pin_L = radiation_filter_.next(poutL_);

        // Write to delay lines
        poutDelay_.next(pout_);
        pinDelay_.next(pin_L);

        last_output_ = outputGain_ * (pout_ + pin_);
        return last_output_;

        // return (pout_0 + pin_0) * 0.01;
    };

    void setBlowPressure(float val)
    {
        maxPressure_ = val;
    };
    ;

    void setVibratoGain(float val)
    {
        vibratoGain_ = val;
    };

    void setVibratoFrequency(float val)
    {
        vibrato_.setFrequency(val);
    };

    void setNoiseGain(float val)
    {
        noiseGain_ = val;
    };

    void setBreathCutoff(float val)
    {
        // The gain of this filter is quite high
        breathCutoff_ = val;
        float Q = 0.99;
        float r = 2.0f * sin(3.14159265358979 * val / ((float)AUDIO_RATE));
        float q = 1.0f - r * Q;
        float as[3] = {1.0f, r * r - q - 1, q};

        // std::vector<float> b_turb(1, r * r);
        // std::vector<float> a_turb(&as[0], &as[0] + 3);

        float b_turb[2] = {1, r * r};

        // ANOTHER WEIRD COEFFICIENTS THING ******************************************************************************************************************************
        float a_turb[2] = {as[0], as[0] + 3};

        turbFilter_.setCoefficients(b_turb, 2, a_turb, 2);
    };

    void setSoftness(float val)
    {
        psi_ = val;
    };

private:
    MEAP_Delay_Linear<int16_t> pinDelay_;
    MEAP_Delay_Linear<int16_t> poutDelay_;
    MEAP_Delay_Linear<int16_t> jetDelay_;
    MEAP_Iir<int16_t> radiation_filter_;
    MEAP_Iir<int16_t> visco_in_filter_;
    MEAP_Iir<int16_t> visco_out_filter_;
    MEAP_Iir<int16_t> jetFilter_;
    MEAP_Noise turb_;
    MEAP_Iir<int16_t> turbFilter_;
    MEAP_SineWave vibrato_;
    MEAP_ADSR adsr_;

    // float M{ 0 };
    // float maxPressure_( 0 );
    float maxPressure_;
    // float blow{ 0 };
    float vibratoGain_;
    float noiseGain_;
    float breathCutoff_;
    float outputGain_;
    float psi_;

    float poutL_;
    float pout_;
    float poutm1_;
    float poutm2_;
    float pin_;
    float pinm1_;
    float pinm2_;

    float b1;
    float b3;
    float b4;

    float Uj_;
    float Ujm1_;

    float Qj_;
    float Qjm1_;
    float Qjm2_;

    float Q1_;
    float Q1m1_;
    float Q1m2_;

    float Qp_;
    float Qpm1_;

    float pm_;

    int16_t last_output_; // this should maybe be a float, or normalize it to 16bit int at end of next
};

#endif // MEAP_RECORDER_H
