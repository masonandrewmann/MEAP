#ifndef s15_H_
#define s15_H_
 
#include <Arduino.h>
#include "mozzi_pgmspace.h"
 
#define s15_NUM_CELLS 8098
#define s15_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) s15_DATA [] = {-1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
-1, 0, 0, 1, 0, 0, 1, -1, 0, -1, -2, -1, -2, -3, -3, -5, -8, -12, -17, -24, -33,
-44, -54, -69, -84, -97, -109, -117, -122, -123, -121, -116, -110, -102, -92,
-81, -68, -52, -37, -19, -2, 15, 32, 49, 65, 81, 95, 106, 112, 117, 119, 120,
120, 118, 115, 115, 111, 110, 107, 105, 102, 100, 98, 95, 92, 89, 84, 81, 76,
72, 66, 61, 56, 51, 46, 41, 37, 33, 30, 27, 24, 23, 21, 20, 17, 16, 14, 13, 10,
8, 6, 4, 1, -2, -4, -8, -10, -13, -16, -20, -23, -27, -31, -34, -38, -42, -45,
-47, -50, -52, -54, -56, -58, -59, -61, -62, -63, -65, -66, -66, -67, -67, -66,
-66, -65, -63, -62, -60, -60, -59, -59, -60, -61, -61, -62, -61, -61, -60, -57,
-55, -54, -53, -56, -62, -70, -78, -88, -96, -102, -107, -109, -108, -106, -101,
-95, -87, -78, -69, -57, -47, -36, -26, -18, -10, -4, 0, 1, -2, -7, -13, -21,
-30, -38, -46, -51, -54, -54, -52, -47, -40, -32, -22, -11, 0, 8, 13, 17, 18,
17, 12, 7, 0, -7, -15, -21, -26, -31, -32, -31, -29, -25, -18, -9, 1, 12, 24,
37, 50, 62, 73, 84, 95, 105, 113, 118, 121, 123, 123, 121, 120, 118, 114, 112,
109, 107, 104, 103, 101, 98, 97, 95, 92, 90, 87, 84, 79, 76, 71, 67, 63, 59, 54,
50, 46, 41, 37, 32, 27, 23, 17, 12, 8, 4, 0, -3, -7, -12, -18, -23, -30, -36,
-44, -52, -59, -66, -72, -78, -83, -88, -90, -92, -94, -95, -95, -95, -93, -91,
-89, -86, -83, -80, -77, -74, -72, -68, -64, -60, -53, -47, -41, -35, -27, -19,
-13, -5, 1, 7, 12, 16, 21, 25, 28, 31, 33, 36, 40, 42, 45, 49, 54, 57, 61, 65,
70, 73, 77, 79, 82, 84, 83, 82, 80, 75, 70, 63, 57, 50, 42, 35, 28, 24, 19, 16,
14, 13, 13, 14, 16, 19, 23, 26, 29, 33, 36, 38, 41, 43, 45, 45, 46, 45, 44, 41,
38, 33, 27, 22, 16, 9, 3, -3, -7, -13, -16, -21, -23, -26, -28, -31, -31, -31,
-32, -31, -31, -29, -27, -24, -21, -17, -14, -11, -8, -4, -1, 3, 7, 10, 12, 15,
17, 20, 22, 24, 27, 29, 31, 32, 32, 31, 28, 25, 20, 15, 8, 1, -6, -13, -20, -27,
-32, -36, -40, -42, -43, -46, -48, -49, -50, -53, -55, -57, -58, -61, -61, -62,
-63, -62, -63, -61, -59, -59, -56, -55, -50, -48, -43, -40, -35, -31, -25, -20,
-15, -9, -5, -2, 3, 5, 8, 9, 9, 8, 7, 6, 4, 2, 0, 0, -1, -1, 0, 2, 4, 6, 9, 12,
13, 15, 15, 16, 15, 16, 15, 15, 14, 13, 12, 11, 11, 9, 6, 5, 3, -1, -1, -4, -5,
-7, -8, -8, -8, -8, -8, -8, -7, -7, -5, -3, -1, 1, 3, 5, 6, 6, 6, 4, 1, -1, -4,
-7, -10, -13, -16, -18, -20, -21, -21, -22, -21, -20, -19, -17, -15, -13, -10,
-8, -5, -2, 2, 5, 8, 11, 14, 17, 20, 22, 25, 26, 28, 29, 30, 32, 32, 33, 33, 34,
34, 33, 32, 30, 27, 24, 19, 13, 8, 1, -4, -10, -16, -21, -27, -31, -35, -39,
-41, -43, -43, -44, -43, -41, -40, -39, -36, -35, -33, -32, -31, -30, -29, -29,
-28, -27, -26, -25, -23, -23, -23, -24, -25, -26, -27, -28, -30, -32, -32, -34,
-33, -35, -35, -34, -34, -33, -31, -31, -30, -29, -27, -25, -23, -20, -18, -16,
-12, -9, -4, 1, 4, 9, 14, 17, 21, 25, 29, 30, 33, 34, 34, 35, 35, 36, 35, 34,
33, 32, 30, 29, 27, 26, 24, 23, 23, 21, 21, 20, 20, 19, 18, 17, 16, 14, 12, 10,
6, 3, -2, -6, -10, -14, -19, -22, -25, -28, -30, -30, -32, -32, -33, -32, -32,
-33, -32, -31, -31, -30, -29, -28, -27, -25, -23, -22, -20, -17, -15, -11, -8,
-5, -1, 4, 8, 11, 15, 17, 20, 21, 22, 22, 21, 20, 18, 17, 13, 11, 9, 7, 4, 0,
-2, -5, -7, -10, -12, -15, -16, -18, -20, -21, -22, -23, -22, -22, -21, -22,
-20, -18, -18, -16, -14, -13, -11, -9, -7, -6, -5, -3, -1, -1, 0, 0, 2, 3, 4, 4,
5, 5, 6, 6, 6, 6, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 3, 3, 4, 4, 6, 8, 12, 15,
20, 25, 29, 33, 37, 41, 44, 46, 47, 49, 49, 49, 48, 48, 45, 44, 42, 40, 37, 34,
29, 26, 22, 16, 12, 8, 4, 0, -5, -7, -10, -13, -16, -19, -21, -23, -25, -27,
-29, -31, -33, -34, -36, -37, -37, -38, -38, -40, -40, -40, -40, -39, -39, -37,
-36, -34, -32, -30, -27, -25, -22, -20, -18, -16, -16, -14, -14, -13, -14, -12,
-13, -13, -12, -11, -10, -9, -7, -5, -3, -2, 0, 2, 3, 4, 6, 7, 7, 9, 10, 11, 11,
11, 12, 11, 11, 12, 11, 10, 9, 8, 8, 7, 4, 4, 2, 1, -1, -3, -6, -7, -9, -11,
-14, -16, -17, -19, -20, -21, -21, -22, -21, -20, -19, -18, -18, -16, -16, -15,
-14, -14, -13, -14, -13, -13, -13, -13, -12, -12, -11, -11, -11, -11, -11, -11,
-12, -11, -11, -10, -8, -8, -7, -7, -7, -6, -6, -7, -8, -10, -10, -12, -13, -13,
-14, -15, -13, -13, -12, -10, -9, -7, -5, -3, -2, -1, 0, 0, 0, 0, -1, -2, -3,
-5, -6, -6, -7, -8, -9, -10, -9, -9, -10, -9, -8, -8, -8, -7, -8, -9, -10, -11,
-13, -15, -18, -20, -23, -25, -27, -30, -32, -33, -34, -35, -34, -35, -34, -34,
-33, -32, -32, -31, -29, -26, -26, -23, -21, -19, -16, -13, -13, -10, -9, -7,
-6, -5, -4, -3, -2, 0, 1, 2, 4, 7, 7, 11, 14, 16, 18, 23, 25, 27, 30, 32, 33,
34, 34, 34, 33, 31, 29, 29, 26, 23, 21, 19, 16, 14, 12, 9, 7, 6, 4, 3, 1, -1,
-2, -3, -5, -7, -9, -11, -13, -15, -17, -17, -18, -19, -20, -19, -19, -19, -18,
-18, -18, -18, -19, -20, -21, -23, -25, -27, -28, -30, -32, -32, -33, -33, -32,
-31, -29, -27, -25, -23, -21, -20, -18, -17, -15, -15, -14, -15, -14, -15, -14,
-15, -15, -14, -14, -13, -11, -9, -8, -4, -2, 1, 6, 10, 14, 18, 22, 25, 26, 28,
27, 28, 25, 23, 21, 18, 14, 13, 10, 8, 7, 6, 5, 3, 4, 5, 6, 6, 6, 8, 9, 10, 11,
13, 14, 15, 15, 14, 13, 12, 10, 7, 3, 0, -4, -8, -11, -15, -18, -21, -23, -25,
-27, -28, -29, -30, -31, -33, -33, -34, -34, -35, -35, -35, -33, -31, -28, -25,
-22, -19, -16, -10, -6, 0, 4, 8, 12, 17, 19, 22, 24, 24, 24, 24, 23, 22, 21, 19,
17, 15, 13, 12, 10, 8, 7, 5, 4, 1, 1, -1, -4, -6, -7, -9, -11, -13, -15, -17,
-17, -17, -17, -17, -16, -13, -12, -10, -7, -4, -1, 2, 4, 6, 7, 7, 7, 6, 7, 5,
4, 2, 2, 1, 1, 1, 0, -1, 0, 0, 1, 1, 1, 2, 4, 5, 6, 8, 9, 11, 12, 14, 17, 18,
19, 20, 23, 23, 24, 25, 25, 26, 26, 26, 25, 24, 24, 22, 22, 20, 18, 17, 17, 15,
14, 13, 13, 12, 12, 13, 12, 13, 13, 13, 13, 14, 13, 13, 12, 11, 9, 7, 6, 5, 3,
2, 0, -1, -2, -3, -2, -2, -2, -1, -1, 0, 1, 2, 2, 2, 3, 3, 4, 3, 3, 2, 2, 1, 0,
-1, -1, -3, -5, -4, -5, -5, -6, -6, -5, -5, -4, -5, -5, -4, -4, -5, -5, -7, -7,
-8, -9, -9, -9, -10, -9, -8, -8, -7, -7, -7, -6, -3, -2, -2, 0, 2, 4, 4, 6, 8,
9, 9, 9, 9, 8, 7, 6, 5, 4, 2, 0, -1, -4, -5, -7, -8, -10, -11, -12, -12, -12,
-12, -13, -11, -11, -11, -8, -8, -7, -5, -4, -2, 0, 1, 3, 5, 7, 10, 12, 13, 15,
17, 19, 20, 22, 23, 23, 23, 24, 24, 24, 22, 22, 20, 18, 17, 15, 13, 11, 10, 9,
9, 8, 9, 8, 9, 11, 11, 12, 14, 16, 15, 16, 17, 17, 17, 16, 15, 14, 13, 12, 10,
8, 7, 6, 5, 4, 6, 6, 6, 7, 7, 9, 10, 10, 9, 10, 9, 9, 7, 6, 5, 3, 3, 2, 1, 1, 1,
2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 12, 13, 13, 13, 13, 12, 12, 10, 9, 8, 6, 4, 4,
2, 3, 2, 2, 3, 4, 3, 4, 5, 6, 6, 6, 6, 7, 8, 8, 9, 11, 12, 13, 14, 17, 18, 19,
20, 22, 24, 25, 26, 27, 29, 29, 29, 30, 30, 29, 28, 26, 25, 22, 20, 18, 15, 13,
12, 9, 9, 8, 8, 7, 8, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 21, 21, 23, 24, 24,
26, 26, 26, 24, 26, 25, 25, 25, 25, 26, 25, 25, 25, 25, 25, 25, 24, 23, 22, 21,
20, 19, 17, 17, 16, 17, 16, 16, 17, 18, 18, 20, 20, 21, 22, 22, 21, 21, 20, 18,
17, 15, 13, 11, 9, 7, 5, 3, 1, -1, -2, -3, -4, -4, -5, -5, -5, -5, -5, -6, -6,
-7, -6, -7, -9, -9, -11, -10, -10, -11, -10, -9, -8, -6, -5, -3, -1, 2, 4, 7, 9,
10, 11, 12, 13, 13, 13, 11, 11, 10, 9, 8, 7, 7, 6, 7, 7, 7, 7, 8, 8, 9, 10, 11,
11, 13, 12, 13, 12, 11, 9, 9, 8, 5, 3, 1, -1, -1, -2, -4, -3, -3, -2, -1, 0, 2,
3, 6, 6, 9, 11, 13, 13, 14, 15, 16, 16, 15, 15, 14, 14, 13, 12, 12, 11, 11, 10,
10, 9, 9, 9, 9, 8, 8, 7, 7, 6, 5, 3, 3, 2, 1, 1, 1, 1, 0, 0, -1, 0, 1, 3, 4, 6,
7, 9, 10, 12, 13, 13, 15, 14, 14, 15, 14, 14, 14, 14, 14, 15, 14, 15, 14, 13,
14, 14, 11, 11, 9, 8, 8, 6, 5, 4, 2, 2, 1, 0, 0, 0, -1, -1, 0, -1, -1, 0, 0, -2,
-2, -3, -4, -5, -6, -6, -5, -6, -5, -3, -3, -1, 0, 1, 3, 4, 6, 6, 6, 6, 6, 5, 4,
3, 1, 0, -2, -3, -3, -5, -6, -6, -6, -5, -3, -1, 0, 3, 5, 8, 9, 10, 12, 12, 12,
12, 11, 11, 10, 8, 6, 5, 3, 1, 0, -1, -3, -4, -4, -5, -5, -6, -6, -6, -6, -5,
-5, -4, -3, -2, -1, 1, 1, 2, 2, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 4, 3,
3, 4, 3, 3, 3, 4, 4, 4, 4, 3, 3, 3, 2, 2, 1, 0, 0, -1, -2, -2, -1, -2, -1, -1,
0, 0, 2, 2, 3, 2, 3, 2, 2, 1, 1, 1, -1, -2, -3, -4, -4, -4, -5, -5, -5, -6, -6,
-6, -4, -5, -3, -4, -3, -3, -3, -3, -3, -4, -4, -5, -5, -6, -6, -7, -8, -8, -9,
-8, -9, -7, -6, -5, -4, -3, -1, 0, 2, 1, 2, 3, 2, 2, 2, 3, 3, 3, 4, 3, 5, 6, 6,
8, 8, 10, 11, 12, 13, 13, 13, 14, 14, 13, 12, 12, 11, 9, 10, 9, 9, 9, 9, 8, 7,
7, 6, 6, 5, 5, 4, 3, 2, 0, -2, -3, -4, -6, -6, -8, -9, -9, -9, -10, -9, -9, -8,
-6, -5, -3, -1, 0, 1, 3, 3, 3, 4, 4, 3, 4, 2, 2, 1, 0, -1, -2, -3, -4, -5, -6,
-8, -8, -10, -11, -11, -13, -13, -15, -14, -16, -16, -16, -15, -16, -17, -16,
-15, -15, -14, -13, -12, -12, -12, -10, -10, -10, -11, -9, -10, -11, -11, -11,
-12, -13, -13, -14, -14, -15, -16, -16, -17, -16, -16, -16, -14, -12, -12, -10,
-8, -7, -7, -5, -5, -5, -6, -5, -5, -5, -6, -6, -6, -5, -5, -5, -4, -5, -4, -4,
-2, -2, -2, -1, -1, -2, -2, -4, -5, -6, -7, -8, -9, -11, -10, -12, -12, -11,
-11, -9, -9, -8, -6, -4, -3, -3, -1, -1, -1, -1, -1, -2, -2, -2, -2, -4, -4, -3,
-4, -5, -5, -5, -5, -4, -5, -5, -4, -5, -6, -7, -7, -7, -9, -10, -11, -11, -11,
-13, -14, -15, -15, -16, -17, -17, -18, -18, -19, -19, -20, -20, -20, -20, -19,
-19, -19, -19, -17, -17, -16, -15, -14, -14, -13, -13, -12, -12, -11, -11, -10,
-10, -10, -11, -9, -10, -9, -9, -9, -8, -8, -7, -6, -5, -4, -4, -3, -1, -1, 0,
1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, -1, 0,
-1, -3, -5, -6, -7, -7, -8, -8, -8, -9, -9, -7, -6, -6, -5, -3, -2, 0, 0, 0, 0,
1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -2, -2, -3, -3, -3, -4, -6, -7, -8, -8,
-9, -10, -11, -12, -12, -13, -13, -12, -12, -12, -10, -9, -8, -8, -7, -7, -7,
-6, -5, -6, -5, -5, -5, -5, -5, -5, -4, -5, -5, -4, -4, -3, -4, -4, -3, -3, -4,
-3, -4, -5, -4, -5, -6, -6, -7, -8, -8, -8, -9, -9, -9, -10, -9, -8, -8, -8, -8,
-7, -6, -5, -5, -4, -3, -3, -1, -1, 0, 0, 1, 2, 2, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6,
6, 6, 5, 5, 5, 5, 4, 5, 4, 3, 3, 3, 2, 3, 1, 1, 2, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6,
7, 7, 7, 8, 8, 6, 5, 4, 2, 1, -1, -2, -3, -4, -6, -7, -7, -9, -10, -11, -12,
-11, -13, -12, -12, -12, -11, -10, -9, -9, -9, -8, -6, -6, -5, -5, -4, -3, -3,
-2, -2, -1, 0, -1, 0, 0, 0, 0, -2, -3, -4, -6, -7, -8, -10, -12, -13, -13, -15,
-14, -13, -13, -12, -10, -8, -6, -5, -2, 1, 4, 5, 7, 9, 11, 13, 14, 14, 15, 15,
15, 14, 14, 13, 12, 11, 11, 9, 10, 8, 8, 8, 7, 8, 7, 7, 7, 6, 5, 4, 4, 2, 2, 0,
0, -2, -2, -2, -2, -3, -2, -2, -1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, -2, -3, -4,
-5, -5, -7, -7, -9, -10, -10, -12, -11, -11, -11, -11, -10, -10, -9, -8, -7, -7,
-6, -5, -5, -3, -2, -2, -2, -2, -2, -1, 0, -1, 0, 0, -1, -2, -2, -3, -3, -4, -5,
-5, -7, -7, -8, -9, -8, -9, -9, -9, -11, -10, -9, -10, -10, -9, -8, -8, -8, -7,
-7, -6, -7, -6, -6, -5, -5, -4, -5, -4, -3, -3, -3, -2, -1, -1, 0, 0, 1, 3, 3,
3, 4, 3, 3, 3, 2, 2, 1, 2, 1, 1, 2, 1, 2, 2, 2, 4, 5, 6, 7, 8, 9, 9, 10, 12, 12,
12, 12, 13, 12, 12, 12, 12, 10, 8, 8, 5, 4, 3, 2, 1, 1, 1, 0, 0, 1, 0, 2, 2, 2,
3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 7, 7, 7, 7, 8, 8, 7, 7, 8, 7, 7, 7, 7, 7, 6, 6, 6,
6, 5, 4, 3, 3, 3, 2, 1, 1, 1, 1, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
0, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, -1, 0, 1, 0, 1, 1, 2, 2, 2, 2, 3, 4, 4, 6,
6, 9, 9, 9, 11, 12, 13, 14, 15, 15, 16, 17, 17, 18, 18, 17, 17, 17, 16, 17, 16,
16, 15, 15, 14, 14, 14, 14, 13, 12, 11, 11, 11, 11, 11, 11, 12, 13, 13, 14, 13,
15, 14, 16, 16, 17, 17, 16, 17, 18, 17, 17, 18, 17, 16, 15, 14, 13, 12, 10, 8,
6, 5, 3, 1, -1, -1, -3, -3, -4, -4, -4, -5, -5, -4, -5, -5, -6, -5, -6, -5, -5,
-5, -5, -5, -6, -7, -7, -8, -8, -9, -9, -10, -9, -8, -9, -7, -6, -6, -5, -4, -3,
-3, -2, -3, -2, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -7, -7, -7, -7, -8, -7,
-7, -7, -6, -5, -5, -4, -4, -3, -2, 0, 0, 1, 3, 3, 4, 5, 7, 6, 7, 8, 7, 7, 6, 5,
4, 3, 1, 1, 0, 1, 1, 1, 1, 2, 3, 3, 4, 7, 8, 9, 11, 13, 15, 16, 16, 17, 18, 18,
16, 16, 15, 14, 14, 13, 12, 12, 12, 12, 11, 11, 11, 12, 11, 11, 10, 11, 10, 10,
10, 9, 10, 9, 8, 8, 8, 7, 7, 5, 5, 4, 4, 4, 3, 4, 2, 3, 2, 3, 2, 2, 2, 1, 1, 0,
-1, -2, -3, -4, -4, -5, -4, -4, -5, -4, -4, -4, -3, -3, -2, -3, -2, -3, -3, -2,
-2, -3, -3, -4, -4, -5, -6, -7, -6, -7, -7, -6, -8, -7, -6, -5, -5, -5, -4, -4,
-3, -3, -3, -3, -2, -2, -2, -2, -1, -2, -1, -1, -1, 0, 2, 1, 3, 4, 4, 5, 6, 6,
7, 6, 7, 6, 6, 5, 4, 2, 2, 1, 1, 0, -1, -1, -2, -2, -2, -1, -2, -3, -1, -2, -2,
-2, -2, -3, -3, -3, -4, -3, -4, -4, -4, -4, -4, -5, -5, -5, -5, -5, -5, -4, -4,
-4, -3, -4, -3, -3, -3, -4, -3, -3, -3, -3, -2, -2, -2, -2, -2, 0, 0, 0, 1, 2,
3, 3, 4, 6, 7, 6, 8, 9, 9, 10, 9, 10, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10,
10, 10, 9, 9, 8, 8, 8, 9, 8, 7, 7, 6, 5, 5, 4, 4, 3, 1, 1, 0, -1, -2, -2, -3,
-3, -3, -2, -2, -2, -1, -1, -1, 0, 1, 1, 1, 2, 1, 1, 2, 2, 2, 1, 1, 1, 1, 0, 0,
0, -1, -1, 0, -1, -1, -1, 0, 0, 0, 1, 1, 0, 0, -1, -2, -2, -1, -3, -2, -3, -2,
-3, -3, -3, -3, -3, -3, -2, -2, -1, -1, -1, 0, -1, -1, 0, 0, 0, 0, 0, -1, 1, 1,
0, 1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1, -1, -2, -2, -2, -2, -2,
-1, -1, 0, 0, 1, 1, 2, 1, 2, 2, 2, 3, 3, 3, 4, 5, 4, 4, 5, 5, 5, 6, 6, 7, 7, 7,
7, 7, 7, 6, 6, 5, 6, 5, 3, 2, 1, 0, 1, -1, -1, -2, -3, -3, -4, -6, -5, -6, -7,
-8, -8, -10, -11, -11, -12, -13, -15, -16, -16, -16, -17, -17, -17, -17, -16,
-16, -15, -15, -14, -13, -13, -12, -11, -11, -10, -9, -9, -10, -9, -9, -9, -8,
-9, -8, -8, -8, -8, -7, -6, -6, -6, -6, -6, -5, -6, -5, -5, -5, -5, -5, -6, -6,
-5, -5, -6, -6, -6, -7, -7, -8, -8, -9, -9, -9, -9, -9, -8, -8, -6, -6, -4, -4,
-2, -1, 0, 0, 1, 2, 3, 2, 3, 4, 3, 4, 4, 4, 5, 4, 3, 2, 2, 3, 1, 1, 1, 0, -1,
-1, -1, -1, -1, -1, 0, -1, -1, 0, 0, 0, 0, 2, 2, 3, 4, 5, 4, 6, 7, 8, 9, 9, 10,
10, 11, 11, 11, 10, 11, 8, 8, 7, 5, 3, 2, 1, 0, -2, -3, -4, -4, -5, -6, -6, -7,
-7, -7, -6, -7, -6, -6, -6, -7, -6, -6, -7, -7, -7, -7, -9, -9, -9, -9, -9, -10,
-9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -10,
-9, -10, -9, -10, -9, -8, -9, -9, -8, -8, -7, -6, -6, -5, -4, -2, -2, -1, -1,
-1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, -1, -1, -2, -1, -2, -2, -3, -2, -2,
-2, -2, -1, -2, -2, -1, -1, -1, -1, -1, -2, -1, -2, -2, -2, -2, -2, -4, -4, -3,
-3, -3, -2, -1, -1, -1, -1, -1, 0, 1, 1, 2, 2, 2, 3, 2, 1, 2, 1, 0, 1, -1, 1, 0,
0, 2, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 0, -1, -3, -3, -4, -5, -4,
-5, -5, -6, -6, -7, -7, -8, -7, -7, -8, -9, -9, -9, -10, -9, -9, -9, -9, -9, -9,
-9, -8, -8, -8, -8, -7, -8, -8, -8, -8, -8, -8, -9, -9, -8, -8, -7, -8, -8, -7,
-8, -7, -7, -7, -7, -8, -8, -7, -6, -6, -7, -7, -6, -7, -7, -7, -7, -6, -6, -6,
-5, -5, -4, -4, -2, -1, 0, 1, 1, 1, 1, 1, 0, 1, 0, -1, -1, -1, -3, -3, -3, -3,
-3, -3, -2, -2, -1, 0, -1, 0, 0, -1, 0, 0, -1, -1, -1, -2, -2, -1, -2, -3, -3,
-3, -4, -4, -5, -5, -4, -4, -3, -3, -2, -2, -1, 0, 0, 0, 2, 1, 2, 2, 1, 3, 2, 1,
2, 1, 2, 2, 2, 1, 3, 2, 2, 1, 2, 2, 2, 1, 2, 1, 1, 1, 1, 0, 0, -1, -2, -2, -3,
-3, -3, -2, -3, -2, -3, -2, -2, -1, -2, -1, -2, -1, -1, -2, -2, -3, -2, -4, -4,
-5, -5, -5, -6, -6, -7, -7, -7, -7, -8, -7, -8, -8, -9, -8, -8, -8, -7, -7, -6,
-5, -5, -5, -5, -3, -4, -3, -4, -4, -4, -5, -5, -6, -7, -6, -6, -6, -6, -6, -5,
-5, -4, -2, -2, -1, -1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 6, 7, 7,
9, 9, 8, 9, 8, 8, 6, 6, 6, 5, 3, 2, 1, 0, 0, -1, -2, -2, -2, -3, -2, -3, -2, -2,
-2, -1, -2, -2, -1, -1, -2, -2, -1, -1, -1, -2, -2, -1, -1, -1, -1, -1, 0, -1,
0, 0, 1, 1, 2, 2, 3, 2, 3, 2, 2, 2, 2, 2, 1, 1, 0, 0, -1, -1, -1, -1, -1, -1, 0,
1, 1, 2, 2, 4, 4, 4, 5, 4, 5, 4, 3, 4, 3, 3, 3, 2, 1, 2, 1, 1, 2, 1, 2, 0, 1, 1,
-1, 0, -2, -3, -4, -4, -4, -5, -4, -5, -5, -4, -3, -2, -2, -1, 1, 1, 2, 4, 4, 5,
4, 5, 5, 4, 5, 5, 5, 4, 4, 4, 4, 4, 5, 4, 5, 4, 5, 4, 6, 5, 4, 4, 3, 3, 2, 2, 1,
0, 0, 0, -1, -1, -1, -2, -1, -2, -1, -1, 0, 0, 0, 1, 1, 1, 2, 3, 4, 3, 3, 3, 4,
5, 5, 4, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 3, 2, 1, 2, 1, 0, 0, 0, 0, -1, -1,
-2, -2, -1, -2, -2, -1, 0, 0, 0, 1, 0, 1, 2, 1, 1, 2, 2, 3, 4, 4, 4, 4, 5, 5, 5,
5, 5, 6, 5, 6, 6, 6, 7, 7, 7, 8, 7, 8, 8, 8, 8, 9, 8, 8, 8, 7, 6, 6, 6, 6, 5, 4,
3, 4, 4, 3, 3, 3, 3, 3, 2, 2, 3, 3, 3, 4, 4, 4, 3, 4, 3, 2, 2, 1, 0, 0, 0, -1,
-1, -2, -2, -2, -2, -2, -1, -1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0,
0, 1, 0, 0, -1, 0, 0, -1, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 0, 0, 0,
-1, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 3, 4, 4, 4, 5, 4, 5, 6, 6, 6, 7, 7, 7,
7, 6, 7, 6, 6, 6, 5, 5, 5, 4, 3, 1, 1, -1, -1, -1, -3, -3, -3, -3, -4, -3, -4,
-4, -5, -4, -4, -4, -5, -4, -4, -4, -4, -4, -4, -3, -2, -1, 0, 0, 0, 1, 1, 0, 0,
0, 0, 0, 0, 0, -1, -1, -2, -1, -1, -2, -1, -1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 2,
2, 2, 1, 2, 2, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 5, 4, 4, 4, 3, 4, 4, 4, 3, 2, 3, 3,
3, 3, 3, 2, 3, 2, 3, 3, 3, 3, 4, 3, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 3, 4, 3, 3, 3,
2, 3, 2, 3, 2, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, -1, -1, -2, -2, -3, -2, -3, -3,
-3, -3, -3, -3, -2, -2, -1, -1, 0, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 4, 4, 5, 4,
5, 6, 6, 5, 5, 4, 4, 3, 4, 4, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 2, 3, 2, 2, 1, 1, 1,
2, 2, 1, 2, 3, 3, 4, 4, 4, 5, 5, 4, 5, 5, 6, 6, 6, 5, 5, 5, 4, 3, 3, 2, 3, 2, 3,
2, 3, 2, 3, 2, 2, 1, 2, 0, 0, 0, -2, -2, -3, -4, -4, -5, -5, -5, -4, -6, -5, -5,
-4, -5, -4, -4, -3, -2, -3, -2, -2, -1, -2, -1, -1, -2, -1, -1, 0, 0, 1, 1, 1,
1, 2, 3, 2, 3, 3, 3, 4, 3, 3, 4, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 2, 3, 2, 4, 3, 4,
4, 3, 3, 4, 3, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2, 1, 2, 2, 4, 3, 3, 3, 2, 3,
3, 2, 2, 1, 1, 0, 0, 1, 0, 0, 0, -1, -1, -1, -1, -1, -2, -2, -3, -2, -4, -3, -4,
-4, -5, -3, -4, -4, -5, -4, -4, -4, -3, -4, -4, -3, -2, -2, -1, -2, -2, -1, -1,
-2, -1, -2, -2, -2, -2, -2, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3, -3, -2,
-2, -2, -2, -1, -1, 0, -1, -1, -1, 0, -1, 0, -2, -1, -2, -2, -2, -1, -1, 0, -1,
1, 0, 1, 1, 1, 0, 1, 0, 0, -1, -1, -1, -2, -2, -2, -1, -2, -2, -1, -2, -1, 0, 0,
1, 0, 1, 2, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 2, 3, 3, 2, 1,
2, 1, 0, 0, 0, -1, -2, -2, -3, -3, -4, -3, -4, -4, -4, -4, -3, -4, -4, -4, -4,
-5, -4, -5, -5, -5, -5, -5, -5, -4, -5, -4, -5, -4, -4, -4, -3, -3, -3, -3, -2,
-2, -3, -2, -2, -1, -1, 0, 0, 0, -1, 0, -1, -1, -1, -2, -3, -3, -3, -2, -2, -1,
-1, -1, 0, 0, 1, 1, 1, 1, 2, 2, 2, 1, 0, -1, -1, -2, -3, -4, -5, -5, -5, -5, -4,
-4, -3, -2, -2, 0, 0, 2, 2, 2, 3, 2, 2, 2, 1, 1, 0, -1, -1, -2, -1, -2, -3, -3,
-3, -3, -2, -2, -2, -2, -2, -1, -1, -2, -2, -2, -2, -2, -3, -2, -3, -4, -5, -4,
-5, -5, -5, -5, -5, -4, -4, -3, -3, -2, -3, -3, -2, -1, 0, -1, -1, 0, 0, 0, 0,
-1, 0, 0, -1, -1, -2, -2, -1, -1, 0, 0, 1, 1, 2, 2, 1, 0, 1, -1, -1, -2, -2, -3,
-3, -4, -3, -3, -3, -3, -2, -2, 0, 0, 1, 1, 2, 1, 1, 1, 0, 0, -1, -2, -2, -3,
-3, -4, -5, -4, -4, -4, -4, -4, -3, -2, -2, -1, 0, -1, 0, -1, -1, -2, -2, -3,
-4, -5, -4, -5, -5, -4, -5, -5, -5, -4, -3, -2, -2, -1, 0, -1, -1, 0, 0, 0, 0,
1, -1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -2, -1,
-1, -2, -1, -3, -3, -4, -3, -4, -5, -5, -5, -5, -6, -5, -5, -4, -5, -4, -4, -4,
-4, -5, -4, -3, -4, -4, -4, -3, -3, -3, -3, -3, -4, -3, -3, -3, -3, -4, -3, -4,
-4, -3, -3, -3, -3, -2, -3, -2, -2, -1, -1, -1, 0, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2,
1, 2, 1, 2, 1, 1, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 0, 0, -1, -1, -2, -3, -2, -3,
-4, -4, -4, -4, -5, -6, -6, -5, -7, -7, -6, -6, -5, -6, -7, -7, -6, -6, -6, -8,
-7, -6, -7, -7, -6, -6, -5, -5, -5, -5, -4, -4, -3, -3, -2, -3, -2, -1, -1, -2,
-1, 0, -1, 0, 0, 0, 1, 0, 0, 1, 2, 2, 2, 3, 3, 2, 4, 4, 5, 4, 5, 5, 6, 5, 6, 5,
6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 2, 2, 2, 3, 2, 2, 2, 3, 3, 1, 1, 2, 2, 1,
1, 1, 0, -1, -1, -1, -2, -3, -3, -3, -4, -4, -4, -4, -4, -4, -3, -3, -3, -2, -3,
-2, -2, -1, -2, -3, -3, -4, -4, -4, -5, -5, -7, -6, -6, -6, -6, -6, -5, -4, -3,
-2, -2, -1, -1, -1, -1, -1, 0, 0, 1, 0, 1, 0, 0, 0, 0, -1, -1, 0, -1, -1, 0, 0,
-1, 0, -1, -1, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3, -2, -3, -2, -2, -1,
-1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 3, 3, 3, 3, 3, 4, 4, 5, 5, 6, 5,
6, 6, 7, 7, 6, 5, 6, 5, 6, 5, 4, 4, 3, 4, 2, 1, 2, 0, 0, -1, -1, -1, -1, -1, -1,
-1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3,
-3, -4, -4, -5, -4, -5, -5, -5, -4, -5, -5, -4, -5, -5, -6, -6, -6, -6, -6, -5,
-5, -5, -5, -5, -4, -4, -3, -4, -4, -3, -3, -1, -1, -1, 0, 0, 1, 0, 1, 0, 1, 1,
1, 1, 1, 1, 1, 2, 3, 3, 3, 3, 4, 3, 4, 3, 3, 4, 4, 3, 3, 3, 4, 3, 4, 4, 4, 4, 5,
5, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 3, 3, 3, 2, 1, 0, 0, -1, -2, -2, -1, -1, -2,
-2, -2, -1, -2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 1, 2, 2, 3, 3, 3, 4, 5, 5, 5,
6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 5, 5, 5, 4, 4, 3, 2, 1, 0, 0, 1, -1, -1, -1, -1,
0, -1, 0, 1, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1,
0, 0, 0, 1, 2, 1, 2, 1, 1, 2, 2, 3, 2, 1, 1, 2, 1, 1, 1, 1, 0, -1, 0, -1, -1,
-1, -1, -1, -1, 0, -1, 0, 0, 1, 2, 2, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
1, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 2,
1, 2, 2, 3, 2, 2, 2, 2, 3, 3, 3, 4, 4, 3, 3, 3, 3, 4, 3, 2, 2, 1, 2, 1, 1, 2, 1,
1, 0, 2, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 3, 4, 4, 3, 4, 4, 4, 4, 4, 5, 4, 4, 4, 5,
5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 4, 4, 3, 2, 2, 1, 0, 0, 0, -1, -1, -2, -2, -2, -3,
-2, -2, -4, -3, -3, -3, -4, -3, -3, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4,
-3, -3, -3, -3, -3, -3, -3, -3, -2, 0, 0, 1, 2, 1, 2, 3, 2, 2, 1, 1, 0, 1, 0, 1,
1, 0, 1, 1, 1, 1, 1, 1, 2, 2, 3, 2, 2, 2, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 2, 2, 4,
4, 4, 5, 6, 5, 5, 6, 6, 6, 6, 5, 5, 4, 5, 4, 4, 3, 4, 3, 3, 3, 4, 4, 4, 5, 6, 6,
7, 6, 7, 6, 6, 7, 7, 5, 5, 5, 4, 4, 4, 4, 4, 2, 2, 3, 3, 2, 1, 1, 2, 3, 1, 2, 2,
3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 0, 1, 0, -1, 0, 1, 0, 0, 0, 0, 0, -1, -1,
-2, -2, -2, -2, -3, -4, -4, -4, -3, -3, -3, -3, -2, -2, -2, -1, 0, -1, -1, -1,
-1, -1, -2, -2, -2, -3, -4, -4, -3, -3, -3, -3, -3, -2, -1, 0, 1, 2, 2, 3, 3, 3,
3, 3, 3, 2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 2, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 4,
5, 4, 4, 3, 2, 2, 2, 3, 2, 2, 2, 2, 2, 3, 2, 2, 1, 2, 2, 2, 1, 1, 1, 1, 0, 1, 0,
0, 0, -1, -1, -2, -2, -1, -1, -2, -1, -1, 0, 0, 1, 1, 1, 1, 2, 3, 3, 3, 2, 3, 3,
3, 3, 3, 2, 2, 3, 2, 2, 2, 3, 3, 2, 3, 2, 2, 3, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0,
1, 0, 1, 1, 0, 0, -1, 0, -1, 0, -1, 0, -2, -2, -2, -2, -1, -2, -2, -2, -1, -1,
-1, -1, -1, 0, -1, 0, -1, -1, -1, -1, -1, 0, 0, -2, -1, -2, -1, -2, -1, -1, 0,
0, -1, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4,
3, 4, 4, 2, 3, 3, 2, 3, 3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, -1, 0, 0, 0, 0, -1, 0,
-1, 0, -1, 0, 0, -1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1, -1, -1, 0, -1, 0,
-1, -1, 0, -1, 0, -1, 0, 0, 0, -1, -2, -1, -1, -2, -2, -2, -3, -2, -2, -2, -3,
-2, -2, -2, -1, -1, -1, -1, -1, -2, -2, -2, -2, -3, -3, -4, -4, -4, -4, -4, -5,
-5, -5, -5, -4, -4, -4, -4, -4, -3, -4, -3, -4, -3, -3, -2, -2, -3, -2, -2, -2,
-2, -2, -2, -1, -2, -2, -2, -2, -2, 0, -1, -1, 0, -1, 0, 0, 0, 1, 2, 2, 2, 1, 2,
2, 2, 2, 2, 2, 2, 3, 1, 3, 2, 1, 3, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 1, 1, 1, 0, 1,
0, 0, -1, -1, -2, -1, -3, -2, -2, -2, -1, -2, -2, -1, -2, -1, -1, -1, -1, -1,
-1, -1, -2, -1, -2, -2, -1, -1, -2, -2, -2, -2, -2, -2, -2, -3, -2, -3, -2, -3,
-3, -2, -2, -3, -2, -2, -2, -3, -3, -2, -3, -3, -2, -3, -3, -3, -2, -3, -3, -2,
-2, -2, -1, -2, -2, -3, -2, -2, -2, -2, -3, -3, -2, -2, -2, -3, -2, -3, -3, -3,
-2, -2, -2, -2, -3, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1, -2, -2, -2, -2,
-2, -2, -1, 0, 0, -1, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -2, -1, -1, -2, -1,
0, 0, 0, 0, 0, -1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0,
-1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -3, -2, -3, -2, -3, -3, -3, -3, -3,
-3, -2, -3, -3, -2, -2, -3, -4, -3, -3, -3, -4, -3, -4, -4, -3, -4, -3, -3, -3,
-2, -3, -3, -3, -3, -3, -2, -2, -2, -1, -2, -1, -1, -1, -1, -1, -1, -2, -2, -1,
-1, -1, -1, -2, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, 0, -1, -1, 0, -1, -1, 0, -1,
-1, -1, -2, -1, -1, -1, -1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 1, 1,
1, 1, 1, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, 0, 0, 0, 0, -1, -1, -1, -2, -1, -1,
-1, -1, -2, -2, -2, -2, -2, -2, -2, -3, -2, -2, -2, -2, -3, -3, -2, -2, -3, -4,
-4, -4, -3, -3, -3, -3, -3, -3, -2, -3, -3, -3, -2, -2, -2, -3, -4, -3, -2, -3,
-3, -2, -3, -2, -1, -2, -1, -1, -2, -2, -2, -1, 0, -1, -1, -1, 0, 0, 0, -1, -1,
-1, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1,
-1, -1, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, -1, 0, 0, 1, 0, 1, 0,
0, -1, 0, -1, -1, -1, -1, -2, -3, -3, -3, -2, -2, -2, -2, -2, -1, 0, 0, 0, 1, 1,
0, 1, 0, -1, -2, -1, -2, -1, -2, -1, -1, -2, -1, -1, 0, 1, 1, 1, 1, 1, 1, 2, 1,
1, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, 0, 0, -1, 0, 0, 0, 0, 1, -1, 1, 0,
-1, 0, -2, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -1, -2, -2,
-2, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, 0, 0, -1, -1, 0, 0, 0, -1, -1, 0, 1,
0, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 2, 1, 2, 2, 1, 1, 1, 2, 1, 1, 0, 0,
0, 0, 0, -1, 1, 1, 0, 1, 1, 2, 1, 2, 1, 1, 1, 0, 0, -1, -1, -1, -2, -1, -1, -2,
-1, -2, -1, -1, -1, 0, -1, 0, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 1, 2, 2, 1,
2, 1, 1, 2, 1, 2, 2, 2, 1, 2, 1, 1, 1, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 2, 1, 1,
0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 2, 1, 1, 1, 2,
1, 1, 1, 1, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, -2, -1, -1, 0,
-1, -1, -1, 0, 0, -1, 0, -1, 0, -1, 0, -1, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 3, 3, 2, 2, 2, 3, 4,
3, 4, 4, 4, 4, 3, 3, 4, 3, 4, 2, 3, 2, 2, 3, 3, 2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3,
3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 2, 2, 2, 1, 2, 2, 1, 1,
1, 1, 2, 1, 1, 1, 1, 0, 1, 0, 0, 0, -1, -2, -1, -2, -1, -1, -2, -1, -1, -2, -1,
-2, -1, -1, -1, -1, -1, -1, -2, -1, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -1,
-2, -2, -2, -1, -2, -1, -2, -1, -1, -1, -2, -1, -1, 0, 0, 0, 0, -1, 0, 1, 1, 0,
2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 1, 2, 3, 1, 2, 2, 1, 2, 2, 1, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 3, 2, 2, 3,
3, 3, 2, 2, 2, 1, 2, 2, 2, 2, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
1, 0, 1, 1, 2, 0, 1, 0, 0, 0, 0, 0, 1, -1, -1, 0, -1, 0, 0, 0, 0, -1, -1, 0, -1,
-1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2,
-1, -1, -2, -1, -1, -1, -1, -1, 0, -1, -1, 0, -1, 0, -1, 1, 0, 1, 0, 0, 1, 1, 0,
2, 1, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 1, 2, 1, 1, 2, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1, 0, 1, 1, 2, 1, 2, 1, 1,
0, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0,
0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1,
-1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, -1, -1, -1, 0, 0,
0, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, -1, 0, 1, 0, 0, 0, -1, -1, 0, -1, 0, 0,
0, -1, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0,
0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 1, -1, 1, 0, 0, 1, 0, 0, -1,
0, 1, -1, 0, 1, 1, 0, 1, 0, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0,
1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1,
-1, -1, -1, -1, -1, -1, -1, 0, -1, -1, 0, -1, -1, 0, -1, 0, 0, -1, 0, -1, -1,
-1, -1, -1, 0, -1, 0, -1, 1, 0, 0, 0, 0, -1, 0, -1, 0, -1, -1, -1, 0, 0, -1, -1,
0, 0, -1, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 1, 0, 1, 1, 0, 0,
0, 0, 1, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1, 0, -1, -1, -1, -1, 0,
-1, 0, -1, -1, -1, -2, -1, 0, 0, -1, -1, -1, -1, 0, -1, -1, 0, -1, -1, -1, -1,
-1, -1, 0, -1, -1, -1, 0, 0, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, -1,
-1, 0, -1, 0, 0, -1, -1, 0, 0, -1, -1, -2, -1, 0, -1, -1, -1, -2, -1, -1, -1, 0,
0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, 0, -1, 0, -1, -1, 1, -1, 0, 0, 0, 0, 0, 0,
0, 1, 1, 1, 0, 1, -1, 0, 0, 0, 0, -1, -1, 0, -1, -1, 0, -1, -1, 0, 0, -1, 0, 0,
0, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, 0, -1, -1, 0, 0, 0,
0, -1, 1, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0,
-1, 0, 0, 0, -1, -1, 0, 0, -1, 0, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1, -1, 0, -1,
-1, 0, 0, 0, 0, 0, 0, 0, -1, 1, 0, -1, 0, 0, 0, -1, -1, 0, -1, 1, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, -1, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, 0, -1,
-1, -1, 0, -1, -1, 0, 0, -1, -1, -1, -1, 0, 0, 0, -1, -1, -1, 0, -1, -1, 0, -1,
0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, 1, -1, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, -1, -1, 0, -1, -1, 0, 0, 0, 0, 1, -1, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, -1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, -1, 1, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0,
0, -1, -1, 0, 0, 1, 0, 0, 0, 1, 0, -1, -1, 0, 1, -1, 0, -1, -1, 0, -1, 0, 0, 0,
0, 0, 0, 0, 0, 1, -1, 0, -1, 0, 0, 0, -1, 1, 0, -1, 0, 0, 0, 0, 0, 1, -1, 0, -1,
0, 0, 0, -1, -1, 0, -1, 0, 0, 0, 0, 1, 0, -1, -1, 0, -1, -1, 1, 0, 0, 0, -1, 0,
0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, -1, -1, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1,
0, 0, 1, -1, 0, 0, 0, 0, 0, -1, 0, -1, -1, 0, 0, 0, 0, 0, 0, 1, -1, 0, -1, 1, 0,
0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0,
0, 0, 0, 0, 0, 1, 0, 0, -1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, -1, 0,
1, 1, 0, 0, 0, -1, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, -1, -1, 0, 1, 1, -1, 0, 0, 0, 0, 0, 0,
0, 1, 0, };

#endif /* s15_H_ */
