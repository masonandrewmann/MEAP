#ifndef MFIR_H_
#define MFIR_H_

template <uint8_t filter_order, class T = int32_t>
class mFIR
{
public:
    mFIR()
    {
        b = (float *)malloc(filter_order * sizeof(float));

        x = (T *)calloc(filter_order, sizeof(T));
    }

    void setCoefficients(float *b_)
    {

        for (int i = filter_order; --i >= 0;)
        {
            b[i] = b_[i];
        }
    }

    T next(T in_sample)
    {
        T out_sample = 0;
        x[0] = in_sample;
        for (int i = filter_order - 1; i > 0; i--)
        {
            out_sample += b[i] * x[i];
            x[i] = x[i - 1];
        }
        out_sample += b[0] * x[0];

        return out_sample;
    }

    float *b;
    T *x;
};
#endif // MFIR_H_