/*
 * Ead.h
 *
 * Adapted from ead~.c puredata external (creb library)
 *
 * This file is part of Mozzi.
 *
 * Copyright (c) 2000-2003 by Tom Schouten
 * Copyright 2012 Tim Barrass
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef mEAD_H_
#define mEAD_H_

#include "math.h"
#include "mozzi_fixmath.h"

/** Exponential attack decay envelope. This produces a natural sounding
envelope. It calculates a new value each time next() is called, which can be
mapped to other parameters to change the amplitude or timbre of a sound.
@note Currently doesn't work at audio rate... may need larger number
types for Q8n8attack and Q8n8decay ?
*/

template <uint32_t UPDATE_RATE = CONTROL_RATE>
class mEad
{

public:
    /** Constructor
    @param update_rate
    Usually this will be MOZZI_CONTROL_RATE or MOZZI_AUDIO_RATE, unless you
    design another scheme for updating. One such alternative scheme could take turns
    for various control changes in a rotating schedule to spread out calculations
    made in successive updateControl() routines.
    */
    mEad()
    {
    }

    /** Set the attack time in milliseconds.
    @param attack_ms The time taken for values returned by successive calls of
    the next() method to change from 0 to 255.
    */
    inline void setAttack(unsigned int attack_ms)
    {
        // Q8n8attack = float_to_Q8n8(millisToOneMinusRealPole(attack_ms));
        attack_ = millisToOneMinusRealPole(attack_ms);
    }

    /** Set the decay time in milliseconds.
    @param decay_ms The time taken for values returned by successive calls of
    the next() method to change from 255 to 0.
    */
    inline void setDecay(unsigned int decay_ms)
    {
        // Q8n8decay = float_to_Q8n8(millisToOneMinusRealPole(decay_ms));
        decay_ = millisToOneMinusRealPole(decay_ms);
    }

    /** Set attack and decay times in milliseconds.
    @param attack_ms The time taken for values returned by successive calls of
    the next() method to change from 0 to 255.
    @param decay_ms The time taken for values returned by successive calls of
    the next() method to change from 255 to 0.
    */
    inline void set(unsigned int attack_ms, unsigned int decay_ms)
    {
        setAttack(attack_ms);
        setDecay(decay_ms);
    }

    /** Start the envelope from the beginning.
    This can be used at any time, even if the previous envelope is not finished.
    */
    inline void start()
    {
        // Q8n24state = 0;
        state_ = 0;
        attack_phase = true;
    }

    /** Set attack and decay times in milliseconds, and start the envelope from the beginning.
    This can be used at any time, even if the previous envelope is not finished.
    @param attack_ms The time taken for values returned by successive calls of
    the next() method to change from 0 to 255.
    @param decay_ms The time taken for values returned by successive calls of
    the next() method to change from 255 to 0.
    */
    inline void start(unsigned int attack_ms, unsigned int decay_ms)
    {
        set(attack_ms, decay_ms);
        // Q8n24state = 0; // don't restart from 0, just go from whatever the current level is, to avoid glitches
        attack_phase = true;
    }

    /** Calculate and return the next envelope value, in the range -128 to 127
    @note Timing: 5us
    */

    inline int32_t next()
    {
        if (attack_phase)
        {
            state_ += (1.0 - state_) * attack_;
            if (state_ >= 1.0)
            {
                state_ = 1.0;
                attack_phase = false;
            }
        }
        else
        { /* decay phase */
            state_ -= decay_ * state_;
        }
        return ((int)(state_ * 255));
    }

private:
    float attack_;
    float decay_;
    float state_;

    bool attack_phase;

    /* convert milliseconds to 1-p, with p a real pole */
    inline float millisToOneMinusRealPole(unsigned int milliseconds)
    {
        static const float NUMERATOR = 1000.0f * log(0.001f);
        return -expm1(NUMERATOR / ((float)UPDATE_RATE * milliseconds));
    }

    // Compute exp(x) - 1 without loss of precision for small values of x.
    inline float expm1(float x)
    {
        if (fabs(x) < 1e-5)
        {
            return x + 0.5 * x * x;
        }
        else
        {
            return exp(x) - 1.0;
        }
    }
};

/**
@example 07.Envelopes/Ead_Envelope/Ead_Envelope.ino
This is an example of how to use the Ead class.
*/

#endif /* EAD_H_ */