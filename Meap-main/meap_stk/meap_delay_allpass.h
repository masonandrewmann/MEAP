#ifndef MEAP_DELAY_ALLPASS_H
#define MEAP_DELAY_ALLPASS_H

/***************************************************/
/*! \class MEAP_DELAY_ALLPASS_H
    \brief STK allpass interpolating delay line class.

    Based on STK Delay implementations



        This class implements a fractional-length digital delay-line using
        a first-order allpass filter.  If the delay and maximum length are
        not specified during instantiation, a fixed maximum length of 4095
        and a delay of 0.5 is set.

        An allpass filter has unity magnitude gain but variable phase
        delay properties, making it useful in achieving fractional delays
        without affecting a signal's frequency magnitude response.  In
        order to achieve a maximally flat phase delay response, the
        minimum delay possible in this implementation is limited to a
        value of 0.5.

        by Perry R. Cook and Gary P. Scavone, 1995--2019.

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
class MEAP_Delay_Allpass
{
public:
    //! The default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
    /*!

      maxDelay must be greater than zero
      delay must be greater than or equal to zero and less than maxDelay
     */
    MEAP_Delay_Allpass(float delay = 0.5, int max_delay = 4095)
    {
        max_delay_samples_ = max_delay;
        write_pos_ = 0;
        ap_input_ = 0.0;
        delay_buffer_ = (T *)malloc(max_delay_samples_ * sizeof(T));

        // this->clear();
        this->setDelay(delay);
    };

    //! Class destructor.
    ~MEAP_Delay_Allpass()
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
        delay_time_ = delay;
        float out_pointer = write_pos_ - delay_time_ + 1.0f; // read chases write

        while (out_pointer < 0)
            out_pointer += max_delay_samples_; // modulo maximum length

        read_pos_ = (uint32_t)out_pointer; // integer part

        if (read_pos_ == max_delay_samples_)
            read_pos_ = 0;

        alpha_ = 1.0f + read_pos_ - out_pointer; // fractional part

        if (alpha_ < 0.5f)
        {
            // The optimal range for alpha is about 0.5 - 1.5 in order to
            // achieve the flattest phase delay response.
            read_pos_ += 1;
            if (read_pos_ >= max_delay_samples_)
            {
                read_pos_ -= max_delay_samples_;
            }
            alpha_ += 1.0f;
        }

        coeff_ = (1.0f - alpha_) / (1.0f + alpha_); // coefficient for allpass
    };

    //! Return the current delay-line length.
    float getDelay(void) const { return delay_time_; };

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

    T nextOut(void)
    {
        // Do allpass interpolation delay.
        T next_output_ = -coeff_ * last_output_;
        next_output_ += ap_input_ + (coeff_ * delay_buffer_[read_pos_]);

        return next_output_;
    }

    //! Return the last computed output value.
    T lastOut(void) const { return last_output_; };

    T next(T input)
    {
        delay_buffer_[write_pos_++] = input;

        // Increment input pointer modulo length.
        if (write_pos_ >= max_delay_samples_)
            write_pos_ = 0;

        last_output_ = nextOut();

        // Save the allpass input and increment modulo length.
        ap_input_ = delay_buffer_[read_pos_++];
        if (read_pos_ >= max_delay_samples_)
            read_pos_ = 0;

        return last_output_;
    }

    void clear()
    {
        for (uint32_t i = 0; i < max_delay_samples_; i++)
            delay_buffer_[i] = 0;
        last_output_ = 0.0;
        ap_input_ = 0.0;
    }

protected:
    T *delay_buffer_;

    uint32_t max_delay_samples_; // max delay length in sasmples

    uint32_t write_pos_ = 0; // index of write pointer in delay_buffer_
    uint32_t read_pos_ = 0;  // index of read pointer in delay_buffer_

    uint32_t delaytime_samples_ = 0; // number of samples to delay

    float delay_time_ = 0; // desired delay time as float
    float alpha_;
    float coeff_;

    T last_output_;
    T ap_input_;
};

#endif // MEAP_DELAY_ALLPASS_H
