#ifndef MANALOG_INSTRUMENT_H_
#define MANALOG_INSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>
#include "mInstrument.h"
#include "mAnalogVoice.h"

// A polyphonic physical modeling based marimba instrument

template <uint16_t mPOLYPHONY = 4>
class mAnalogInstrument : public mInstrument<mPOLYPHONY>
{
public:
    mAnalogInstrument(uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i] = new mAnalogVoice;
        }
    };

    void midiStop()
    {
        mInstrument<mPOLYPHONY>::midiStop();
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            voices[i]->noteOff();
        }
    }

    void noteOn(uint16_t note, uint16_t vel)
    {
        mInstrument<mPOLYPHONY>::noteOnVoiceHandler(note, vel);
        voices[this->_curr_voice]->noteOn(note, vel);
    }

    void noteOff(uint16_t note)
    {
        int16_t off_voice = mInstrument<mPOLYPHONY>::noteOffVoiceHandler(note);
        if (off_voice != -1)
        {
            voices[off_voice]->noteOff();
        }
    }

    void flush()
    {
        mInstrument<mPOLYPHONY>::flush();
        for (int i = 0; i < mPOLYPHONY; i++)
        {
            voices[i]->noteOff();
        }
    }

    void setFilterParameters(int32_t cutoff_hz, float resonance, int voice_num = 0)
    {
        for (int i = 0; i < mPOLYPHONY; i++)
        {
            voices[i]->setFilterParameters(cutoff_hz, resonance, voice_num);
        }
    }

    void setFilterType(AnalogFilterTypes filter_type, int voice_num = 0)
    {
        for (int i = 0; i < mPOLYPHONY; i++)
        {
            voices[i]->setFilterType(filter_type, voice_num);
        }
    }

    void setFilterEnvAmt(float cutoff_amt, float res_amt, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setFilterEnvAmt(cutoff_amt, res_amt, voice_num);
        }
    }

    void setOscOctave(float detune, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setOscOctave(detune, voice_num);
        }
    }

    void setOscTranspose(float detune, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setOscTranspose(detune, voice_num);
        }
    }

    void setOscDetune(float detune, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setOscDetune(detune, voice_num);
        }
    }

    void setOscWavetable(AnalogWavetable wavetable, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setOscWavetable(wavetable, voice_num);
        }
    }

    void setADSR(int attack, int decay, float sustain, int release, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setADSR(attack, decay, sustain, release, voice_num);
        }
    }

    void setFilterADSR(int attack, int decay, float sustain, int release, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setFilterADSR(attack, decay, sustain, release, voice_num);
        }
    }
    void setOscFilterSend(float filtersend, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setOscFilterSend(filtersend, voice_num);
        }
    }

    void setLfoHz(float hz, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoHz(hz, voice_num);
        }
    }

    void setLfoSixteenths(float num_sixteenths, float bpm, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoSixteenths(num_sixteenths, bpm, voice_num);
        }
    }

    void setLfoPhase(float phase, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoPhase(phase, voice_num);
        }
    }

    void setLfoDelay(int delay_ms, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoDelay(delay_ms, voice_num);
        }
    }

    void setLfoFade(int delay_ms, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoFade(delay_ms, voice_num);
        }
    }

    void setLfoRetrig(bool retrig, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoRetrig(retrig, voice_num = 0);
        }
    }

    void setLfoOscAmt(float amt, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoOscAmt(amt, voice_num);
        }
    }

    void setLfoResAmt(float amt, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoResAmt(amt, voice_num);
        }
    }

    void setLfoCutoffAmt(float amt, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoCutoffAmt(amt, voice_num);
        }
    }

    void setLfoLevelAmt(float amt, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setLfoLevelAmt(amt, voice_num);
        }
    }

    void setOscSync(float percent, int voice_num = 0)
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->setOscSync(percent, voice_num);
        }
    }

    void update()
    {
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            voices[i]->update();
        }
    }

    int32_t next()
    {
        int32_t out_sample = 0;
        for (int i = mPOLYPHONY; --i >= 0;)
        {
            out_sample += voices[i]->next();
        }
        return out_sample;
    }

    // CLASS VARIABLES
    mAnalogVoice *voices[mPOLYPHONY];
};

#endif // MANALOG_INSTRUMENT_H_