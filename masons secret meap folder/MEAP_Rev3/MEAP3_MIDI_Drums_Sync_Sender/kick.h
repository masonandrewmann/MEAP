#ifndef kick_H_
#define kick_H_
 
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "mozzi_pgmspace.h"
 
#define kick_NUM_CELLS 982
#define kick_SAMPLERATE 16384
 
CONSTTABLE_STORAGE(int8_t) kick_DATA [] = {0, -1, -1, -1, -2, -3, -3, -4, -5,
-5, -6, -7, -9, -10, -12, -13, -14, -16, -18, -20, -21, -24, -26, -28, -28, -24,
-17, -6, 1, 2, -5, -14, -22, -28, -29, -21, -21, -24, -10, -2, -28, -54, -44,
-8, 30, 52, 61, 64, 57, 28, -3, -14, -9, 7, 25, 36, 48, 68, 70, 45, 24, 28, 42,
50, 52, 63, 81, 81, 61, 47, 55, 75, 96, 105, 92, 69, 49, 37, 36, 49, 73, 93, 97,
85, 63, 35, 17, 17, 28, 41, 58, 70, 64, 49, 35, 24, 17, 16, 10, 3, 1, 2, 1, 3,
-1, -13, -21, -19, -15, -17, -23, -33, -44, -51, -54, -54, -54, -55, -57, -60,
-64, -65, -63, -63, -71, -81, -86, -82, -77, -74, -76, -85, -96, -105, -107,
-102, -94, -89, -86, -82, -82, -87, -94, -97, -99, -99, -100, -99, -99, -99,
-97, -94, -91, -92, -95, -93, -89, -86, -82, -82, -83, -80, -76, -76, -80, -82,
-81, -78, -70, -62, -55, -50, -50, -55, -57, -53, -45, -38, -34, -32, -31, -29,
-29, -34, -36, -27, -12, 1, 11, 18, 16, 5, -5, -9, -6, 3, 15, 24, 28, 33, 38,
39, 32, 25, 21, 20, 24, 38, 57, 69, 66, 53, 41, 38, 41, 47, 55, 60, 63, 62, 58,
54, 54, 54, 52, 49, 52, 60, 67, 71, 71, 70, 66, 64, 63, 63, 61, 58, 58, 61, 66,
68, 67, 64, 62, 64, 67, 69, 69, 67, 62, 58, 57, 60, 63, 65, 67, 67, 66, 65, 65,
64, 64, 62, 60, 61, 63, 62, 58, 54, 53, 55, 58, 62, 64, 65, 64, 61, 55, 50, 46,
47, 50, 52, 54, 55, 56, 54, 52, 50, 50, 50, 51, 49, 44, 41, 39, 39, 41, 43, 42,
37, 30, 29, 32, 36, 36, 35, 33, 32, 30, 28, 26, 25, 26, 27, 25, 20, 16, 12, 10,
12, 16, 20, 18, 13, 9, 7, 7, 8, 6, 3, 1, 0, -1, -2, -5, -8, -8, -5, -2, -3, -8,
-13, -18, -21, -24, -24, -22, -18, -16, -16, -17, -18, -21, -25, -30, -33, -32,
-29, -28, -30, -35, -39, -42, -42, -40, -38, -39, -40, -42, -44, -46, -47, -46,
-45, -45, -46, -46, -46, -44, -45, -48, -53, -58, -61, -60, -55, -49, -46, -46,
-50, -55, -58, -60, -59, -57, -55, -55, -55, -56, -58, -62, -64, -63, -61, -60,
-60, -61, -62, -61, -60, -58, -57, -57, -57, -60, -63, -64, -64, -63, -61, -59,
-58, -58, -59, -61, -61, -61, -60, -60, -60, -58, -56, -55, -54, -55, -56, -57,
-56, -56, -56, -56, -56, -55, -54, -54, -54, -52, -50, -47, -45, -43, -43, -44,
-45, -46, -46, -45, -43, -41, -41, -42, -42, -40, -36, -33, -30, -28, -25, -25,
-26, -28, -30, -30, -28, -25, -22, -22, -23, -23, -21, -18, -14, -12, -12, -14,
-14, -11, -6, -3, -1, -1, -2, -3, -5, -5, -4, -1, 1, 3, 5, 6, 7, 8, 11, 13, 14,
13, 11, 11, 12, 13, 14, 16, 21, 25, 29, 30, 30, 28, 26, 24, 24, 24, 26, 28, 31,
35, 38, 39, 39, 39, 40, 41, 40, 40, 40, 39, 37, 37, 39, 43, 47, 48, 48, 48, 50,
53, 57, 58, 57, 54, 53, 52, 51, 50, 50, 51, 53, 57, 61, 64, 64, 63, 60, 55, 52,
53, 56, 58, 59, 59, 59, 60, 62, 64, 65, 65, 64, 63, 64, 63, 61, 58, 58, 60, 63,
65, 66, 66, 64, 62, 62, 60, 58, 56, 56, 59, 62, 63, 62, 60, 59, 60, 60, 60, 59,
59, 59, 57, 54, 50, 47, 48, 51, 56, 59, 58, 54, 49, 46, 46, 45, 45, 45, 47, 47,
46, 45, 44, 43, 40, 37, 36, 36, 35, 34, 31, 29, 28, 28, 29, 30, 30, 30, 30, 30,
29, 24, 18, 13, 11, 12, 14, 16, 15, 13, 12, 12, 15, 16, 14, 10, 8, 6, 5, 4, 3,
2, 1, 0, -2, -2, -2, -3, -5, -7, -5, -2, -2, -4, -8, -12, -15, -18, -19, -18,
-17, -17, -18, -22, -26, -28, -28, -28, -28, -30, -31, -34, -36, -38, -39, -40,
-42, -45, -47, -48, -50, -53, -56, -56, -56, -56, -57, -59, -61, -63, -65, -68,
-69, -71, -73, -74, -75, -74, -74, -73, -72, -72, -72, -73, -74, -77, -80, -82,
-84, -86, -87, -86, -83, -79, -75, -72, -72, -74, -76, -78, -78, -79, -79, -80,
-80, -80, -78, -75, -71, -66, -61, -59, -59, -60, -61, -61, -60, -60, -60, -58,
-54, -50, -47, -46, -46, -47, -46, -44, -40, -36, -31, -28, -28, -29, -31, -31,
-30, -28, -26, -23, -21, -18, -16, -14, -12, -11, -10, -9, -6, -4, -2, -2, -2,
-2, -3, -4, -6, -6, -3, 2, 6, 9, 9, 7, 5, 5, 8, 11, 12, 12, 10, 9, 9, 10, 11,
11, 12, 14, 14, 15, 16, 16, 14, 10, 8, 9, 13, 16, 16, 15, 14, 15, 17, 16, 15,
15, 17, 19, 17, 15, 12, 11, 12, 15, 17, 17, 16, 14, 14, 16, 20, 22, 23, 23, 22,
21, 18, 15, 14, 13, 14, 16, 18, 19, 19, 19, 19, 20, 19, 18, 18, 18, 19, 21, 22,
23, 21, 17, 14, 12, 12, 12, 12, 13, 15, 16, 17, 17, 17, 15, 11, 8, 6, 5, 5, 4,
3, 3, 5, 6, 5, 4, 2, -1, -3, -5, -6, -5, -5, -6, -8, -9, -9, -10, -11, -12, -14,
-15, -15, -14, -16, -18, -19, -20, -19, -18, -18, -19, -21, -23, -24, -24, -24,
-23, -22, -23, -23, -22, -20, -19, -19, -21, -24, -27, -28, -27, -26, -24, -23,
-23, -23, -22, -21, -20, -19, -17, -15, -14, -14, -14, -14, -13, -12, -13, -14,
-14, -12, -11, -11, -11, -11, -10, -7, -2, };

#endif /* kick_H_ */