#ifndef MEAP_DELAY_LINEAR_H
#define MEAP_DELAY_LINEAR_H

/***************************************************/
/*! \class MEAP_DELAY_LINEAR_H
    \brief MEAP linearly interpolating delay line class.

    Based on STK Delay implementations

    This class implements a non-interpolating digital delay-line.  If
    the delay and maximum length are not specified during
    instantiation, a fixed maximum length of 4095 and a delay of zero
    is set.

    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.

Audio delay line for comb filter, flange, chorus and short echo effects.
 @tparam  NUM_BUFFER_SAMPLES is the length of the delay buffer in samples.  This should
 be a power of two. The largest delay you'll fit in an atmega328 will be 512
 cells, which at 16384 Hz sample rate is 31 milliseconds. More of a flanger or a
 doubler than an echo. The amount of memory available for delays on other chips will vary.
 AudioDelay() doesn't have feedback.  If you want feedback, use AudioDelayFeedback().
 @tparam the type of numbers to use for the signal in the delay.  The default is int8_t, but int could be useful
 when adding manual feedback.  When using int, the input should be limited to 15 bits width, ie. -16384 to 16383.
 */
/***************************************************/
template <class T = int16_t>
class MEAP_Delay_Linear
{
public:
    //! The default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
    /*!

      maxDelay must be greater than zero
      delay must be greater than or equal to zero and less than maxDelay
     */
    MEAP_Delay_Linear(float delay = 0.5, int max_delay = 4095)
    {
        max_delay_samples_ = max_delay;
        write_pos_ = 0;
        delay_buffer_ = (T *)malloc(max_delay_samples_ * sizeof(T));
        this->setDelay(delay);
        // this->clear();
    };

    //! Class destructor.
    ~MEAP_Delay_Linear()
    {
        free(delay_buffer_);
    };

    //! Set the maximum delay-line length.
    /*!
      This method should generally only be used during initial setup
      of the delay line.  If it is used between calls to the tick()
      function, without a call to clear(), a signal discontinuity will
      likely occur.  If the current maximum length is greater than the
      new length, no memory allocation change is made.
    */
    void setMaximumDelay(uint32_t delay)
    {
        if (delay == max_delay_samples_)
            return;
        max_delay_samples_ = delay;
        delay_buffer_ = (T *)realloc(delay_buffer_, delay * sizeof(T));
    }

    //! Get the maximum delay-line length.
    uint32_t getMaximumDelay(void) { return max_delay_samples_ - 1; };

    //! Set the delay-line length.
    /*!
      The valid range for \e delay is from 0 to the maximum delay-line length.
    */
    void setDelay(float delay)
    {
        _delay_time = delay;
        float out_pointer = write_pos_ - _delay_time; // read chases write

        while (out_pointer < 0)
            out_pointer += max_delay_samples_; // modulo maximum length

        read_pos_ = (uint32_t)out_pointer; // integer part

        alpha_ = out_pointer - read_pos_; // fractional part
        om_alpha_ = (float)1.0 - alpha_;

        if (read_pos_ >= max_delay_samples_)
            read_pos_ = 0;
    };

    //! Return the current delay-line length.
    float getDelay(void) const { return _delay_time; };

    //! Return the value at \e tapDelay samples from the delay-line input.
    /*!
      The tap point is determined modulo the delay-line length and is
      relative to the last input value (i.e., a tapDelay of zero returns
      the last input value).
    */
    T tapOut(uint32_t tapDelay)
    {
        int32_t tap = write_pos_ - tapDelay - 1;
        while (tap < 0) // Check for wraparound.
            tap += delaytime_samples_;

        return delay_buffer_[tap];
    }

    //! Set the \e value at \e tapDelay samples from the delay-line input.
    void tapIn(T value, uint32_t tapDelay)
    {
        int32_t tap = write_pos_ - tapDelay - 1;
        while (tap < 0) // Check for wraparound.
            tap += delaytime_samples_;

        delay_buffer_[tap] = value;
    }

    //! Sum the provided \e value into the delay line at \e tapDelay samples from the input.
    /*!
      The new value is returned.  The tap point is determined modulo
      the delay-line length and is relative to the last input value
      (i.e., a tapDelay of zero sums into the last input value).
    */
    T addTo(T value, uint32_t tapDelay)
    {
        int32_t tap = write_pos_ - tapDelay - 1;
        while (tap < 0) // Check for wraparound.
            tap += delaytime_samples_;

        return delay_buffer_[tap] += value;
    }
    //! Return the last computed output value.
    T lastOut(void) const { return last_output_; };

    T nextOut(void)
    {
        // First 1/2 of interpolation
        last_output_ = delay_buffer_[read_pos_] * om_alpha_;
        // Second 1/2 of interpolation
        if (read_pos_ + 1 < max_delay_samples_)
            last_output_ += delay_buffer_[read_pos_ + 1] * alpha_;
        else
            last_output_ += delay_buffer_[0] * alpha_;

        return last_output_;
    }

    T next(T input)
    {
        T output_sample;
        delay_buffer_[write_pos_++] = input;

        // Increment input pointer modulo length.
        if (write_pos_ >= max_delay_samples_)
            write_pos_ = 0;

        output_sample = nextOut();

        // Increment output pointer modulo length.
        if (++read_pos_ >= max_delay_samples_)
            read_pos_ = 0;

        return output_sample;
    }

    void clear()
    {
        for (uint32_t i = 0; i < max_delay_samples_; i++)
            delay_buffer_[i] = 0;
    }

protected:
    T *delay_buffer_;

    uint32_t max_delay_samples_; // max delay length in sasmples

    uint32_t write_pos_ = 0; // index of write pointer in delay_buffer_
    uint32_t read_pos_ = 0;  // index of read pointer in delay_buffer_

    uint32_t delaytime_samples_ = 0; // number of samples to delay

    float _delay_time = 0; // desired delay time as float
    float alpha_;          // lower side interpolation
    float om_alpha_;       // upper side interpolation
    T last_output_;        // output sample
};

#endif // MEAP_DELAY_LINEAR_H
