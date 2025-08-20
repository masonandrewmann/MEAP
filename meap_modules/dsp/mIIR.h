#ifndef MIIR_H_
#define MIIR_H_

template <uint8_t filter_order, class T = int32_t>
class mIIR
{
public:
    mIIR()
    {
        a = (float *)malloc(filter_order * sizeof(float));
        b = (float *)malloc(filter_order * sizeof(float));

        x = (T *)calloc(filter_order, sizeof(T));
        y = (T *)calloc(filter_order, sizeof(T));
    }

    void setCoefficients(float *a_, float *b_)
    {

        for (int i = filter_order; --i >= 0;)
        {
            a[i] = a_[i];
            b[i] = b_[i];
        }

        if (a[0] != 1.f)
        { // normalize coefficients if a[0] is not equal to 1
            float scale = 1.f / a[0];
            for (int i = filter_order; --i >= 0;)
            {
                a[i] *= scale;
                b[i] *= scale;
            }
        }
    }

    T next(T in_sample)
    {
        y[0] = 0;
        x[0] = in_sample;
        for (int i = filter_order - 1; i > 0; i--)
        {
            y[0] += b[i] * x[i];
            x[i] = x[i - 1];
        }
        y[0] += b[0] * x[0];
        for (int i = filter_order - 1; i > 0; i--)
        {
            y[0] += -a[i] * y[i];
            y[i] = y[i - 1];
        }
        return y[0];
    }

    float *a;
    float *b;
    T *x;
    T *y;
};
#endif // MIIR_H_