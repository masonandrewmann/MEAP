// #ifndef M_DRUM_RACK_INSTRUMENT_H_
// #define M_DRUM_RACK_INSTRUMENT_H_

// #include <dependencies/LinkedList/LinkedList.h>

// #include "mInstrument.h"

// template <uint16_t mNUM_SAMPLES, uint32_t mMAX_SAMPLE_LENGTH, uint16_t mPOLYPHONY = 4, class T = int8_t>
// class mDrumRackInstrument : public mInstrument<mPOLYPHONY>
// {
// public:
//     mDrumRackInstrument(const T **sample_list, uint32_t *sample_lengths, uint8_t *midi_table_name = NULL) : mInstrument<mPOLYPHONY>(midi_table_name)
//     {
//         sample_list_ = sample_list;
//         sample_lengths_ = sample_lengths;

//         default_freq = (float)AUDIO_RATE / (float)mMAX_SAMPLE_LENGTH;

//         for (uint16_t i = mPOLYPHONY; --i >= 0;)
//         {
//             velocity_[i] = 32385; // 127 * 255
//             sample_bank_[i].setTable(sample_list_[0]);
//             sample_bank_[i].setEnd(sample_lengths_[0]);
//             sample_bank_[i].setFreq(default_freq);
//             // sample_bank_[i].stop();
//         }

//         for (uint16_t i = 0; i < mNUM_SAMPLES; i++)
//         {
//             sample_gain_[i] = 255;
//         }

//         if (sizeof(T) == sizeof(int16_t))
//         {
//             shift_val_ = 15; // 8 bits down for sample birate + 7 bits for velocity
//         }
//         else
//         {
//             shift_val_ = 7; // just 7 bits down for velocity
//         }
//     }

//     void midiStop()
//     {
//         mInstrument<mPOLYPHONY>::midiStop();
//         for (int16_t i = mPOLYPHONY; --i >= 0;)
//         {
//             // voices[i].noteOff();
//             sample_bank_[i].stop();
//         }
//     }

//     void noteOn(uint16_t note, uint16_t vel)
//     {
//         mInstrument<mPOLYPHONY>::noteOnVoiceHandler(note, vel);
//         // sample_bank_[this->_curr_voice].setTable(sample_list_[note]);
//         // sample_bank_[this->_curr_voice].setEnd(sample_lengths_[note]);
//         // sample_bank_[this->_curr_voice].start();
//         // velocity_[this->_curr_voice] = vel * sample_gain_[note];
//     }

//     void noteOff(uint16_t note)
//     {
//         int16_t off_voice = mInstrument<mPOLYPHONY>::noteOffVoiceHandler(note);
//         if (off_voice != -1)
//         {
//             // voices[off_voice].noteOff();
//             // sample_bank_[off_voice].stop();
//         }
//     }

//     void flush()
//     {
//         mInstrument<mPOLYPHONY>::flush();
//         for (int8_t i = mPOLYPHONY; --i >= 0;)
//         {
//             // voices[i].noteOff();
//             // sample_bank_[i].stop();
//         }
//     }

//     // void update()
//     // {
//     //     for (uint16_t i = 0; i < mPOLYPHONY; i++)
//     //     {
//     //         // voices[i].update();
//     //     }
//     // }

//     int32_t next()
//     {
//         int32_t out_sample = 0;
//         for (uint16_t i = mPOLYPHONY; --i >= 0;)
//         {
//             // out_sample += voices[i].next();
//             // out_sample += (sample_bank_[i].next() * velocity_[i]) >> shift_val_;
//             out_sample += sample_bank_[i].next();
//         }

//         return out_sample;
//     }

//     // CLASS VARIABLES
//     mSample<mMAX_SAMPLE_LENGTH, AUDIO_RATE, T> sample_bank_[mPOLYPHONY];
//     int16_t velocity_[mPOLYPHONY];
//     int trigger_num;

//     uint16_t sample_gain_[mNUM_SAMPLES];

//     float default_freq;
//     const int8_t **sample_list_;
//     uint32_t *sample_lengths_;

//     uint8_t shift_val_;
// };

// #endif // M_DRUM_RACK_INSTRUMENT_H_
