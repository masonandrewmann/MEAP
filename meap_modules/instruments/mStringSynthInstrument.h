#ifndef MSTRING_SYNTH_INSTRUMENT_H_
#define MSTRING_SYNTH_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>

#include "mInstrument.h"
#include "mStringSynthVoice.h"

// Polyphic string synth, inspired by ARP Solina

template <uint16_t mPOLYPHONY = 4>
class mStringSynthInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mStringSynthInstrument(uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].init();
        }
    }

    void setADSR(uint32_t attack_time, uint32_t decay_time, uint16_t sustain_level, uint32_t release_time)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setADSR(attack_time, decay_time, sustain_level, release_time);
        }
    }

    void setCutoffFreqAndResonance(uint8_t cutoff, uint8_t resonance)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setCutoffFreqAndResonance(cutoff, resonance);
        }
    }

    void setDetune(float detune)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i].setDetune(detune);
        }
    }

    // void setEnsembleMix(uint32_t mix)
    // {
    //     for (int i = mPOLYPHONY; --i >= 0;)
    //     {
    //         voices[i].setEnsembleMix(mix);
    //     }
    // }

    // void setPhaserFreq(float phaser_freq)
    // {
    //     for (int i = mPOLYPHONY; --i >= 0;)
    //     {
    //         voices[i].setPhaserFreq(phaser_freq);
    //     }
    // }

    void midiStop()
    {
        mInstrument<mPOLYPHONY>::midiStop();
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
        }
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        mInstrument<mPOLYPHONY>::noteOnVoiceHandler(note, vel);
        voices[this->_curr_voice].noteOn(note, vel);
    }

    void noteOff(uint16_t note)
    {
        int16_t off_voice = mInstrument<mPOLYPHONY>::noteOffVoiceHandler(note);
        if (off_voice != -1)
        {
            voices[off_voice].noteOff();
        }
    }

    void flush()
    {
        mInstrument<mPOLYPHONY>::flush();
        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].noteOff();
        }
    }

    void update()
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i].update();
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            out_sample += voices[i].next();
        }

        return out_sample;
    }

    // CLASS VARIABLES
    mStringSynthVoice voices[mPOLYPHONY];
};

#endif // MSTRING_SYNTH_INSTRUMENT_H_
