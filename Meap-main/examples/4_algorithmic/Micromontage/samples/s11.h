#ifndef s11_H_
#define s11_H_
 
#include <Arduino.h>
#include "mozzi_pgmspace.h"
 
#define s11_NUM_CELLS 3061
#define s11_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) s11_DATA [] = {0, 0, 0, 1, -1, -1, -1, 0, -1, -1, 1,
0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 1, 0, 0,
-1, 0, 0, -1, 0, 1, 0, -1, -1, 0, 0, 0, -1, -2, 0, 2, 2, 1, 0, 1, 0, -1, -2, 0,
-1, -2, -2, -1, 2, 5, 5, 1, -1, 2, 3, -3, 1, 0, -3, -4, -1, 7, 5, 4, -2, -6, -3,
-1, -1, -5, 2, 8, 4, 4, -4, -8, -9, -4, -1, -3, -4, -2, 12, 10, 4, -2, 0, 2, -3,
-6, 2, 7, -6, -4, -4, -4, 1, 1, 1, 8, 1, -2, 0, -5, -3, -8, -1, 1, -2, -5, 7,
11, -5, -17, -7, 17, 15, 11, 8, 12, -5, -16, -5, -2, 7, 0, -1, -2, -3, 2, -2, 1,
-1, -4, -13, -13, -12, 1, 11, 10, 3, 0, -5, -13, -18, -21, -3, 9, 17, 6, 0, -3,
2, 9, 16, 13, 0, 0, -10, -7, -7, -12, -8, -1, 2, 3, 10, 8, -4, 1, 3, -1, -5, -9,
-1, 3, 6, 5, 5, -2, -4, -1, -3, -1, -3, 4, -1, 0, -1, 4, 8, 4, 3, -1, -2, -5,
-4, -1, -1, -2, 0, 0, 1, 1, 1, -1, -1, 1, -1, 1, 3, 3, 1, -1, -1, -1, 0, -1, 1,
2, 2, 3, 3, 1, 0, 0, -1, -1, -1, 0, 1, 2, 2, 1, 0, 0, 0, 1, 0, 2, 1, 2, 2, 3, 2,
1, 0, 0, -1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 2, 1, 1, 0, -1, -1, -1, 0,
1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, -1, 0, 0, 0, 1, 2, 2, 0, 1,
1, -1, 2, 1, 0, 1, 3, 3, -1, -2, -2, 1, 4, 1, 0, 0, 2, 4, -2, -5, -2, 2, 3, 1,
-2, 1, -2, -6, 3, 3, 5, 3, 0, -4, -6, -6, -6, -1, -1, 7, 10, 4, 6, 0, -4, -9,
-8, 2, 9, 13, -8, -7, 5, 6, -6, -18, -10, 2, 9, 18, 13, -5, -9, 6, -10, -10, -1,
-3, -1, 0, 7, 2, -1, -7, 23, 25, 3, -25, -40, -20, 14, 28, 15, 5, -1, -4, -15,
-23, -7, 7, 0, 12, 5, 4, -15, -15, 1, -1, -1, 4, 14, 12, 17, -2, -17, -29, -18,
-3, -7, 9, 25, 24, 19, 1, -18, -23, -25, 0, 7, 3, 9, 20, 19, 2, -10, -11, -13,
-9, 2, -6, -1, 10, 9, 9, -7, -11, -13, -10, 0, 10, 8, -5, 7, 8, 13, -1, -12,
-18, -15, 2, 2, -3, -2, 11, 4, 2, -7, -3, -3, 0, 0, -4, 4, 5, 10, 1, -1, -7, -4,
-3, -8, -1, 2, 3, 5, 4, 1, 2, -1, -3, -3, -1, 1, -1, 2, 5, 5, 5, 2, -1, -3, -3,
-2, -1, 1, 3, 4, 2, 2, 1, 0, 0, -1, -2, -2, 0, 0, 2, 2, 2, 0, 0, 0, -1, -1, -2,
0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, -1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1,
1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 2, 1, -1, -1, -1, 0, 1, 0,
0, 0, 1, 1, 1, 0, 1, -1, -1, -1, 0, 1, 1, -1, 0, 1, -1, -3, -3, -3, -1, 1, 1, 2,
-4, -3, 0, 1, 3, 0, -1, -1, 1, 3, -1, -5, -5, -2, 5, 2, -2, -8, -3, 1, 4, 10, 6,
3, -7, -9, -2, -6, -3, 2, -1, 7, 7, 2, 0, -1, 0, 3, -1, -8, -7, -6, 10, 6, -1,
-8, -9, 4, -6, 0, 4, 12, 4, -1, 2, -15, -8, -4, 8, 8, -2, -1, -9, -11, 7, 7, 9,
1, -25, -8, 18, 29, 16, -6, -20, -24, -4, -7, 0, 11, 7, 15, 4, 12, -16, -20,
-21, 0, 31, 2, -2, 0, 10, -11, -16, -20, 0, 14, -2, 9, -6, -21, -7, 10, 0, -5,
-7, 0, 15, 8, -29, -24, 0, 5, 21, 9, -11, -7, 12, -9, -15, -12, 16, 9, -5, -18,
-23, 6, 5, 3, 9, 21, -5, -3, 8, 6, -2, -27, -31, 17, 14, 1, 5, 0, -3, 10, 16,
17, 4, -35, -26, -13, 9, 6, 9, 5, 18, 13, -9, 7, -7, 4, 13, -8, -9, -19, -15, 4,
11, 17, 11, -3, -16, -16, -7, 5, 20, 9, 1, 11, 7, -21, -26, -7, 3, 9, 4, 7, -1,
-24, -23, 9, 28, 14, 2, 14, 18, -6, -18, -10, 1, 15, 15, 3, 19, 10, -28, -23,
13, 17, 14, -16, -45, -15, 23, 31, 26, 11, 5, -9, -18, -11, 2, -2, 18, 29, 8, 7,
-23, 7, 23, 9, -16, -30, 2, 22, 1, -17, 7, 22, 15, -2, -14, -11, 18, 37, 15, 4,
15, 9, 4, -16, -21, -9, 0, 6, 4, 24, 24, 2, -7, -34, -26, -10, -9, 5, 36, 55,
45, -2, -41, -13, -7, 2, 8, -7, -15, -22, -18, 8, 55, 78, 48, -3, -27, -23, -10,
-6, -20, -41, -38, 0, 38, 42, 22, 6, 4, 5, 4, 2, 2, 5, 12, 19, 19, 14, 6, -3,
-1, 11, 23, 29, 20, -4, -17, 2, 31, 26, -4, -12, 9, 25, 12, -6, -14, -12, -1,
10, 18, 19, 16, 10, 7, 5, 2, -1, -5, -9, -9, -7, -5, -3, -2, -1, 1, 4, 8, 12,
14, 14, 13, 13, 12, 10, 7, 3, 0, -3, -4, -6, -5, -6, -7, -6, -6, -6, -6, -8, -7,
-8, -6, -4, -3, -1, 0, 4, 6, 5, 2, -4, -9, -14, -16, -16, -16, -15, -11, -5, 0,
6, 7, 4, -1, -8, -12, -14, -16, -18, -20, -22, -15, -3, 15, 29, 35, 27, 7, -16,
-35, -44, -40, -24, -3, 15, 26, 26, 14, -1, -17, -24, -13, 7, 21, 6, -25, -33,
-14, 18, 37, 24, 1, -15, -16, -5, 2, 1, -3, -5, -4, 0, 7, 9, 5, 0, -1, 1, 2, -2,
-8, -3, 8, 5, -7, -16, -16, -1, 21, 48, 59, 45, 13, -26, -39, -30, -17, -8, 11,
9, -7, 0, 20, 11, 11, -5, 3, 13, -16, 1, 11, 15, 22, 11, 11, 26, 13, -7, -23,
-45, -32, 5, 28, 26, -29, -60, -33, 11, 51, 40, -3, -23, -26, -27, -4, -11, -44,
0, 23, 6, 7, -4, 21, 14, -11, -52, -43, -17, -21, -22, -27, 1, 3, 5, -12, 3, -3,
7, 0, -27, -23, -52, -53, -28, 6, -13, 12, 2, 1, -6, -35, -23, -22, -15, -26,
-21, -20, 7, 6, -8, 3, -1, -16, -24, -44, -51, -39, -26, -1, 20, 33, 38, -4,
-39, -42, -36, -19, -20, -7, -11, -14, 21, 20, -11, -25, -17, -24, -30, -24,
-26, -14, -9, 3, 7, 3, 9, 8, 12, 10, -33, -40, -30, 3, 3, 10, 29, 3, 2, -11, 1,
-20, -7, 0, -5, -13, -20, -8, -5, 9, 2, 10, -7, -2, 5, -3, 2, -8, -1, -5, -24,
0, 9, 14, 5, 4, -1, -20, -9, -20, -4, -6, -5, -6, 7, 20, 14, 4, -5, -1, -18, 5,
18, 8, -5, 11, 15, 19, 10, -9, -16, -16, 1, 6, 22, -1, -24, -8, 14, 18, 26, 25,
15, -6, -2, 14, 3, 5, 1, -8, -6, 4, 10, 22, 13, 4, 12, 10, 14, 10, 7, -5, 5, 10,
1, 13, 12, 5, 1, -8, -2, 5, -5, 9, 3, 13, 18, 12, 10, 3, -1, -9, -2, 6, 29, 16,
3, 7, -4, 2, 18, 19, 7, -6, -9, -9, 2, 10, 4, 5, 10, 6, 1, -2, 3, -2, -10, -12,
-1, 22, 29, 20, 3, 2, -3, -1, 8, 15, 14, 14, 12, 9, 11, 2, -1, -6, -1, 5, 9, 12,
12, 7, 8, 11, 10, 7, 6, 4, 1, 4, 5, 7, 9, 11, 10, 10, 10, 12, 6, -1, 1, 0, 4, 3,
9, 11, 11, 13, 10, 8, 7, 4, 2, 3, 2, 5, 8, 8, 9, 7, 7, 5, 6, 5, 3, 3, 3, 2, 4,
5, 5, 6, 5, 6, 6, 4, 5, 4, 4, 5, 5, 7, 7, 7, 7, 7, 7, 6, 5, 5, 4, 4, 3, 4, 4, 4,
5, 5, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 2, 1, 1, 0, 1, 1, 1, 2, 1, 1,
1, 1, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, -1, -1, -2, -3, -2, -1, -2, -2, -1,
-2, -2, -2, -1, -1, -1, -2, -2, -1, 1, 0, 0, 0, 0, -2, -2, -1, 1, -1, 0, 0, -3,
-2, -1, -3, -2, -2, -4, -2, 0, 2, 1, 0, -3, -4, -2, -1, -1, 1, -2, -1, 3, 1, 4,
-7, -6, -6, 1, 8, 9, 4, -4, -6, -10, -4, -6, -2, -2, -6, -2, -4, -1, 0, -1, -11,
-8, -6, -9, 8, 9, 1, -8, -10, 9, 19, 14, 2, -17, -16, -1, 4, -1, 0, -2, -7, -12,
-12, -4, -7, -13, -4, 8, 17, 16, -8, -17, -5, -14, -21, -22, -15, -1, -10, -16,
0, 1, 4, 25, 19, 14, 4, -15, -35, -43, -15, 6, 30, 34, 17, 19, 28, 53, 32, 12,
-23, -56, -34, 7, 61, 39, 32, -12, -38, -19, -20, -12, -40, -19, -28, -33, -8,
8, 21, 13, -12, -36, -55, -46, -15, -15, -20, -38, 15, 25, -4, 17, -8, -20, 1,
18, 10, -4, -12, -34, -56, -27, -27, -31, -1, -24, -8, 9, -4, -30, -17, 13, 2,
-8, -48, -53, -16, 25, 53, 48, 59, 49, 30, 16, -37, -50, -55, -31, -12, 7, 19,
20, 31, 40, 30, -2, -26, -78, -73, -68, -45, -22, 15, 26, 22, 37, 42, 31, 4, -9,
-22, -38, -50, -34, -16, -20, -1, 2, -20, 10, 8, 16, -5, 12, 8, 9, 34, 27, 35,
9, 4, 23, 29, 18, 29, 11, -17, -9, -20, -28, -1, -29, 5, 29, 24, 38, 37, 22,
-29, -35, -1, 38, 29, 8, 14, 41, 41, 38, -1, -10, 24, 16, 12, -4, 5, 25, 39, 30,
35, 55, 33, 9, -6, -20, -39, -53, -67, -61, -41, -30, -3, 14, 33, 36, 18, 8,
-18, -31, -49, -51, -2, 4, 11, 19, 34, 44, 16, 35, 38, 42, 57, 39, 16, -11, -6,
-12, -30, -12, 18, 36, 41, 23, 8, 18, 12, 26, 49, 38, 17, -14, -21, -3, 38, 49,
15, -3, 3, -12, -38, -43, -39, 4, 26, 35, 42, 29, 36, 52, 73, 67, 38, 24, 29,
11, -25, -29, -10, -12, 2, 18, 17, 12, 0, 1, 6, 14, 14, 5, -4, -2, 8, 17, 15, 5,
-6, -8, -3, 5, 17, 17, 3, -24, -47, -55, -45, -30, -20, -16, -15, -15, -14, -13,
-9, -5, 1, 11, 23, 32, 38, 38, 31, 22, 15, 14, 14, 15, 17, 17, 5, -9, -27, -46,
-62, -74, -78, -76, -66, -51, -33, -16, 2, 16, 28, 37, 41, 43, 45, 43, 41, 39,
36, 32, 25, 17, 8, -3, -10, -19, -26, -31, -34, -35, -35, -32, -33, -33, -35,
-40, -44, -51, -55, -58, -60, -58, -54, -50, -45, -29, -11, -20, -35, -35, -32,
-28, -33, -25, -5, 18, 28, 29, 41, 42, 33, 34, 17, -17, -36, -26, 10, 20, 18,
27, 27, 30, 42, 35, 16, 4, -6, -16, -27, -34, -32, -16, 12, 37, 46, 51, 72, 64,
36, 17, 15, 11, 5, 34, 18, 3, -20, 7, 16, 12, 16, -6, 19, 1, -20, -26, -37, -30,
5, 39, 43, 19, 14, -32, -49, -64, -70, -53, -56, -30, -4, 7, 26, 36, 10, 15,
-30, -13, 23, 33, 21, -12, -9, -19, -13, 2, -29, -59, -21, -19, -14, -19, -18,
-14, -43, -36, -24, 14, 5, -40, -22, -20, -21, 9, -6, 11, 19, 44, 27, 12, 25,
-34, -36, -45, -5, 33, 25, 29, 4, 3, 32, 81, 64, 29, 4, -20, -6, -38, -84, -104,
-111, -115, -95, -95, -87, -54, -28, -23, -35, -14, -31, -20, -3, -9, -21, -20,
-4, 9, 4, 4, -8, -76, -99, -79, -11, -17, -75, -40, 2, 33, 45, -5, -8, 26, 56,
41, 36, -8, -42, -16, -9, 16, -13, 3, -14, -27, 0, -26, -5, 12, -39, -77, -96,
-100, -102, -76, -55, -54, -13, 30, 36, 16, 51, 40, -23, -68, -60, -39, 18, 76,
83, 73, 31, 15, -5, -37, -57, -43, -7, 12, 10, -1, -18, -11, -13, -31, -25, -15,
3, 12, 11, 2, -2, 1, 6, 25, 46, 57, 48, 32, 14, -2, -11, -30, -47, -47, -41,
-27, -9, 7, 21, 27, 19, 7, -7, -23, -25, -24, -13, -8, -5, -10, 4, 15, 16, 36,
38, 37, 29, 22, 17, 6, 13, 11, 7, 7, 6, 17, 18, 18, 20, 18, 22, 26, 30, 29, 27,
28, 30, 32, 31, 31, 30, 27, 27, 25, 22, 23, 20, 16, 13, 19, 21, 20, 19, 18, 20,
21, 26, 27, 24, 20, 17, 18, 17, 18, 18, 17, 18, 20, 19, 21, 23, 23, 23, 22, 20,
19, 19, 16, 15, 14, 14, 13, 14, 13, 13, 11, 9, 8, 7, 5, 5, 4, 4, 3, 2, 3, 2, 2,
2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 0, 1, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0,
-1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -4, -4, -4, -5, -6, -6,
-6, -7, -7, -7, -8, -9, -9, -8, -9, -9, -9, -10, -9, -9, -7, -7, -8, -7, -8, -7,
-6, -5, -3, -5, -6, -8, -10, -9, -9, -9, -7, -8, -7, -7, -7, -8, -11, -10, -10,
-9, -10, -14, -11, -11, -10, -7, -9, -7, -8, -8, -9, -9, -9, -13, -9, -4, -5,
-9, -14, -19, -18, -13, -5, -2, -5, -7, -6, -10, -9, -6, -8, -5, -7, -13, -14,
-14, -16, -7, 0, 0, -1, -12, -14, -13, -6, -3, -2, -3, -6, -10, -8, -2, -5, -5,
-2, 4, 1, -9, -11, -19, -15, -5, -6, -6, 0, 1, -3, -5, -5, -8, -7, -2, 1, 8, 1,
-13, -15, -13, -14, -11, -6, -3, -5, -5, -13, -12, -8, -2, 0, -14, -13, 1, 3, 0,
-4, -7, -3, -1, 3, -6, -15, -7, -3, -9, -12, -13, -19, -14, -8, -10, -3, -12,
-5, 9, 5, -8, -21, -19, -15, -16, -13, -15, -10, -2, 4, 12, 6, 5, -2, -8, 1, 0,
-2, -8, -11, -12, -5, -5, -6, 2, -5, -1, -1, -5, 1, 0, -3, -4, -1, -8, -7, -5,
-5, -3, 5, 9, 5, 2, 4, 6, 3, 3, 1, 2, 3, 6, 5, 8, 8, 1, 3, 2, 7, 9, 9, 10, 7, 4,
6, 9, 8, 10, 11, 7, 9, 10, 10, 9, 9, 11, 9, 8, 7, 6, 8, 8, 9, 11, 11, 9, 11, 11,
10, 11, 11, 12, 11, 13, 11, 11, 11, 10, 10, 10, 8, 10, 11, 11, 11, 10, 10, 9,
10, 10, 9, 10, 11, 10, 10, 9, 9, 10, 9, 9, 8, 8, 9, 8, 8, 8, 8, 8, 8, 7, 7, 7,
7, 6, 6, 6, 5, 6, 5, 6, 6, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 3, 3, 3, 2, 2,
3, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, -1, -1, -1, -2, -1, -2, -2, -1, -2, -2, -3,
-3, -2, -3, -3, -3, -4, -3, -4, -4, -4, -4, -4, -5, -4, -5, -5, -5, -4, -5, -5,
-5, -6, -6, -6, -5, -6, -6, -6, -5, -6, -6, -6, -6, -6, -6, -7, -7, -6, -7, -7,
-6, -7, -6, -6, -5, -7, -6, -5, -7, -6, -6, -5, -6, -5, -6, -6, -6, -5, -5, -6,
-5, -4, -5, -6, -5, -4, -4, -6, -4, -6, -4, -4, -3, -4, -4, -3, -3, -3, -4, -4,
-4, -3, -3, -1, -3, -4, -5, -3, -2, -1, -3, -1, -2, -2, -2, -2, -3, -2, -2, -1,
-2, -1, 0, -1, -2, -3, -2, -2, -1, 0, 1, 1, 0, 0, 0, -1, -3, -3, -3, -2, 1, 1,
0, -1, -2, 0, 0, 0, 1, 2, 1, -1, -3, 0, 1, 2, -1, -1, -1, 0, 0, 0, -1, 1, 2, 0,
-1, 0, 1, 0, -1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 0, -1, 0, 0, 1, 2, 0, 1, 2,
0, 0, 0, 1, 1, 0, -2, -2, -1, -1, 2, 2, 2, 1, 2, 0, -1, 0, -1, -2, -1, 0, 1, 0,
1, 1, 2, 2, 3, 0, -2, -2, 0, 1, 1, 0, 0, -1, -2, 1, 1, 2, 0, -1, 0, 0, -1, 0,
-1, 0, -2, -1, 0, 0, -1, 0, 1, 1, 0, -1, -1, 1, 1, 2, 2, 1, 0, -1, -2, -3, -4,
-1, 0, 2, 3, 2, 1, -3, -3, -3, 0, 2, 1, 0, 0, 1, 0, 2, 0, -1, 0, 1, 0, -1, -2,
-1, -1, -2, -1, 0, 1, 2, 2, 2, 2, 1, 0, -1, -1, -1, 0, 1, 1, 2, 1, 0, 0, 0, 1,
1, 2, 2, 2, 1, 0, -1, 0, -1, 1, 2, 2, 1, 0, 0, -2, -1, 0, 1, 2, 2, 2, 2, 1, 0,
0, 0, 0, 1, 0, 0, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
0, -1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, -1, 0, 0,
0, 0, -1, 1, -1, 0, 0, 1, 0, 0, -1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 0, 1, 0, 1, 0,
0, 0, -1, 0, 0, 0, 0, 0, 0, };

#endif /* s11_H_ */
