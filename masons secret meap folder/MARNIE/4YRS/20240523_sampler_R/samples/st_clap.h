#ifndef st_clap_H_
#define st_clap_H_
 
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "mozzi_pgmspace.h"
 
#define st_clap_NUM_CELLS 2185
#define st_clap_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) st_clap_DATA [] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1,
0, 0, 0, 0, 0, -1, -1, -1, 0, -1, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1,
-1, 0, 0, -1, 0, 0, -1, -1, 0, -2, -4, 3, 2, -17, -36, -41, -4, 49, 51, 36, 40,
18, -32, -56, -45, -7, 25, 11, -3, 2, 2, -2, 0, 3, 1, -2, -1, 0, -1, -3, 0, 4,
1, -14, -29, -33, 19, 82, 48, -18, -52, -46, 5, 21, 1, -3, 0, 1, 1, -1, 0, 3,
-1, -7, 0, 4, -5, -3, 13, -15, -60, -49, -3, 53, 81, 63, 39, 12, -48, -87, -41,
8, 15, 12, 2, -3, -3, 1, 2, -2, -1, 4, 2, -4, 0, 3, -3, -5, 2, -16, -42, -25,
11, 39, 49, 34, 17, -1, -34, -55, -22, 11, 6, 2, 6, 1, -6, 2, 6, -3, -5, 5, 3,
-7, 4, -3, -64, -98, -69, -3, 68, 73, 47, 46, 46, 35, 23, 18, 21, 12, -48, -96,
-86, -29, 46, 45, -2, -3, 11, 0, -3, 10, -33, -97, -86, -31, 34, 60, 24, 12, 32,
26, 15, 23, 27, 14, 11, 23, 17, 5, 17, 14, -29, -68, -84, -40, 32, 36, 15, 12,
6, -4, 4, 15, -11, -56, -102, -98, -18, 37, 39, 28, 17, 25, 35, 34, 34, 39, 29,
11, 5, -2, -16, -20, -7, -19, -46, -39, -17, -2, -14, -65, -67, -16, 14, 28, 41,
47, 44, 34, 22, 16, 17, 16, 8, 1, -3, -10, -18, -28, -37, -40, -34, -25, -16, 0,
21, 33, 37, 31, 22, 12, 2, -3, -8, -10, -9, -6, -8, -14, -20, -26, -29, -26,
-19, -17, -11, -3, 1, 6, 8, 10, 14, 20, 25, 27, 30, 34, 32, 23, 8, -7, -14, -21,
-20, -12, -17, -26, -26, -10, 19, 22, 3, 3, 7, -5, -10, -1, -23, -72, -88, -66,
-11, 28, 17, 10, 17, 18, 16, 25, 37, 44, 44, 37, 24, 4, -14, -33, -55, -64, -55,
-35, -10, 18, 41, 54, 60, 53, 36, 15, 6, -2, -35, -64, -55, -23, 10, 12, -8, -7,
-3, -18, -44, -68, -48, 2, 28, 31, 23, 14, 18, 17, 11, 7, 4, 5, 10, 12, 12, 12,
8, 4, 5, 4, -6, -27, -57, -38, 15, 29, 23, 17, 6, -9, -8, 4, -16, -46, -60, -48,
-2, 17, 7, 5, 5, 8, 13, 19, 28, 32, 33, 32, 27, 15, 6, -27, -86, -81, -20, 20,
36, 21, 6, 5, 2, -9, -75, -128, -88, 5, 84, 101, 50, 30, 38, 27, 12, 12, 10, 2,
0, -3, -3, -12, -38, -53, -49, -18, 34, 32, 1, 6, -7, -64, -85, -48, 8, 55, 56,
43, 43, 35, 15, 0, -7, -17, -19, -12, -9, -13, -14, -13, -17, -24, -22, -13, -2,
13, 27, 37, 39, 37, 29, 13, -11, -31, -45, -51, -49, -41, -27, -10, 5, 15, 20,
21, 23, 23, 22, 22, 22, 24, 27, 24, 10, -9, -21, -42, -66, -48, -6, 11, 1, -27,
-33, -1, 13, 6, 10, 17, 22, 30, 41, 46, 38, 18, -13, -46, -70, -84, -85, -73,
-49, -16, 20, 46, 61, 67, 65, 51, 32, 18, 9, -1, -5, -3, -3, -7, -7, -10, -26,
-37, -32, -13, 10, -10, -48, -45, -24, -10, -4, 1, 17, 35, 48, 55, 52, 39, 21,
6, -6, -13, -18, -21, -22, -23, -29, -40, -51, -59, -55, -40, -21, 3, 31, 51,
61, 62, 55, 43, 28, 15, 9, 8, 6, -1, -14, -29, -45, -59, -70, -72, -60, -37,
-11, 14, 34, 45, 47, 43, 37, 30, 24, 20, 18, 17, 14, 7, -3, -12, -21, -28, -30,
-27, -22, -14, -10, -8, -5, -4, -4, -3, -1, 2, 7, 11, 15, 15, 16, 17, 19, 22,
24, 22, 15, 2, -16, -37, -57, -71, -74, -64, -44, -16, 14, 39, 55, 60, 54, 41,
26, 12, 4, 0, 1, 2, 0, -4, -12, -22, -32, -37, -36, -29, -15, 1, 15, 26, 31, 29,
24, 16, 8, 2, -2, -4, -4, -4, -6, -10, -15, -19, -22, -23, -20, -14, -7, 1, 8,
12, 14, 13, 11, 9, 7, 5, 5, 5, 4, 3, 0, -2, -4, -6, -7, -6, -6, -3, -1, -1, 0,
0, -2, -3, -4, -6, -6, -7, -7, -4, 0, 3, 6, 9, 11, 11, 9, 6, 2, -2, -6, -10,
-10, -11, -10, -10, -8, -5, -5, -6, -5, -2, 1, 4, 7, 8, 7, 6, 6, 4, 4, 3, 3, 4,
4, 2, -1, -8, -14, -15, -15, -12, -7, -2, 3, 8, 12, 13, 9, 5, 2, -3, -7, -7, -5,
-2, -1, -4, -7, -8, -6, -2, 1, 3, 5, 10, 15, 16, 12, 14, 10, -20, -41, -32, -7,
19, 19, 2, -3, -1, -1, -5, -11, -12, -3, 7, 11, 0, -10, -9, -7, -6, 1, 9, 13,
19, 23, 10, -11, -23, -21, -8, 2, -2, -4, -2, -1, 0, 1, 0, 0, -4, -9, -17, -20,
7, 41, 54, 33, -18, -41, -31, -9, 8, 4, -3, -1, 0, -4, -5, -6, -14, -40, -89,
-80, -3, 51, 75, 78, 69, 63, 50, 28, 5, -23, -75, -103, -57, 1, 31, 28, -1, -2,
5, -17, -55, -95, -77, -3, 39, 44, 33, 20, 25, 30, 27, 32, 42, 42, 31, 17, 2,
-13, -25, -30, -30, -27, -22, -17, -18, -19, -18, -19, -15, -7, -2, 5, 9, 13,
16, 15, 11, 10, 16, 22, 28, 31, 26, 19, 7, -11, -26, -37, -43, -40, -26, -12, 2,
15, 23, 25, 19, 12, 12, 13, 9, 1, -5, -9, -13, -17, -21, -29, -38, -38, -30,
-17, -2, 17, 34, 47, 51, 47, 37, 16, -3, -18, -47, -63, -45, -13, 21, 24, 7, 4,
6, 5, 2, -2, -6, -9, -6, -5, -4, 3, 6, 4, -2, -7, -4, 0, 1, 2, 2, 3, 0, -5, -6,
-4, 1, 1, -3, -1, 2, 4, 3, -2, -3, -2, -3, -5, -4, -4, -6, -5, 0, -1, -5, -4, 3,
25, 35, 18, -6, -24, -24, -11, -4, 0, 0, 0, 0, 0, 0, -3, -3, 0, 3, 4, 1, -1, -1,
-3, -3, 1, 5, 6, 5, 1, -1, -1, 1, 1, -2, -3, -3, -3, -5, -6, -4, -2, 0, 1, 2, 3,
0, 0, 0, -3, -1, 1, -9, -22, -19, 2, 45, 60, 31, -10, -42, -33, -8, 2, 2, -4, 1,
6, 1, -4, -3, -1, -3, -5, -2, 4, 8, 6, 4, 5, 3, -2, -5, -6, -5, -2, -2, -1, -1,
-1, 1, 1, 0, 0, 0, 2, 0, -3, -6, -6, 1, 7, 7, 2, 0, 0, -1, -2, -5, -8, -5, -1,
3, 5, 7, 6, 1, -2, -1, -3, -7, -9, -5, -1, 0, 1, -4, -13, -46, -72, -31, 36, 79,
88, 58, 36, 25, -9, -46, -67, -56, -11, 16, 16, 10, 3, 6, 4, -1, 1, 5, -2, -10,
-5, 1, 0, -2, 4, 3, -3, 0, -3, -24, -40, -40, -13, 29, 42, 38, 32, 26, 25, 15,
-4, -35, -57, -37, -4, 15, 19, 7, 5, 11, 3, -6, 1, -6, -34, -60, -68, -22, 34,
45, 37, 24, 18, 21, 22, 18, 11, 6, 3, 1, -3, -6, -5, -7, -11, -18, -14, 0, 5, 3,
3, 3, 5, 4, 2, 3, 0, -11, -23, -36, -27, 10, 32, 32, 24, 8, -6, -17, -26, -26,
-18, -8, 1, 15, 29, 36, 42, 42, 3, -55, -58, -21, 10, 18, 2, -2, 8, 9, 2, -5,
-28, -80, -91, -43, 12, 52, 56, 47, 52, 49, 33, 22, 15, 1, -7, 2, 0, -27, -51,
-60, -30, 13, 30, 19, -15, -48, -66, -47, -11, 2, 6, 8, 15, 29, 38, 43, 45, 41,
30, 17, 7, -4, -11, -10, -6, -5, -8, -11, -16, -23, -30, -29, -22, -16, -16,
-20, -22, -19, -10, 1, 15, 32, 45, 51, 51, 44, 31, 16, 1, -8, -14, -18, -15, -9,
-2, 1, -2, -4, -3, -3, -8, -15, -22, -27, -29, -27, -22, -11, 3, 16, 28, 36, 36,
30, 23, 16, 8, 2, -2, -4, -3, 0, 2, 3, 0, -2, -1, 4, 8, 8, 9, 9, 6, 2, -6, -14,
-22, -34, -45, -53, -57, -51, -36, -13, 13, 35, 52, 64, 65, 55, 35, 14, -3, -14,
-19, -16, -11, -7, -4, -4, -2, 0, 1, 0, -3, -6, -10, -18, -22, -21, -15, -2, 12,
27, 45, 59, 60, 46, 20, -6, -29, -49, -63, -68, -58, -37, -12, 14, 37, 49, 49,
42, 28, 11, -5, -19, -28, -28, -23, -15, -5, 2, 5, 2, 0, 0, 2, 9, 13, 14, 14, 7,
0, 2, 6, 8, 9, 9, 5, -9, -22, -28, -12, 11, 10, 1, -2, -12, -30, -48, -51, -30,
-5, -1, -2, 5, 19, 36, 48, 53, 48, 38, 24, 11, -2, -13, -19, -18, -14, -13, -14,
-17, -23, -29, -32, -29, -21, -12, -2, 7, 14, 19, 19, 18, 18, 21, 26, 32, 36,
34, 25, 9, -8, -19, -26, -29, -24, -14, -8, -3, 1, 5, 9, 9, 10, 12, 16, 21, 26,
27, 21, 11, -3, -18, -28, -34, -34, -31, -27, -20, -12, -5, 3, 7, 10, 12, 17,
21, 21, 23, 26, 25, 21, 11, 0, -14, -32, -41, -33, -14, 4, 8, 4, 1, 0, -2, -5,
-3, 0, 4, 5, 2, 2, 1, 3, 7, 5, 0, -4, -3, 0, 3, 4, 3, 4, 5, -2, -8, -7, -6, -4,
-3, -1, 3, 7, 9, 6, 0, 1, 0, -34, -72, -83, -50, 28, 68, 64, 57, 49, 35, 19, 3,
-10, -12, -13, -13, -4, 1, -9, -18, -20, -9, 7, 11, 8, 6, 7, 9, 6, 0, -2, -7,
-29, -47, -46, -25, 3, 11, 11, 17, 25, 32, 33, 32, 25, 12, 2, -4, -10, -14, -12,
-11, -10, -11, -9, 1, 8, 9, 8, 5, 4, 4, 3, 1, 2, 2, -1, -3, -6, -23, -41, -47,
-31, -2, 13, 15, 16, 18, 22, 22, 20, 17, 14, 11, 10, 10, 9, 1, -18, -37, -29,
-6, 11, 16, 8, 3, -2, -33, -63, -59, -34, 0, 23, 26, 33, 42, 42, 36, 26, 13, 0,
-8, -12, -13, -12, -8, -4, -2, 3, 5, 2, 1, 3, 6, 11, 16, 17, 10, -2, -13, -16,
-8, -2, -2, -5, -11, -18, -21, -8, 9, 23, 28, 18, 0, -15, -21, -19, -8, -2, -3,
2, 8, 6, 2, 3, -11, -42, -60, -56, -12, 36, 44, 38, 33, 26, 19, 14, 11, 11, 11,
10, 3, -9, -27, -38, -19, 5, 11, 9, 1, -18, -38, -48, -44, -24, -7, -1, 11, 26,
36, 40, 38, 34, 25, 17, 9, 3, 0, 0, 0, -3, -10, -20, -27, -31, -32, -31, -24,
-15, -10, -5, -2, -2, 1, 8, 17, 28, 39, 46, 47, 39, 25, 9, -5, -17, -25, -27,
-23, -16, -9, -5, -3, -3, -5, -7, -9, -11, -13, -13, -13, -12, -9, -2, 8, 21,
33, 40, 40, 36, 25, 11, -5, -20, -31, -34, -29, -19, -7, 6, 17, 22, 22, 19, 11,
1, -9, -15, -18, -19, -18, -17, -12, -5, 0, 4, 5, 4, 2, 2, 5, 7, 7, 7, 6, 6, 5,
2, 1, 1, -1, -3, -4, -1, 2, 3, 7, 8, 3, -5, -13, -15, -12, -29, -52, -56, -43,
-17, 5, 20, 35, 46, 47, 40, 29, 15, 2, -6, -9, -10, -9, -7, -4, -2, 0, 2, 2, 0,
-5, -14, -23, -30, -33, -32, -27, -17, -4, 9, 22, 32, 35, 32, 25, 18, 13, 9, 7,
7, 5, -1, -9, -19, -30, -39, -43, -38, -25, -8, 8, 20, 28, 32, 29, 25, 22, 19,
18, 17, 16, 13, 7, -3, -16, -26, -33, -38, -38, -32, -21, -10, -1, 5, 8, 9, 9,
9, 10, 11, 11, 12, 13, 14, 11, 10, 11, 11, 11, 9, 2, -9, -18, -24, -33, -42,
-40, -31, -23, -11, 5, 13, 13, 16, 18, 8, 8, 24, 24, 10, -1, 1, 26, 32, -2, -15,
-4, -7, -12, -3, 2, -7, -9, 2, -3, -14, -8, -7, -30, -36, -17, -30, -54, 2, 80,
114, 75, -43, -73, -14, -3, -15, -6, 1, -5, -4, 3, 4, 1, 1, 2, 1, 1, 3, 6, 7, 4,
1, 0, 1, 2, 2, 1, -1, -2, -1, -2, -3, -3, -3, -4, -4, -2, 0, -1, -2, 0, 0, 0,
-1, -2, -2, -2, -1, -1, -2, -2, 0, 1, 2, 1, 0, 0, -2, -2, 0, -1, -1, -1, -2, -1,
-1, -3, -3, -2, -1, 1, 1, 0, 0, -1, -3, -3, -3, -3, 0, 1, 1, 1, 0, -1, -1, -1,
-1, -1, -1, 0, 0, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1,
-1, 0, 0, -1, -1, -1, -1, -1, 0, 0, -1, -1, -1, -1, -1, 0, };

#endif /* st_clap_H_ */