#ifndef MEAP_RANDOM_DISTRIBUTION_H_
#define MEAP_RANDOM_DISTRIBUTION_H_

#define NV_MAGICCONST 1.7155277699214135 // 4 * _exp(-0.5)/_sqrt(2.0)

class mRandomDistribution
{
public:
    mRandomDistribution() {
    };

    /** triangular.
     * https://en.wikipedia.org/wiki/Triangular_distribution
     * Generates a random sample N in a triangular distribution in range lo <= N <= hi, with a specified mode
     @param lo : low limit of distribution
     @param hi : high limit of distribution
     @param mid : mode between lo and hi
    */
    static float triangular(float lo, float hi, float mid)
    {
        float val = Meap::frand();

        float c = (mid - lo) / (hi - lo);

        if (val > c)
        {
            val = 1.0 - val;
            c = 1.0 - c;
            float t = hi;
            hi = lo;
            lo = t;
        }
        return lo + (hi - lo) * sqrt(val * c);
    }

    /** normal.
     * https://en.wikipedia.org/wiki/Normal_distribution
     * Generates a random sample in a normal distribution with specified mean and standard deviation
     @param mean
     @param std_dev
    */
    static float normal(float mean, float std_dev)
    {
        float u1, u2, z, zz;

        while (true)
        {
            u1 = Meap::frand();
            u2 = 1.0 - Meap::frand();
            z = NV_MAGICCONST * (u1 - 0.5) / u2;
            zz = z * z / 4.0;
            if (zz <= -log(u2))
            {
                break;
            }
        }
        return mean + z * std_dev;
    }

    /** log_normal.
     * WARNING: NOT THOROUGHLY TESTED, CAN RETURN NaN
     * https://en.wikipedia.org/wiki/Log-normal_distribution
     * Generates a random sample in a log-normal distribution with specified mean and standard deviation
     @param mean
     @param std_dev
    */
    static float log_normal(float mean, float std_dev)
    {
        return log(normal(mean, std_dev));
    }

    // static int64_t linear(int64_t min, int64_t max)
    // {
    //     return Meap::irand(min, max);
    // }

    /** exponential.
     * https://en.wikipedia.org/wiki/Exponential_distribution
     * Generates a random sample in a decreasing exponential distribution with specified lambda
     @param lambd 1.0 divided by desired mean
    */
    static float exponential(float lambd)
    {
        return -log(1.0 - Meap::frand()) / lambd;
    }

    /** linear.
     * Generates a random sample in a decreasing linear distribution with specified min and max
     @param lo low limit of output range
     @param hi high limit of output range
    */
    static float linear(float lo, float hi)
    {
        return min(Meap::frand() * (hi - lo) + lo, Meap::frand() * (hi - lo) + lo);
    }

    /** gamma.
     * Generates a random sample in a gamma distribution with specified alpha and beta
     * https://en.wikipedia.org/wiki/Gamma_distribution
     * alpha & beta > 0
     @param alpha shape parameter
     @param beta rate parameter: larger = greater spread
    */
    float gamma(float alpha, float beta)
    {
        const float LOG4 = log(4);
        const float SG_MAGICCONST = 1.0 + log(4.5);

        if (alpha > 1.0)
        {
            //  #  Uses R.C.H. Cheng, "The generation of Gamma
            //  #  variables with non-integral shape parameters",
            //  #  Applied Statistics, (1977), 26, No. 1, p71-74

            float ainv = sqrt(2.0 * alpha - 1.0);
            float bbb = alpha - LOG4;
            float ccc = alpha + ainv;

            float u1, u2, v, x, z, r;
            while (true)
            {
                u1 = Meap::frand();
                if (u1 < 1e-7)
                    continue;
                if (u1 > 0.9999999)
                    continue; //  needed?

                u2 = 1.0 - Meap::frand();
                v = log(u1 / (1.0 - u1)) / ainv;
                x = alpha * exp(v);
                z = u1 * u1 * u2;
                r = bbb + ccc * v - x;
                if (((r + SG_MAGICCONST - 4.5 * z) >= 0.0) ||
                    (r >= log(z)))
                {
                    return x * beta;
                }
            }
        }
        else if (alpha == 1.0)
        {
            return -log(1.0 - Meap::frand()) * beta;
        }
        else //  alpha in 0..1
        {
            //  #  Uses ALGORITHM GS of Statistical Computing - Kennedy & Gentle

            float u, b, p, x, u1;
            while (true)
            {
                u = Meap::frand();
                b = (MEAP_EULER + alpha) / MEAP_EULER;
                p = b * u;
                if (p <= 1.0)
                    x = pow(p, (1.0 / alpha));
                else
                    x = -log((b - p) / alpha);
                u1 = Meap::frand();
                if (p > 1.0)
                {
                    if (u1 <= pow(x, (alpha - 1.0)))
                        break;
                }
                else
                {
                    if (u1 <= exp(-x))
                        break;
                }
            }
            return x * beta;
        }
    }
    /** gamma.
     * Generates a random sample in a gamma distribution with specified alpha and beta
     * https://en.wikipedia.org/wiki/Beta_distribution
     * alpha & beta > 0
     @param lo low limit of output range
     @param hi high limit of output range
    */
    float beta(float alpha, float beta)
    {
        float y = gamma(alpha, 1.0);
        if (y == 0)
            return 0.0;
        return y / (y + gamma(beta, 1.0));
    };

    // static int64_t gamma(int64_t min, int64_t max)
    // {
    //     return Meap::irand(min, max);
    // }

    // static int64_t beta(int64_t min, int64_t max)
    // {
    //     return Meap::irand(min, max);
    // }
};

#endif // MEAP_RANDOM_DISTRIBUTION_H_
