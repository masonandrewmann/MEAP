#ifndef MEAP_CONTROLSINE_H
#define MEAP_CONTROLSINE_H

const uint32_t MEAP_CTRL_SINE_TABLE_SIZE = 2048;

/***************************************************/
/*! \class stk::SineWave
    \brief STK sinusoid oscillator class.

    This class computes and saves a static sine "table" that can be
    shared by multiple instances.  It has an interface similar to the
    WaveLoop class but inherits from the Generator class.  Output
    values are computed using linear interpolation.

    The "table" length, set in SineWave.h, is 2048 samples by default.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

class MEAP_ControlSine
{
public:
    //! Default constructor.
    MEAP_ControlSine(void) : time_(0.0), rate_(1.0), phaseOffset_(0.0)
    {
        table_ = (float *)malloc((MEAP_CTRL_SINE_TABLE_SIZE + 1) * sizeof(float));
        float temp = 1.0 / (float)MEAP_CTRL_SINE_TABLE_SIZE;
        for (uint32_t i = 0; i <= MEAP_CTRL_SINE_TABLE_SIZE; i++)
            table_[i] = sin(6.2831853072 * i * temp);
    }

    //! Class destructor.
    ~MEAP_ControlSine(void){};

    //! Clear output and reset time pointer to zero.
    void reset(void)
    {
        time_ = 0.0;
        last_output_ = 0;
    };

    //! Set the data read rate in samples.  The rate can be negative.
    /*!
      If the rate value is negative, the data is read in reverse order.
    */
    void setRate(float rate) { rate_ = rate; };

    //! Set the data interpolation rate based on a looping frequency.
    /*!
      This function determines the interpolation rate based on the file
      size and the current Stk::sampleRate.  The \e frequency value
      corresponds to file cycles per second.  The frequency can be
      negative, in which case the loop is read in reverse order.
     */
    void setFrequency(float frequency)
    {
        // This is a looping frequency.
        this->setRate(MEAP_CTRL_SINE_TABLE_SIZE * frequency / CONTROL_RATE);
    };

    //! Increment the read pointer by \e time in samples, modulo the table size.
    void addTime(float time)
    {
        // Add an absolute time in samples.
        time_ += time;
    };

    //! Increment the read pointer by a normalized \e phase value.
    /*!
      This function increments the read pointer by a normalized phase
      value, such that \e phase = 1.0 corresponds to a 360 degree phase
      shift.  Positive or negative values are possible.
     */
    void addPhase(float phase)
    {
        // Add a time in cycles (one cycle = MEAP_CTRL_SINE_TABLE_SIZE).
        time_ += MEAP_CTRL_SINE_TABLE_SIZE * phase;
    };

    //! Add a normalized phase offset to the read pointer.
    /*!
      A \e phaseOffset = 1.0 corresponds to a 360 degree phase
      offset.  Positive or negative values are possible.
     */
    void addPhaseOffset(float phaseOffset)
    {
        // Add a phase offset relative to any previous offset value.
        time_ += (phaseOffset - phaseOffset_) * MEAP_CTRL_SINE_TABLE_SIZE;
        phaseOffset_ = phaseOffset;
    };

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    //! Compute and return one output sample.
    float next(void)
    {

        // Check limits of time address ... if necessary, recalculate modulo
        // MEAP_CTRL_SINE_TABLE_SIZE.
        while (time_ < 0.0f)
            time_ += MEAP_CTRL_SINE_TABLE_SIZE;
        while (time_ >= MEAP_CTRL_SINE_TABLE_SIZE)
            time_ -= MEAP_CTRL_SINE_TABLE_SIZE;

        iIndex_ = (uint16_t)time_;
        alpha_ = time_ - iIndex_;
        float tmp = table_[iIndex_];
        tmp += (alpha_ * (table_[iIndex_ + 1] - tmp));

        // Increment time, which can be negative.
        time_ += rate_;

        last_output_ = tmp;
        return last_output_;
    };

protected:
    float *table_;
    float time_;
    float rate_;
    float phaseOffset_;
    unsigned int iIndex_;
    float alpha_;
    float last_output_;
};

#endif // MEAP_CONTROLSINE_H
