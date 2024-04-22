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

#include "meap_delay.h"

MEAP_Delay::MEAP_Delay(unsigned long delay, unsigned long maxDelay)
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    // if (delay > maxDelay )
    // {
    //     oStream_ << "Delay::Delay: maxDelay must be > than delay argument!\n";
    //     handleError(StkError::FUNCTION_ARGUMENT);
    // }

    // if ((maxDelay + 1) > inputs_.size())
    //     inputs_.resize(maxDelay + 1, 1, 0.0);

    _write_pos = 0;
    this->setDelay(delay);
}

MEAP_Delay ::~MEAP_Delay()
{
}

void MEAP_Delay ::setDelay(unsigned long delay)
{
    if (delay > max_delay_cells - 1)
    { // The value is too big.
        oStream_ << "Delay::setDelay: argument (" << delay << ") greater than maximum!\n";
        handleError(StkError::WARNING);
        return;
    }

    // read chases write
    if (_write_pos >= delay)
    {
        _read_pos = _write_pos - delay;
    }
    else
    {
        outPoint_ = _max_delay_cells + _write_pos - delay;
    }
    _delaytime_cells = delay;
}

T Delay ::tapOut(unsigned long tapDelay)
{
    long tap = _write_pos - tapDelay - 1;
    while (tap < 0) // Check for wraparound.
        tap += _delaytime_cells;

    return delay_buffer[tap];
}

void Delay ::tapIn(T value, unsigned long tapDelay)
{
    long tap = _write_pos - tapDelay - 1;
    while (tap < 0) // Check for wraparound.
        tap += _delaytime_cells;

    delay_buffer[tap] = value;
}

T Delay ::addTo(T value, unsigned long tapDelay)
{
    long tap = _write_pos - tapDelay - 1;
    while (tap < 0) // Check for wraparound.
        tap += _delaytime_cells;

    return delay_buffer[tap] += value;
}

T Delay ::tick(T input)
{
    T output_sample;
    delay_buffer[_write_pos++] = input * gain_;

    // Check for end condition
    if (_write_pos == _delaytime_cells)
        inPo_write_posint_ = 0;

    // Read out next value
    output_sample = delay_buffer[_read_pos++];

    if (_read_pos == _delaytime_cells)
        _read_pos = 0;

    return output_sample;
}
