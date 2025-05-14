#ifndef MMARIMBAVOICE_H_
#define MMARIMBAVOICE_H_

#include <meap_modules/filters/mRingz.h>
#include <meap_modules/generators/mNoise.h>
#include <meap_modules/building_blocks/mEnvelope.h>

class mMarimbaVoice
{
public:
    mMarimbaVoice()
    {
        init();
    };

    void init()
    {

        for (int i = 0; i < 8; i++)
        {
            modes[i].init();
            modes[i].updateParameters(ratios[i], times[i]);
        }

        env.setTimes(1, 50, 1, 1);
        env.setADLevels(255, 0);
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        float freq = mtof(note);
        for (int i = 8; --i >= 0;)
        {
            modes[i].updateParameters(freq * ratios[i], times[i]);
        }
        env.noteOn();
    }

    void noteOff()
    {
    }

    // to be called in audio loop
    int32_t next()
    {
        int32_t out_sample = 0;
        env.update();
        int32_t exciter = (env.next() * noise.next()) >> 8;
        for (int i = 8; --i >= 0;)
        {
            out_sample += modes[i].next(exciter) * amps[i];
        }
        return out_sample >> 11;
    }

    // CLASS VARIABLES
    float ratios[8] = {1, 2, 2.803, 3.871, 5.074, 7.81, 10.948, 14.421};
    float amps[8] = {1, 0.044, 0.891, 0.0891, 0.794, 0.1, 0.281, 0.079};
    float times[8] = {1, 0.205, 1, 0.196, 0.339, 0.047, 0.058, 0.047};
    mRingz modes[8];
    mNoise noise;
    ADSR<AUDIO_RATE, AUDIO_RATE> env;
};

#endif // MMARIMBAVOICE_H_