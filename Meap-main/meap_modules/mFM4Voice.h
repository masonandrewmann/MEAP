#ifndef MFM4VOICE_H_
#define MFM4VOICE_H_

#include <tables/sin8192_int8.h> // table for Oscils to play

// ALGORITHMS
// 0: 3 -> 2 -> 1 -> 0
// 1: (3 + 2) -> 1 -> 0
// 2: (3 + (2 -> 1)) -> 0
// 3: ((3 -> 2) + (3 -> 1)) -> 0
// 4: ((3 -> 2) -> 1) + ((3 -> 2) -> 0)
// 5: (3 -> 2 -> 1) + 0
// 6: (3 + 2 + 1) -> 0
// 7: (3 -> 2) + (1 -> 0)
// 8: (3 -> 2) + 1 + 0
// 9: 3 + 2 + 1 + 0

template <uint32_t mNUM_CELLS = SIN8192_NUM_CELLS, class T = int8_t>
class mFM4Voice
{
public:
    mFM4Voice()
    {
        for (uint16_t i = 0; i < 4; i++)
        {
            op[i].init(SIN8192_DATA);
            last_output[i] = 0;
        }
        algorithm = 0;
    };

    mFM4Voice(const T *TABLE_NAME)
    {
        for (uint16_t i = 0; i < 4; i++)
        {
            op[i].init(TABLE_NAME);
            last_output[i] = 0;
        }
        algorithm = 0;
    };

    void init(const T *TABLE_NAME)
    {
        for (uint16_t i = 0; i < 4; i++)
        {
            op[i].init(TABLE_NAME);
            last_output[i] = 0;
        }
        algorithm = 0;
    }

    void setFreqRatios(float one_r, float two_r, float three_r, float four_r)
    {
        op[0].setFreqRatio(one_r);
        op[1].setFreqRatio(two_r);
        op[2].setFreqRatio(three_r);
        op[3].setFreqRatio(four_r);
    }

    // 0-3
    float getRatio(int op_num)
    {
        return op[op_num].getFreqRatio();
    }

    void setAlgorithm(uint16_t a)
    {
        if (a < 0)
        {
            a = 0;
        }
        else if (a > 10)
        {
            a = 10;
        }
        algorithm = a;
    }

    void update()
    {
        op[0].update();
        op[1].update();
        op[2].update();
        op[3].update();
    }

    void noteOn(uint8_t note, uint8_t vel)
    {
        op[0].noteOn(note, vel);
        op[1].noteOn(note, vel);
        op[2].noteOn(note, vel);
        op[3].noteOn(note, vel);
    }

    void noteOff()
    {
        op[0].noteOff();
        op[1].noteOff();
        op[2].noteOff();
        op[3].noteOff();
    }

    int32_t next()
    {
        int32_t output_sample;
        switch (algorithm) // replace this switch statement with something more efficient!
        {
        case 0:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next(last_output[2]);
            last_output[0] = op[0].next(last_output[1]);
            output_sample = last_output[0];
            break;
        case 1:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next();
            last_output[1] = op[1].next(last_output[3] + last_output[2]);
            last_output[0] = op[0].next(last_output[1]);
            output_sample = last_output[0];
            break;
        case 2:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next();
            last_output[1] = op[1].next(last_output[2]);
            last_output[0] = op[0].next(last_output[3] + last_output[1]);
            output_sample = last_output[0];
            break;
        case 3:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next(last_output[3]);
            last_output[0] = op[0].next(last_output[2] + last_output[1]);
            output_sample = last_output[0];
            break;
        case 4:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next(last_output[2]);
            last_output[0] = op[0].next(last_output[2]);
            output_sample = last_output[1] + last_output[0];
            break;
        case 5:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next(last_output[2]);
            last_output[0] = op[0].next();
            output_sample = last_output[1] + last_output[0];
            break;
        case 6:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next();
            last_output[1] = op[1].next();
            last_output[0] = op[0].next(last_output[3] + last_output[2] + last_output[1]);
            output_sample = last_output[0];
            break;
        case 7:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next();
            last_output[0] = op[0].next(last_output[1]);
            output_sample = last_output[2] + last_output[0];
            break;
        case 8:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next(last_output[3]);
            last_output[0] = op[0].next(last_output[3]);
            output_sample = last_output[2] + last_output[1] + last_output[0];
            break;
        case 9:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next(last_output[3]);
            last_output[1] = op[1].next();
            last_output[0] = op[0].next();
            output_sample = last_output[2] + last_output[1] + last_output[0];
            break;
        case 10:
            last_output[3] = op[3].next();
            last_output[2] = op[2].next();
            last_output[1] = op[1].next();
            last_output[0] = op[0].next();
            output_sample = last_output[3] + last_output[2] + last_output[1] + last_output[0];
            break;
        }
        return output_sample;
    }

    int32_t last_output[4];
    mOperator<mNUM_CELLS, T> op[4];

protected:
    uint16_t algorithm;
};

#endif // MFM4VOICE_H_