#ifndef M_NOISE_H_
#define M_NOISE_H_

// pink noise implementation from https://www.firstpr.com.au/dsp/pink-noise/#Filtering

class mNoise
{
public:
    mNoise()
    {
        b0 = 0;
        b1 = 0;
        b2 = 0;
        b3 = 0;
        b4 = 0;
        b5 = 0;
        b6 = 0;
    };

    //! Compute and return one 16-bit signed (-32768 to 32767) output sample.
    int16_t white()
    {
        white_sample = xorshift96() % 65535 - 32768;
        return white_sample;
    };

    int16_t next()
    {
        return white();
    }

    int16_t pink()
    {
        white_sample = white();
        b0 = 0.99886 * b0 + white_sample * 0.0555179;
        b1 = 0.99332 * b1 + white_sample * 0.0750759;
        b2 = 0.96900 * b2 + white_sample * 0.1538520;
        b3 = 0.86650 * b3 + white_sample * 0.3104856;
        b4 = 0.55000 * b4 + white_sample * 0.5329522;
        b5 = -0.7616 * b5 - white_sample * 0.0168980;
        pink_sample = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white_sample * 0.5362;
        b6 = white_sample * 0.115926;
        return pink_sample;
    }
    int16_t b0, b1, b2, b3, b4, b5, b6, tmp, white_sample, pink_sample;
};

#endif // M_NOISE_H_
