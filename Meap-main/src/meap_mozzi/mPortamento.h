
#ifndef MPORTAMENTO_H_
#define MPORTAMENTO_H_

#include "mozzi_midi.h"
#include "mozzi_fixmath.h"
#include "Line.h"

template <unsigned int CONTROL_UPDATE_RATE>
class
    mPortamento
{

public:
    mPortamento() : MICROS_PER_CONTROL_STEP(1000000 / CONTROL_UPDATE_RATE)
    {
    }

    inline void setTime(unsigned int milliseconds)
    {
        // control_steps_per_portamento = ((long)milliseconds*1000)/MICROS_PER_CONTROL_STEP; // more accurate but slower
        control_steps_per_portamento = convertMsecToControlSteps(milliseconds);
    }

    inline void start(uint8_t note)
    {
        target_freq = Q16n16_mtof(Q8n0_to_Q16n16(note));
        aPortamentoLine.set(target_freq, control_steps_per_portamento);
        countdown = control_steps_per_portamento;
        portamento_on = true;
    }

    inline void start(Q16n16 note)
    {
        target_freq = Q16n16_mtof(note);
        aPortamentoLine.set(target_freq, control_steps_per_portamento);
        countdown = control_steps_per_portamento;
        portamento_on = true;
    }

    inline void start(float note)
    {
        target_freq = float_to_Q16n16(mtof(note));
        aPortamentoLine.set(target_freq, control_steps_per_portamento);
        countdown = control_steps_per_portamento;
        portamento_on = true;
    }

    inline void startFreq(float freq)
    {
        target_freq = float_to_Q16n16(freq);
        aPortamentoLine.set(target_freq, control_steps_per_portamento);
        countdown = control_steps_per_portamento;
        portamento_on = true;
    }

    inline Q16n16 next()
    {
        if (portamento_on == true)
        {
            if (--countdown < 0)
            {
                // stay level when portamento has finished
                aPortamentoLine.set(target_freq, target_freq, control_steps_per_portamento);
                portamento_on = false;
            }
        }
        return aPortamentoLine.next();
    }

    int countdown;
    int control_steps_per_portamento;
    Q16n16 target_freq;
    bool portamento_on;
    const unsigned int MICROS_PER_CONTROL_STEP;
    Line<Q16n16> aPortamentoLine;

    // copied from ADSR.h
    inline static const unsigned int convertMsecToControlSteps(unsigned int msec)
    {
        return (uint16_t)(((uint32_t)msec * CONTROL_UPDATE_RATE) >> 10); // approximate /1000 with shift
    }
};

#endif /* MPORTAMENTO_H_ */
