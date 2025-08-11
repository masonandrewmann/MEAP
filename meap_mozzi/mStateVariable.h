
/*
 * StateVariable.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef MSTATEVARIABLE_H_
#define MSTATEVARIABLE_H_

#include "Arduino.h"
#include "math.h"
#include "meta.h"
#include "mozzi_fixmath.h"
#include "mozzi_utils.h"
#include "ResonantFilter.h"

// enum filter_types { LOWPASS, BANDPASS, HIGHPASS, NOTCH };

class mStateVariable
{

public:
    mStateVariable() {}

    void setResonance(Q0n8 resonance)
    {
        // qvalue goes from 255 to 0, representing .999 to 0 in fixed point
        // lower q, more resonance
        q = resonance;
        scale = (Q0n8)sqrt((unsigned int)resonance << 8);
    }

    void setCentreFreq(unsigned int centre_freq)
    {
        // simple frequency tuning with error towards nyquist (reference?  where did
        // this come from?)
        // f = (Q1n15)(((Q16n16_2PI*centre_freq)>>AUDIO_RATE_AS_LSHIFT)>>1);
        f = (Q15n16)((Q16n16_2PI * centre_freq) >>
                     (AUDIO_RATE_AS_LSHIFT)); // this works best for now
                                              // f = (Q15n16)(((Q16n16_2PI*centre_freq)<<(16-AUDIO_RATE_AS_LSHIFT))>>16);
                                              // // a small shift left and a round 16 right is faster than big
                                              // non-byte-aligned right in one go float ff =
                                              // Q16n16_to_float(((Q16n16_PI*centre_freq))>>AUDIO_RATE_AS_LSHIFT); f =
                                              // float_to_Q15n16(2.0f *sin(ff));
    }

    int next(int input)
    {
        l += ((f * b) >> 16);
        h = (((long)input - l - (((long)b * q) >> 8)) * scale) >> 8;
        b += ((f * h) >> 16);
    }

    int low()
    {
        return l;
    }

    int high()
    {
        return h;
    }

    int band()
    {
        return b;
    }

    int notch()
    {
        return (h + l);
    }

    int l, b, h;
    Q0n8 q, scale;
    volatile Q15n16 f;
};

#endif /* MSTATEVARIABLE_H_ */