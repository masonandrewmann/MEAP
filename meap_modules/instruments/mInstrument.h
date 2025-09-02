#ifndef MINSTRUMENT_H_
#define MINSTRUMENT_H_

#include <dependencies/LinkedList/LinkedList.h>

// parent class of mInstruments

struct MidiMessage
{
    uint8_t message_type;
    uint8_t data1;
    uint8_t data2;
    uint16_t time;
};

template <uint16_t mPOLYPHONY = 4>
class mInstrument
{
public:
    mInstrument(uint8_t *midi_table_name = NULL)
    {
        for (uint16_t i = 0; i < mPOLYPHONY; i++)
        {
            // _voices[i].init();
            _free_voices.unshift(i); // add all voices to voice queue
        }
        _curr_voice = 0;
        _midi_table_name = midi_table_name;
        _playing = false;
        _pulse_counter = 0;

        _midi_message.message_type = 0;
        _midi_message.data1 = 0;
        _midi_message.data2 = 0;
        _midi_message.time = 0;
    }

    void setMidiTable(uint8_t *midi_table_name)
    {
        _midi_table_name = midi_table_name;
    }

    void midiStart()
    {
        _playing = true;
        _current_midi_address = _midi_table_name;
        _pulse_counter = 0;

        _midi_message.message_type = _current_midi_address[0];
        _midi_message.data1 = _current_midi_address[1];
        _midi_message.data2 = _current_midi_address[2];
        _midi_message.time = (_current_midi_address[3] << 8) + _current_midi_address[4];
    }

    void midiStop()
    {
        _playing = false;
    }

    bool isPlaying()
    {
        return _playing;
    }

    void updateMidi()
    {
        if (_playing)
        {
            _pulse_counter += 1;
            while (_pulse_counter >= _midi_message.time)
            {
                switch (_midi_message.message_type) // notes are indexed from 0 on sample_bank starting from C-1 (0)
                {
                case 0x80: // note off
                    noteOff(_midi_message.data1);
                    break;
                case 0x90: // note on
                    if (_midi_message.data1 != 127)
                    {
                        noteOn(_midi_message.data1, _midi_message.data2);
                    }
                    break;
                case 255: // end of file
                    _playing = false;
                    return;
                    break;
                }
                _current_midi_address += 5;
                _pulse_counter = 0;

                _midi_message.message_type = _current_midi_address[0];
                _midi_message.data1 = _current_midi_address[1];
                _midi_message.data2 = _current_midi_address[2];
                _midi_message.time = (_current_midi_address[3] << 8) + _current_midi_address[4];
            }
        }
    }

    virtual void noteOn(uint16_t note, uint16_t vel) = 0;

    virtual void noteOff(uint16_t note) = 0;

    void noteOnVoiceHandler(uint16_t note, uint16_t vel)
    {
        if (_free_voices.size() > 0)
        {
            _curr_voice = _free_voices.shift(); // remove head element and return it!
        }
        else
        {
            uint8_t voice_num = _nonfree_voices.get(0)->voice_num; // voice num of note to turn off
            // voices[voice_num].noteOff();                           // turn note off
            _free_voices.add(voice_num);        // re-add voice to free queue
            delete (_nonfree_voices.get(0));    // delete voice from pressed queue
            _nonfree_voices.remove(0);          // remove freed voice from pressed queue
            _curr_voice = _free_voices.shift(); // remove head element
        }

        // store note in pressed notes queue
        MeapNoteAndVoice *my_note = new MeapNoteAndVoice;
        my_note->note_num = note;
        my_note->voice_num = _curr_voice;
        _nonfree_voices.add(my_note);
    }

    int16_t noteOffVoiceHandler(uint16_t note)
    {
        uint8_t num_nonfree_voices = _nonfree_voices.size();
        for (uint8_t i = 0; i < num_nonfree_voices; i++)
        {
            if (_nonfree_voices.get(i)->note_num == note)
            {
                uint8_t voice_num = _nonfree_voices.get(i)->voice_num; // voice num of note to turn off
                // voices[voice_num].noteOff();
                _free_voices.add(voice_num);     // re-add voice to free queue
                delete (_nonfree_voices.get(i)); // delete voice from pressed queue
                _nonfree_voices.remove(i);       // remove freed voice from pressed queue
                return voice_num;
            }
        }
        return -1;
    }

    void flush()
    {
        _nonfree_voices.clear();
        _free_voices.clear();
        for (uint8_t i = 0; i < mPOLYPHONY; i++)
        {
            _free_voices.unshift(i); // add all voices to voice queue
            // voices[i].noteOff();
        }
    }

    // CLASS VARIABLES

    LinkedList<int16_t> _free_voices;
    LinkedList<MeapNoteAndVoice *> _nonfree_voices;

    uint8_t *_midi_table_name;
    uint8_t *_current_midi_address;
    bool _playing;
    uint64_t _pulse_counter;
    MidiMessage _midi_message;

    uint16_t _curr_voice;
};

#endif // MINSTRUMENT_H_
