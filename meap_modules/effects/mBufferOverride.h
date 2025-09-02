// an IIR comb filter with a linearly interpolated delay line

#ifndef MBUFFER_OVERRRIDE_H_
#define MBUFFER_OVERRRIDE_H_

#include <math.h>

template <class T = int32_t>
class mBufferOverride
{
public:
    mBufferOverride(uint32_t forced_buffer_size_, float buffer_divisor_, float mix_ = 0.5, uint32_t max_buffer_size_ = 32768)
    {
        max_buffer_size = max_buffer_size_;
        record_buffer = (T *)calloc(max_buffer_size, sizeof(T));
        forced_buffer_size = forced_buffer_size_;
        buffer_divisor = buffer_divisor_;
        divisor_length = (float)forced_buffer_size_ / buffer_divisor;
        short_accumulator = 0;
        long_accumulator = 0;
        record_accumulator = 0;
        divisor_start = 0;
        mix = mix_;
    };

    ~mBufferOverride()
    {
        free(record_buffer);
    };

    // divisions of forced_buffer_size
    void setDivisor(float divisor_)
    {
        buffer_divisor = divisor_;
    }

    // length of forced_buffer_size (1 to max_buffer_size)
    void setBufferSize(uint32_t size)
    {
        forced_buffer_size = size;
    }

    void setDelay(float delay_samples)
    {
        // delay_samples_ = delay_samples;
        // read_pointer_ = (float)write_pointer_ - delay_samples;
        // if (read_pointer_ < 0)
        // {
        //     read_pointer_ += max_delay_samples_;
        // }
    }

    // sets delay as a number of milliseconds
    int32_t setDelayMS(float ms)
    {
        // setDelay(ms * (float)AUDIO_RATE * 0.001);
        // return delay_samples_;
    }

    // sets delay as a number of sixteenth notes at a tempo
    // returns delay time in samples
    int32_t setDelaySixteenths(uint16_t num_sixteenths, uint16_t bpm)
    {
        // setDelay(num_sixteenths * (15.f / (float)bpm) * ((float)AUDIO_RATE));
        // return delay_samples_;
    }

    void setMix(float mix_)
    {
        mix = mix_;
    }

    void clear()
    {
        // for (int i = max_delay_samples_; --i >= 0;)
        // {
        //     delay_buffer_[i] = 0;
        // }
    }

    T next(T in_sample)
    {
        // RECORDING
        record_buffer[record_accumulator] = in_sample;
        record_accumulator++;
        if (record_accumulator >= max_buffer_size)
        {
            record_accumulator = 0;
        }

        // PLAYBACK
        // T out_sample = in_sample;
        T out_sample = record_buffer[(int)short_accumulator];
        short_accumulator++;
        if (short_accumulator >= max_buffer_size)
        {
            short_accumulator -= max_buffer_size;
        }
        divisor_counter++;
        if (divisor_counter >= divisor_length)
        {
            short_accumulator = divisor_start;
            divisor_counter = 0;
        }

        long_accumulator++;
        // reached end of forced buffer, find new place to put short buffer
        if (long_accumulator >= forced_buffer_size)
        {
            long_accumulator = 0;

            divisor_length = (float)forced_buffer_size / buffer_divisor;
            divisor_start = record_accumulator;
        }

        return (out_sample * mix + (1 - mix) * in_sample);
    }

    // CLASS VARIABLES

    T *record_buffer;
    int32_t max_buffer_size;
    int32_t forced_buffer_size;
    int32_t divisor_length;
    int32_t divisor_start;
    int32_t divisor_counter;
    float buffer_divisor;
    float mix;

    int32_t short_accumulator;  // for passing through divisor buffers
    int32_t long_accumulator;   // for passing through forced buffer
    int32_t record_accumulator; // for recording to record buffer
};

#endif // MDIGITALDELAY_H_