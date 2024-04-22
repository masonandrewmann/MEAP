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
template <unsigned int NUM_BUFFER_SAMPLES, class T = int8_t>
class MEAP_Delay
{
public:
  //! The default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  /*!

    maxDelay must be greater than zero
    delay must be greater than or equal to zero and less than maxDelay
   */
  MEAP_Delay(unsigned long delay = 0, unsigned long maxDelay = 4095);

  //! Class destructor.
  ~MEAP_Delay();

  //! Set the delay-line length.
  /*!
    The valid range for \e delay is from 0 to the maximum delay-line length.
  */
  void setDelay(unsigned long delay);

  //! Return the current delay-line length.
  unsigned long getDelay(void) const { return _delaytime_cells; };

  //! Return the value at \e tapDelay samples from the delay-line input.
  /*!
    The tap point is determined modulo the delay-line length and is
    relative to the last input value (i.e., a tapDelay of zero returns
    the last input value).
  */
  StkFloat tapOut(unsigned long tapDelay);

  //! Set the \e value at \e tapDelay samples from the delay-line input.
  void tapIn(T value, unsigned long tapDelay);

  //! Sum the provided \e value into the delay line at \e tapDelay samples from the input.
  /*!
    The new value is returned.  The tap point is determined modulo
    the delay-line length and is relative to the last input value
    (i.e., a tapDelay of zero sums into the last input value).
  */
  StkFloat addTo(T value, unsigned long tapDelay);

protected:
  // StkFrames outputs_;
  // StkFrames inputs_;

  T delay_buffer[NUM_BUFFER_SAMPLES];

  unsigned long _write_pos;
  unsigned long _read_pos;
  unsigned long _delaytime_cells = NUM_BUFFER_SAMPLES;
  unsigned long c = NUM_BUFFER_SAMPLES;
}

#endif // MEAP_DELAY_H
