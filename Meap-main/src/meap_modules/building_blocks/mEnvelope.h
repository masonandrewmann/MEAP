#ifndef MENVELOPE_H_
#define MENVELOPE_H_

#include "mEnvSection.h"

template <uint32_t UPDATE_RATE = CONTROL_RATE>
class mEnvelope
{
public:
    mEnvelope(float *levels, int32_t *times, int32_t num_sections, int32_t release_step)
    {
        _env_sections = new mEnvSection<UPDATE_RATE> *[num_sections];
        _envelope_state = -1;
        _release_step = release_step; // envelope will hold at end of release step until noteoff is received
        _num_sections = num_sections;
        _starting_level = levels[0];
        _last_out = _starting_level;
        for (int i = 0; i < _num_sections; i++)
        {
            _env_sections[i] = new mEnvSection<UPDATE_RATE>(times[i], levels[i], levels[i + 1], mCURVE_EXPONENTIAL);
        }
    };

    ~mEnvelope()
    {
        for (int i = 0; i < _num_sections; i++)
        {
            delete _env_sections[i];
        }
    }

    void noteOn()
    {
        _envelope_state = 0;
        _env_sections[0]->trigger();
        _env_sections[0]->setStartLevel(_last_out); // set next env section to start at current env value to prevent clicks (no RTZ)
    }

    void noteOff()
    {
        _envelope_state = _release_step + 1;
        if (_envelope_state >= _num_sections)
        {
            _envelope_state = _num_sections - 1;
        }
        _env_sections[_envelope_state]->setStartLevel(_last_out); // set next env section to start at current env value to prevent clicks
        _env_sections[_envelope_state]->trigger();
    }

    float next()
    {
        if (_envelope_state == -1) // starting state
        {
            _last_out = _starting_level;
            return _last_out;
        }
        else
        {
            if (_env_sections[_envelope_state]->done() && (_envelope_state + 1) < _num_sections && _envelope_state != _release_step)
            {
                _envelope_state++;
                _env_sections[_envelope_state]->trigger();
            }
            _last_out = _env_sections[_envelope_state]->next();
            return _last_out;
        }
    }

    int32_t _envelope_state;
    int32_t _release_step;
    float _starting_level;
    float _last_out;
    // bool _rtz;
    int32_t _num_sections;
    mEnvSection<UPDATE_RATE> **_env_sections;
};

#endif // MENVELOPE_H_