// I NEED A LOT OF WORK :~(

#ifndef MEAP_LENTPITSHIFT_H
#define MEAP_LENTPITSHIFT_H

#include "meap_effect.h"
#include "meap_delay.h"

/***************************************************/
/*! \class stk::LentPitShift
    \brief Pitch shifter effect class based on the Lent algorithm.

    This class implements a pitch shifter using pitch
    tracking and sample windowing and shifting.

    by Francois Germain, 2009.
*/
/***************************************************/

#define MEAP_PS_BUFFER_SIZE 512

class MEAP_LentPitShift : public MEAP_Effect
{
public:
    //! Class constructor.
    MEAP_LentPitShift(float periodRatio = 1.0, int16_t tMax = MEAP_PS_BUFFER_SIZE) : inputFrames(0., tMax, 1), outputFrames(0., tMax, 1), ptrFrames(0), inputPtr(0), outputPtr(0.), tMax_(tMax), periodRatio_(periodRatio), zeroFrame(0.f, tMax, 1)
    {
        window = new float[2 * tMax_]; // Allocation of the array for the hamming window
        threshold_ = 0.1;              // Default threshold for pitch tracking

        dt = new float[tMax + 1];    // Allocation of the euclidian distance coefficient array.  The first one is never used.
        cumDt = new float[tMax + 1]; // Allocation of the cumulative sum array
        cumDt[0] = 0.;               // Initialization of the first coefficient of the cumulative sum
        dpt = new float[tMax + 1];   // Allocation of the pitch tracking function coefficient array
        dpt[0] = 1.;                 // Initialization of the first coefficient of dpt which is always the same

        // Initialisation of the input and output delay lines
        inputLine_.setMaximumDelay(3 * tMax_);
        // The delay is choosed such as the coefficients are not read before being finalised.
        outputLine_.setMaximumDelay(3 * tMax_);
        outputLine_.setDelay(3 * tMax_);

        // Initialization of the delay line of pitch tracking coefficients
        // coeffLine_ = new Delay[512];
        // for(int i=0;i<tMax_;i++)
        //	coeffLine_[i] = new Delay( tMax_, tMax_ );
    }

    ~MEAP_LentPitShift(void)
    {
        delete window;
        delete dt;
        delete dpt;
        delete cumDt;
    }

    //! Reset and clear all internal state.
    void clear(void)
    {
        inputLine_.clear();
        outputLine_.clear();
    };

    //! Set the pitch shift factor (1.0 produces no shift).
    void setShift(float shift)
    {
        if (shift <= 0.0f)
            periodRatio_ = 1.0;
        periodRatio_ = 1.0f / shift;
    };

    //! Input one sample to the filter and return one output.
    float next(float input)
    {
        float sample;

        inputFrames[ptrFrames] = input;

        sample = outputFrames[ptrFrames++];

        // Check for end condition
        if (ptrFrames == (int)inputFrames.size())
        {
            ptrFrames = 0;
            process();
        }

        return sample;
    };

protected:
    //! Apply the effect on the input samples and store it.
    /*!
      The samples stored in the input frame vector are processed
      and the delayed result are stored in the output frame vector.
    */
    void process()
    {
        float x_t;   // input coefficient
        float x_t_T; // previous input coefficient at T samples
        float coeff; // new coefficient for the difference function

        unsigned long alternativePitch = tMax_; // Global minimum storage
        lastPeriod_ = tMax_ + 1;                // Storage of the lowest local minimum under the threshold

        // Loop variables
        unsigned long delay_;
        unsigned int n;

        // Initialization of the dt coefficients.  Since the
        // frames are of tMax_ length, there is no overlapping
        // between the successive windows where pitch tracking
        // is performed.
        for (delay_ = 1; delay_ <= tMax_; delay_++)
            dt[delay_] = 0.;

        // Calculation of the dt coefficients and update of the input delay line.
        for (n = 0; n < inputFrames.size(); n++)
        {
            x_t = inputLine_.next(inputFrames[n]);
            for (delay_ = 1; delay_ <= tMax_; delay_++)
            {
                x_t_T = inputLine_.tapOut(delay_);
                coeff = x_t - x_t_T;
                dt[delay_] += coeff * coeff;
            }
        }

        // Calculation of the pitch tracking function and test for the minima.
        for (delay_ = 1; delay_ <= tMax_; delay_++)
        {
            cumDt[delay_] = dt[delay_] + cumDt[delay_ - 1];
            dpt[delay_] = dt[delay_] * delay_ / cumDt[delay_];

            // Look for a minimum
            if (dpt[delay_ - 1] - dpt[delay_ - 2] < 0 && dpt[delay_] - dpt[delay_ - 1] > 0)
            {
                // Check if the minimum is under the threshold
                if (dpt[delay_ - 1] < threshold_)
                {
                    lastPeriod_ = delay_ - 1;
                    // If a minimum is found, we can stop the loop
                    break;
                }
                else if (dpt[alternativePitch] > dpt[delay_ - 1])
                    // Otherwise we store it if it is the current global minimum
                    alternativePitch = delay_ - 1;
            }
        }

        // Test for the last period length.
        if (dpt[delay_] - dpt[delay_ - 1] < 0)
        {
            if (dpt[delay_] < threshold_)
                lastPeriod_ = delay_;
            else if (dpt[alternativePitch] > dpt[delay_])
                alternativePitch = delay_;
        }

        if (lastPeriod_ == tMax_ + 1)
            // No period has been under the threshold so we used the global minimum
            lastPeriod_ = alternativePitch;

        // We put the new zero output coefficients in the output delay line and
        // we get the previous calculated coefficients
        outputLine_.next(zeroFrame, outputFrames);

        // Initialization of the Hamming window used in the algorithm
        for (int n = -(int)lastPeriod_; n < (int)lastPeriod_; n++)
            window[n + lastPeriod_] = (1 + std::cos(STK_PI * n / lastPeriod_)) / 2;

        long M;       // Index of reading in the input delay line
        long N;       // Index of writing in the output delay line
        float sample; // Temporary storage for the new coefficient

        // We loop for all the frames of length lastPeriod_ presents between inputPtr and tMax_
        for (; inputPtr < (int)(tMax_ - lastPeriod_); inputPtr += lastPeriod_)
        {
            // Test for the decision of compression/expansion
            while (outputPtr < inputPtr)
            {
                // Coefficients for the linear interpolation
                env[1] = std::fmod(outputPtr + tMax_, 1.0);
                env[0] = 1.0f - env[1];
                M = tMax_ - inputPtr + lastPeriod_ - 1;                                         // New reading pointer
                N = 2 * tMax_ - (unsigned long)std::floor(outputPtr + tMax_) + lastPeriod_ - 1; // New writing pointer
                for (unsigned int j = 0; j < 2 * lastPeriod_; j++, M--, N--)
                {
                    sample = inputLine_.tapOut(M) * window[j] / 2.f;
                    // Linear interpolation
                    outputLine_.addTo(env[0] * sample, N);
                    outputLine_.addTo(env[1] * sample, N - 1);
                }
                outputPtr = outputPtr + lastPeriod_ * periodRatio_; // new output pointer
            }
        }
        // Shifting of the pointers waiting for the new frame of length tMax_.
        outputPtr -= tMax_;
        inputPtr -= tMax_;
    };

    // Frame storage vectors for process function
    StkFrames inputFrames;
    StkFrames outputFrames;
    int ptrFrames; // writing pointer

    // Input delay line
    MEAP_Delay<int16_t> inputLine_;
    int inputPtr;

    // Output delay line
    MEAP_Delay<int16_t> outputLine_;
    int64_t outputPtr;

    // Pitch tracker variables
    uint32_t tMax_; // Maximal period measurable by the pitch tracker.
    // It is also the size of the window used by the pitch tracker and
    // the size of the frames that can be computed by the next function

    float threshold_;     // Threshold of detection for the pitch tracker
    uint32_t lastPeriod_; // Result of the last pitch tracking loop
    float *dt;            // Array containing the euclidian distance coefficients
    float *cumDt;         // Array containing the cumulative sum of the coefficients in dt
    float *dpt;           // Array containing the pitch tracking function coefficients

    // Pitch shifter variables
    float env[2];        // Coefficients for the linear interpolation when modifying the output samples
    float *window;       // Hamming window used for the input portion extraction
    float periodRatio_;  // Ratio of modification of the signal period
    StkFrames zeroFrame; // Frame of tMax_ zero samples

    // Coefficient delay line that could be used for a dynamic calculation of the pitch
    // Delay* coeffLine_;
};

#endif // MEAP_LENTPITSHIFT_H
