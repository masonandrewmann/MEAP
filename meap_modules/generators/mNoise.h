#ifndef M_NOISE_H_
#define M_NOISE_H_

// pink noise implementation from https://www.firstpr.com.au/dsp/pink-noise/#Filtering

class mNoise
{
public:
    mNoise() {
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

    // coefficients for sr=32768 calculated using matlab script at http://www.cooperbaker.com/home/code/pink%20noise/
    int16_t pink()
    {
        white_sample = white();

        filter_states[0] = 0.000244094 * white_sample + (0.999755906) * filter_states[0];
        filter_states[1] = 0.000976375 * white_sample + (0.999023625) * filter_states[1];
        filter_states[2] = 0.003905501 * white_sample + (0.996094499) * filter_states[2];
        filter_states[3] = 0.015622004 * white_sample + (0.984377996) * filter_states[3];
        filter_states[4] = 0.062488016 * white_sample + (0.937511984) * filter_states[4];
        filter_states[5] = 0.249952063 * white_sample + (0.750047937) * filter_states[5];
        filter_states[6] = 0.999808252 * white_sample + (0.000191748) * filter_states[6];

        pink_sample = (filter_states[0] +
                       filter_states[1] * 0.501187234 +
                       filter_states[2] * 0.251188643 +
                       filter_states[3] * 0.125892541 +
                       filter_states[4] * 0.063095734 +
                       filter_states[5] * 0.031622777 +
                       filter_states[6] * 0.015848932) *
                      5;
        return pink_sample;
    }
    int16_t white_sample, pink_sample;
    int16_t filter_states[7];
};

#endif // M_NOISE_H_
