#ifndef MEAP_INSTRUMENT_H_
#define MEAP_INSTRUMENT_H_

class mInstrument
{
public:
  //! Class constructor.
  mInstrument(void){};

  //! Reset and clear all internal state (for subclasses).
  /*!
    Not all subclasses implement a clear() function.
  */
  virtual void clear(void) {};

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(float frequency, uint8_t amplitude) = 0;

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(uint8_t amplitude);

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(float frequency);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(uint8_t number, uint8_t value);

  //! Compute one sample frame and return the specified \c channel value.
  /*!
    For monophonic instruments, the \c channel argument is ignored.
  */
  virtual int16_t next();
};

#endif // MEAP_INSTRUMENT_H_
