#ifndef M_NOISE_H_
#define M_NOISE_H_

class mNoise
{
public:
    mNoise() {};

    //! Return the last computed output value.
    int16_t lastOut(void) const { return last_output_; };

    //! Compute and return one 16-bit signed (-32768 to 32767) output sample.
    int16_t next(void)
    {
        last_output_ = xorshift96() % 65535 - 32768;
        return last_output_;
    };

protected:
    int16_t last_output_;
};

#endif // M_NOISE_H_
