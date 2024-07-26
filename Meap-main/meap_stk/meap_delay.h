#ifndef MEAP_DELAY_H
#define MEAP_DELAY_H

/***************************************************/
/*! \class MEAP_Delay
    \brief MEAP non-interpolating delay line class.

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
class MEAP_Delay
{
public:
  //! The default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  /*!

    maxDelay must be greater than zero
    delay must be greater than or equal to zero and less than maxDelay
   */
  MEAP_Delay(uint32_t delay = 0, uint32_t max_delay = 4095)
  {
    max_delay_samples_ = max_delay;
    write_pos_ = 0;
    delay_buffer_ = (T *)malloc(max_delay_samples_ * sizeof(T));
    this->setDelay(delay);
    // this->clear();
  };

  //! Class destructor.
  ~MEAP_Delay()
  {
    free(delay_buffer_);
  };

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
  void setDelay(uint32_t delay)
  {

    // read chases write
    if (write_pos_ >= delay)
    {
      read_pos_ = write_pos_ - delay;
    }
    else
    {
      read_pos_ = max_delay_samples_ + write_pos_ - delay;
    }
    delaytime_samples_ = delay;
  };

  //! Return the current delay-line length.
  uint32_t getDelay(void) const { return delaytime_samples_; };

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

  //! Return the value that will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  T nextOut(void) { return delay_buffer_[read_pos_]; };

  T next(T input)
  {
    delay_buffer_[write_pos_++] = input;

    // Check for end condition
    if (write_pos_ >= max_delay_samples_)
      write_pos_ = 0;

    // Read out next value
    last_output_ = delay_buffer_[read_pos_++];

    if (read_pos_ >= max_delay_samples_)
      read_pos_ = 0;

    return last_output_;
  }

  void clear()
  {
    for (uint32_t i = 0; i < max_delay_samples_; i++)
      delay_buffer_[i] = 0;
  }

protected:
  T *delay_buffer_;

  uint32_t max_delay_samples_; // max delay length in sasmples

  uint32_t write_pos_ = 0;         // index of write pointer in delay_buffer_
  uint32_t read_pos_ = 0;          // index of read pointer in delay_buffer_
  uint32_t delaytime_samples_ = 0; // number of samples to delay

  T last_output_;
};

#endif // MEAP_DELAY_H
