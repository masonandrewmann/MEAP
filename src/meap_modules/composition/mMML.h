#ifndef M_MML_H_
#define M_MML_H_

#include <meap_modules/instruments/mInstrument.h>

// Sequencer based on Music Macro Language
// modeled off of implementations by timbre.js https://github.com/mohayonao/timbre.js/blob/master/src/objects/mml.js

class mMML
{
public:
    mMML(void (*noteOn_)(uint16_t note, uint16_t vel), void (*noteOff_)(uint16_t note), String mml_string_)
    {
        noteOn = noteOn_;
        noteOff = noteOff_;
        timer = 0;
        switcher = false;
        mml_string = mml_string_;
        string_pointer = 0;
        using_instrument = true;

        prev_note = 0;

        setTempo(120);
        setVelocity(100);
        setOctave(3);
    }

    void start()
    {
    }

    void stop()
    {
    }

    void pause()
    {
    }

    // sets number of microseconds in one measure
    void setTempo(int32_t tempo)
    {
        period_micros = 240000000 / tempo;
    }

    void setNoteLength(int32_t default_length_)
    {
        default_length = default_length_;
    }

    void setVelocity(int8_t velocity_)
    {
        velocity = velocity_;
    }

    void setOctave(int8_t octave_)
    {
        octave = octave_;
    }

    bool isStartCommand(char cmd)
    {
        switch (cmd)
        {
        case 'a' ... 'g':
            return true;
        case 'o':
            return true;
        case 'v':
            return true;
        case 't':
            return true;
        case 'r':
            return true;
        case 'l':
            return true;
        }
        return false;
    }

    void next()
    {
        if (micros() > timer)
        {
            char my_char = mml_string.charAt(string_pointer++);
            while (my_char == ' ') // skip spaces
            {
                my_char = mml_string.charAt(string_pointer++);
            }

            switch (my_char)
            {
            case 'a' ... 'g': //  **** NOTE COMMAND ****
            {
                int note_length = default_length;
                float dot_modifier = 1;
                // instrument->noteOff(prev_note);
                noteOff(prev_note);
                switch (my_char)
                {
                case 'c':
                    prev_note = (octave + 2) * 12;
                    break;
                case 'd':
                    prev_note = 2 + (octave + 2) * 12;
                    break;
                case 'e':
                    prev_note = 4 + (octave + 2) * 12;
                    break;
                case 'f':
                    prev_note = 5 + (octave + 2) * 12;
                    break;
                case 'g':
                    prev_note = 7 + (octave + 2) * 12;
                    break;
                case 'a':
                    prev_note = 9 + (octave + 2) * 12;
                    break;
                case 'b':
                    prev_note = 11 + (octave + 2) * 12;
                    break;
                }

                my_char = mml_string.charAt(string_pointer);
                while (!isStartCommand(my_char))
                {
                    switch (my_char)
                    {
                    case '-': // flat
                        prev_note--;
                        break;
                    case '+': // sharp
                        prev_note++;
                        break;
                    case '>': // octave up
                        prev_note += 12;
                        break;
                    case '<': // octave down
                        prev_note -= 12;
                        break;
                    case '0' ... '9': // note length
                        note_length = parseNumber();
                        string_pointer--;
                        break;
                    case '.': // dotted note length
                        dot_modifier *= 1.5;
                        break;
                    }

                    my_char = mml_string.charAt(++string_pointer);
                }
                // instrument->noteOn(prev_note, velocity);
                noteOn(prev_note, velocity);
                timer = micros() + dot_modifier * (period_micros / (note_length));
                break;
            }
            case 'o': // **** OCTAVE COMMAND ****
                octave = parseNumber();
                break;
            case 'v': // **** VELOCITY COMMAND ****
                velocity = parseNumber();
                if (velocity > 127)
                {
                    velocity = 127;
                }
                break;
            case 't': // **** TEMPO COMMAND ****
                setTempo(parseNumber());
                break;
            case 'r': // **** REST COMMAND ****
            {
                int note_length = default_length;
                float dot_modifier = 1;

                my_char = mml_string.charAt(string_pointer);
                while (!isStartCommand(my_char))
                {
                    switch (my_char)
                    {
                    case '0' ... '9': // note length
                        note_length = parseNumber();
                        string_pointer--;
                        break;
                    case '.': // dotted note length
                        dot_modifier *= 1.5;
                        break;
                    }

                    my_char = mml_string.charAt(++string_pointer);
                }
                Serial.println(dot_modifier);
                timer = micros() + dot_modifier * (period_micros / (note_length));
                break;
            }
            case 'l': // **** NOTE LENGTH COMMAND ****
                setNoteLength(parseNumber());
                break;
            }

            if (string_pointer == mml_string.length())
            {
                string_pointer = 0;
            }
        }
    }

    int32_t parseNumber()
    {
        int start_index = string_pointer;
        int end_index;
        char my_char = mml_string.charAt(string_pointer);
        while (!isStartCommand(my_char) && my_char != ' ')
        {
            string_pointer++;
            if (string_pointer == mml_string.length())
            {
                break;
            }
            my_char = mml_string.charAt(string_pointer);
        }
        end_index = string_pointer;
        // string_pointer--;
        return (mml_string.substring(start_index, end_index).toInt());
    }

    bool using_instrument;

    String mml_string;
    int16_t string_pointer;
    int8_t octave;
    int8_t velocity;
    int32_t default_length = 4;

    void (*noteOn)(uint16_t note, uint16_t vel); // pointer to user-supplied noteOn function
    void (*noteOff)(uint16_t note);              // pointer to user-supplied noteOff function

    // string
    uint64_t timer;
    uint32_t period_micros;
    bool switcher;

    int prev_note;
};

#endif // M_MML_H_