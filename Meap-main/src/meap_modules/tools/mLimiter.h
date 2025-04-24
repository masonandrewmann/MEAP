// #ifndef MEAP_LIMITER_H_
// #define MEAP_LIMITER_H_

// class mLimiter
// {
// public:
//     //! Class constructor.
//     mLimiter(void)
//     {
//         // hard-coded coefficients for a butterworth with fc=30 at Fs = 32768
//         b0 = 0.00000823907113;
//         b1 = 0.0000164781423;
//         b2 = 0.00000823907113;

//         a1 = -1.99186488;
//         a2 = 0.99189784;

//         y_1 = 0;
//         y_2 = 0;
//         x_1 = 0;
//         x_2 = 0;
//     };

//     T next(int32_t in_sample)
//     {
//         // apply filter to abs(x) to calculate gain
//         int32_t filtered_sample = b0 * abs(in_sample) + b1 * x_1 + b2 * x_2 - a1 * y_1 - a2 * y_2;
//         // check if sample is above limit and attenuate if needed
//         // check dafx book, the pdf is bad!
//     }

//     // CLASS VARIABLES
//     float b0;
//     float b1;
//     float b2;
//     float a0;
//     float a1;
//     float a2;

//     float gain;
//     uint32_t threshold;
//     int32_t x_1;
//     int32_t x_2;
//     int32_t y_1;
//     int32_t y_2;
// };

// #endif // MEAP_LIMITER_H_
