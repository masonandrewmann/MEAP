#ifndef s5_H_
#define s5_H_
 
#include <Arduino.h>
#include "mozzi_pgmspace.h"
 
#define s5_NUM_CELLS 4047
#define s5_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) s5_DATA [] = {0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,
0, -1, 1, 0, 0, 1, 1, 0, 0, -1, 0, 0, 0, 1, -1, -1, -1, 0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 0, 0, -1, 0, -2, -2, 0, 0, -1, 0, 1, -1, 0, 1, 1, -1, -2, -1, -2, -2, 1,
1, -3, 0, 2, 0, -1, 2, 1, -1, 0, 1, -3, -4, 1, 2, 1, 2, 2, 0, -1, 5, 5, 0, -2,
-1, -3, -3, 2, 3, -2, -1, 3, 2, -1, 2, 3, -2, 0, 2, -3, -8, -2, 3, 1, -1, 2, -2,
-6, 4, 8, 1, -3, -1, -5, -6, 0, 3, -4, -3, 3, 2, -1, 2, 6, -1, 0, 4, -1, -10,
-6, 3, 1, 1, 5, -1, -7, 2, 13, 6, -1, 0, -4, -9, -2, 5, -1, -6, 4, 6, -2, 1, 7,
1, -1, 8, 2, -14, -11, 0, 1, 1, 7, 3, -9, -3, 13, 9, 0, 1, -2, -13, -7, 6, 0,
-8, 2, 8, -1, -1, 8, 3, -2, 7, 8, -10, -16, -4, 1, -1, 8, 7, -9, -8, 10, 13, 2,
2, 0, -12, -12, 3, 1, -11, -2, 8, 0, -4, 5, 4, -5, 6, 12, -7, -19, -10, -2, -3,
6, 12, -6, -13, 4, 15, 6, 6, 6, -8, -14, 2, 5, -8, -4, 11, 5, -2, 7, 9, -1, 7,
21, 5, -16, -11, -2, -5, 5, 17, 0, -17, 0, 15, 8, 7, 11, -6, -19, -5, 3, -11,
-11, 4, 3, -9, -1, 4, -7, -2, 17, 8, -18, -20, -11, -13, -5, 14, 2, -22, -12,
11, 9, 6, 14, 0, -19, -8, 6, -9, -14, 4, 8, -2, 6, 14, 2, 2, 25, 25, -1, -13,
-6, -9, -3, 20, 16, -9, -9, 14, 19, 13, 20, 14, -11, -12, 6, -5, -21, -6, 9, -3,
-2, 11, -1, -13, 13, 26, -4, -24, -17, -23, -24, 2, 12, -16, -23, 1, 7, 0, 12,
12, -14, -20, -2, -9, -27, -13, 8, 0, 0, 14, 7, -9, 12, 36, 16, -10, -6, -11,
-20, 6, 31, 8, -9, 12, 19, 9, 24, 33, 6, -9, 7, 3, -20, -10, 12, 7, 3, 16, 9,
-13, 1, 27, 20, -5, -15, -24, -36, -18, 13, 7, -20, -17, -3, -7, -1, 18, 4, -27,
-18, -6, -28, -36, -7, -1, -12, 4, 12, -19, -20, 24, 31, 1, -6, -12, -36, -23,
25, 26, -8, -6, 13, 9, 15, 42, 30, -4, 4, 23, -1, -19, 5, 16, 6, 20, 31, 0, -14,
30, 50, 21, 5, 1, -31, -31, 15, 24, -14, -18, 4, -4, -3, 24, 17, -18, -11, 4,
-21, -43, -27, -15, -20, -5, 4, -18, -32, -4, 23, 11, -7, -15, -33, -44, -18, 9,
-8, -23, -2, 8, -2, 13, 34, 10, -7, 16, 10, -27, -20, 10, 3, 4, 35, 28, -9, 7,
46, 35, 17, 25, 4, -33, -15, 24, 12, -4, 15, 18, -5, 10, 37, 14, -13, 8, 10,
-29, -36, -10, -19, -23, 17, 21, -29, -36, 5, 5, -6, 13, -7, -63, -52, -1, -3,
-20, -1, -3, -28, -5, 25, 2, -16, 10, 18, -9, -17, -10, -13, 0, 37, 43, 4, -17,
7, 35, 43, 44, 31, -8, -22, 18, 42, 20, 10, 23, 12, 2, 28, 31, -1, 9, 43, 18,
-27, -21, -11, -20, 5, 28, -18, -63, -32, 5, -1, 1, 3, -38, -68, -32, -8, -39,
-48, -24, -34, -54, -26, -6, -31, -22, 23, 6, -44, -34, -12, -32, -17, 28, -1,
-44, -6, 35, 22, 36, 67, 30, -9, 24, 41, 12, 18, 46, 24, 9, 44, 60, 42, 53, 75,
57, 30, 28, 21, 1, 5, 27, 26, 2, -3, 14, 20, 23, 38, 27, -21, -42, -24, -26,
-47, -37, -32, -56, -46, -6, -15, -45, -24, -4, -32, -48, -45, -73, -90, -42,
-5, -34, -53, -28, -16, -10, 24, 33, -20, -46, -9, 1, -25, -13, 14, -1, 7, 57,
49, 3, 29, 79, 54, 25, 32, 0, -33, 28, 82, 36, 4, 32, 31, 23, 67, 72, 5, -12,
22, 4, -28, -10, 0, -14, 1, 21, -10, -36, -9, 20, 13, -9, -32, -60, -66, -33, 0,
-8, -38, -39, -20, -13, -1, 19, 6, -24, -16, -3, -28, -34, 3, 16, 0, 10, 20, 0,
10, 58, 63, 25, 15, 17, -5, -5, 34, 40, 8, 8, 27, 16, 13, 53, 58, 10, -7, 10,
-15, -37, -2, 6, -40, -33, 5, -20, -40, 6, 13, -36, -30, -11, -66, -91, -37,
-25, -59, -41, -25, -60, -44, 21, 16, -32, -29, -20, -40, -30, -6, -25, -38, 0,
31, 22, 17, 29, 41, 48, 56, 46, 14, -4, 19, 51, 54, 43, 45, 48, 54, 81, 93, 58,
27, 42, 49, 26, 15, 14, 2, 18, 57, 50, 3, -8, 19, 27, 12, -3, -32, -66, -55,
-16, -21, -51, -42, -22, -38, -42, -20, -40, -73, -45, -24, -70, -89, -53, -52,
-57, -6, 5, -60, -64, -7, -8, -26, 3, -10, -58, -30, 21, -1, -15, 31, 34, 1, 14,
31, 10, 21, 57, 42, 6, 14, 33, 30, 37, 52, 42, 19, 14, 23, 26, 31, 41, 39, 14,
0, 14, 23, 17, 17, 17, -8, -26, -9, 6, 5, 12, 14, -6, -16, 2, 7, -12, -18, -6,
-9, -26, -27, -14, -3, 16, 36, 19, -23, -18, 19, 17, -8, -9, -19, -39, -9, 32,
9, -7, 31, 34, -2, 9, 19, -28, -39, 4, -7, -48, -27, -6, -21, 7, 49, 6, -45,
-20, -1, -28, -37, -40, -70, -61, -9, 0, -30, -23, 7, 13, 6, -5, -31, -48, -34,
-14, -21, -32, -21, 2, 18, 30, 44, 39, 20, 17, 26, 14, -3, -2, 4, 13, 37, 51,
38, 33, 57, 79, 68, 40, 16, 5, 10, 18, 21, 9, 6, 28, 45, 30, 23, 41, 39, 10, 0,
-9, -38, -33, -2, -13, -35, -7, 5, -24, -11, 24, 2, -29, -18, -34, -68, -42,
-12, -41, -47, -11, -15, -37, -10, 14, -5, -10, -9, -42, -55, -19, -6, -31, -36,
-17, -11, -7, 3, 3, 1, 12, 13, -13, -37, -28, -4, 1, -13, -19, -17, -8, 15, 32,
24, 12, 12, 7, -3, 3, 10, -1, -9, 1, 14, 24, 29, 30, 40, 55, 56, 38, 14, 7, 25,
45, 29, -1, 6, 36, 51, 61, 60, 36, 32, 55, 44, 1, -2, 10, -8, -11, 9, 1, -12,
17, 28, 3, 2, 9, -31, -58, -37, -39, -67, -67, -57, -59, -39, -12, -31, -56,
-38, -21, -42, -68, -78, -80, -68, -47, -45, -53, -38, -6, 3, -5, -5, 2, -1,
-12, -16, -17, -13, 6, 24, 23, 31, 55, 62, 52, 56, 64, 56, 42, 32, 24, 31, 53,
58, 47, 49, 60, 64, 59, 47, 40, 42, 32, 5, -8, 2, 13, 19, 16, -5, -11, 11, 14,
-15, -24, -15, -28, -39, -35, -45, -49, -19, -8, -42, -51, -26, -28, -39, -26,
-26, -46, -41, -27, -34, -28, -2, -7, -34, -30, -7, -5, -13, -12, -11, -5, 9, 9,
-7, -7, 19, 32, 7, -17, -11, 6, 15, 19, 11, 0, 12, 33, 29, 12, 16, 21, 7, -4, 0,
1, 4, 12, 15, 15, 27, 34, 24, 13, 16, 23, 14, -9, -22, -4, 16, 18, 13, 14, 18,
33, 45, 23, -7, 1, 13, -9, -25, -14, -10, -5, 15, 17, 1, 12, 27, 4, -23, -17,
-17, -36, -42, -37, -35, -24, -5, -7, -16, -3, 9, -8, -33, -38, -32, -34, -40,
-44, -39, -22, -1, 7, 1, 1, 17, 15, -9, -21, -11, -7, -11, -12, -12, 0, 25, 42,
33, 26, 35, 40, 25, 8, 4, 10, 14, 9, 5, 11, 25, 40, 45, 33, 26, 32, 25, 1, -4,
6, 2, -9, -10, -6, 1, 22, 27, 5, -4, 13, 13, -10, -23, -18, -18, -16, -16, -27,
-25, 0, 16, 3, -11, -8, -1, -6, -17, -22, -21, -21, -23, -23, -20, -9, 5, 5, -9,
-9, 6, 3, -16, -23, -18, -17, -20, -22, -26, -19, 5, 13, -2, -12, 3, 14, 2, -10,
-14, -14, -12, -7, -7, -9, 5, 26, 24, 12, 20, 36, 27, 12, 12, 12, 8, 15, 17, 9,
20, 47, 48, 30, 33, 49, 42, 26, 19, 11, 5, 13, 14, 2, 2, 22, 31, 17, 7, 14, 13,
-4, -18, -26, -32, -32, -27, -34, -45, -29, -7, -16, -35, -30, -23, -36, -46,
-49, -58, -58, -43, -38, -49, -40, -10, -6, -20, -17, -8, -12, -19, -16, -18,
-20, -4, 8, 1, 4, 32, 49, 34, 28, 38, 40, 31, 33, 30, 20, 31, 50, 43, 30, 48,
66, 54, 38, 42, 38, 27, 27, 23, 8, 7, 23, 18, 1, 6, 21, 14, -6, -10, -9, -19,
-22, -21, -34, -43, -27, -19, -36, -43, -25, -21, -39, -42, -37, -42, -43, -32,
-36, -49, -35, -17, -26, -38, -21, -9, -18, -22, -12, -13, -13, 1, 4, -8, -2,
20, 19, 2, 10, 27, 22, 15, 23, 27, 21, 32, 42, 28, 20, 40, 46, 25, 19, 36, 34,
24, 28, 32, 22, 26, 37, 26, 8, 18, 28, 9, -6, 4, 7, -3, -4, 2, -8, -10, 1, -2,
-25, -26, -10, -21, -41, -36, -26, -34, -34, -25, -29, -36, -24, -16, -36, -44,
-26, -25, -42, -40, -24, -26, -26, -12, -9, -17, -6, 10, -2, -16, 0, 11, -1, -4,
13, 17, 14, 27, 36, 26, 28, 46, 42, 22, 28, 42, 34, 22, 33, 38, 30, 36, 47, 34,
25, 37, 39, 16, 8, 20, 15, -1, 1, 8, -3, -5, 5, -2, -21, -14, -5, -23, -39, -28,
-25, -41, -41, -32, -38, -43, -30, -28, -44, -42, -24, -30, -47, -38, -25, -34,
-35, -21, -19, -25, -11, 2, -8, -12, 9, 14, 0, 3, 20, 17, 11, 23, 29, 21, 28,
44, 38, 26, 40, 51, 36, 29, 42, 41, 28, 32, 39, 27, 23, 37, 34, 15, 18, 29, 18,
2, 9, 9, -6, -9, -2, -11, -24, -11, -8, -26, -29, -15, -20, -37, -34, -27, -38,
-45, -33, -34, -46, -37, -23, -32, -40, -25, -18, -31, -30, -18, -21, -28, -15,
-7, -16, -13, 7, 8, -3, 9, 22, 14, 8, 21, 23, 13, 21, 34, 25, 21, 38, 44, 30,
31, 45, 38, 26, 32, 36, 22, 21, 32, 26, 14, 22, 31, 17, 7, 18, 15, -3, -2, 4,
-9, -17, -6, -8, -23, -20, -9, -17, -31, -23, -20, -34, -37, -27, -32, -43, -33,
-24, -34, -36, -21, -21, -33, -27, -15, -23, -28, -15, -10, -20, -12, 2, -1, -6,
8, 16, 6, 7, 21, 18, 11, 21, 28, 20, 20, 34, 34, 24, 30, 40, 31, 24, 33, 35, 21,
23, 31, 23, 15, 23, 26, 12, 8, 17, 10, -3, 1, 4, -8, -14, -5, -10, -22, -18,
-11, -22, -30, -20, -22, -33, -32, -24, -31, -37, -27, -23, -33, -30, -19, -22,
-30, -19, -12, -20, -19, -8, -6, -13, -4, 6, 1, -1, 13, 16, 7, 14, 23, 20, 15,
26, 30, 21, 24, 34, 30, 23, 30, 35, 23, 22, 30, 26, 16, 20, 24, 14, 9, 16, 13,
1, 2, 7, -3, -10, -5, -5, -17, -18, -12, -18, -26, -21, -18, -29, -30, -23, -25,
-34, -28, -21, -28, -30, -21, -20, -26, -21, -13, -16, -18, -9, -4, -10, -6, 4,
3, 1, 9, 16, 10, 12, 23, 22, 17, 23, 30, 24, 23, 31, 31, 24, 26, 32, 26, 21, 27,
27, 17, 16, 21, 15, 7, 11, 12, 1, -2, 3, -2, -11, -9, -8, -16, -20, -17, -19,
-27, -25, -22, -28, -32, -26, -25, -32, -29, -24, -26, -29, -22, -19, -23, -21,
-13, -13, -16, -9, -2, -6, -4, 6, 7, 4, 11, 17, 15, 15, 22, 24, 21, 25, 31, 29,
25, 30, 32, 28, 28, 31, 28, 23, 25, 25, 19, 17, 19, 15, 8, 8, 9, 1, -4, -2, -4,
-12, -14, -12, -18, -23, -21, -21, -28, -29, -26, -29, -33, -30, -28, -32, -32,
-27, -27, -29, -25, -21, -22, -21, -15, -12, -14, -10, -3, -2, -2, 5, 9, 7, 10,
18, 18, 18, 23, 27, 25, 26, 31, 32, 28, 31, 33, 30, 30, 32, 30, 25, 24, 25, 22,
18, 19, 17, 10, 8, 7, 2, -3, -3, -5, -11, -14, -13, -17, -23, -23, -23, -28,
-30, -28, -30, -32, -33, -31, -32, -34, -30, -28, -29, -27, -23, -22, -21, -17,
-13, -13, -10, -5, -3, 0, 4, 8, 9, 12, 17, 19, 20, 23, 27, 27, 28, 31, 33, 31,
32, 34, 33, 32, 32, 31, 29, 27, 26, 24, 20, 18, 17, 12, 8, 6, 3, -2, -4, -6,
-10, -15, -16, -19, -22, -25, -25, -28, -31, -31, -32, -34, -34, -33, -34, -34,
-32, -30, -31, -29, -26, -23, -22, -18, -15, -13, -9, -5, -3, 1, 4, 8, 11, 13,
17, 21, 22, 25, 28, 31, 30, 33, 35, 35, 35, 36, 35, 33, 33, 32, 30, 27, 26, 24,
20, 17, 15, 11, 7, 5, 2, -3, -7, -10, -14, -17, -19, -22, -26, -28, -29, -32,
-33, -35, -35, -36, -37, -36, -35, -35, -34, -32, -30, -28, -26, -22, -20, -17,
-13, -9, -5, -2, 2, 6, 9, 13, 16, 19, 22, 25, 28, 31, 33, 34, 36, 36, 38, 37,
38, 38, 36, 35, 33, 32, 29, 27, 24, 21, 17, 14, 11, 7, 3, -1, -5, -9, -12, -16,
-19, -23, -26, -28, -31, -33, -36, -37, -38, -39, -39, -39, -39, -38, -36, -34,
-34, -31, -28, -26, -22, -18, -15, -12, -7, -3, 1, 5, 9, 13, 17, 20, 24, 26, 29,
32, 34, 36, 38, 40, 39, 40, 39, 40, 38, 38, 36, 33, 31, 28, 26, 22, 18, 15, 11,
6, 2, -2, -6, -10, -14, -18, -22, -25, -28, -30, -34, -36, -37, -39, -40, -41,
-40, -40, -40, -39, -38, -35, -33, -32, -28, -25, -22, -18, -14, -9, -6, -1, 3,
7, 12, 16, 19, 23, 27, 31, 32, 35, 37, 40, 40, 42, 41, 42, 41, 41, 39, 37, 36,
33, 31, 28, 23, 20, 16, 12, 8, 4, -1, -5, -10, -14, -18, -23, -25, -29, -33,
-35, -38, -39, -41, -41, -42, -43, -43, -42, -40, -38, -37, -34, -32, -28, -26,
-21, -18, -13, -9, -4, 0, 4, 9, 13, 17, 22, 25, 28, 32, 35, 37, 40, 41, 43, 43,
43, 43, 42, 41, 40, 38, 36, 32, 29, 25, 22, 18, 14, 9, 5, 0, -5, -9, -14, -17,
-22, -25, -29, -33, -36, -39, -40, -41, -42, -44, -44, -43, -42, -42, -40, -38,
-35, -32, -29, -26, -22, -17, -13, -9, -4, 0, 5, 10, 14, 18, 23, 26, 30, 33, 35,
38, 40, 41, 43, 44, 44, 44, 43, 42, 40, 37, 35, 31, 28, 24, 20, 16, 12, 7, 2,
-2, -7, -11, -16, -20, -24, -28, -31, -35, -37, -38, -41, -42, -43, -44, -43,
-43, -42, -41, -38, -35, -32, -30, -26, -22, -18, -13, -9, -4, 0, 4, 9, 13, 18,
22, 25, 30, 32, 35, 37, 40, 41, 43, 43, 43, 42, 42, 41, 39, 36, 34, 31, 28, 24,
20, 15, 11, 6, 2, -3, -7, -12, -16, -20, -24, -28, -30, -34, -36, -39, -40, -41,
-43, -42, -42, -41, -40, -38, -36, -34, -31, -27, -23, -20, -15, -12, -8, -2, 2,
6, 10, 15, 18, 23, 26, 29, 32, 35, 37, 38, 39, 40, 41, 40, 39, 39, 37, 35, 33,
29, 27, 23, 19, 15, 11, 7, 3, -2, -6, -10, -14, -18, -21, -25, -28, -31, -33,
-36, -37, -38, -39, -39, -39, -38, -36, -35, -34, -31, -29, -26, -22, -19, -14,
-10, -6, -2, 2, 6, 10, 14, 18, 21, 24, 28, 30, 32, 34, 35, 36, 37, 37, 37, 36,
34, 34, 31, 29, 26, 22, 20, 15, 12, 9, 4, 1, -3, -7, -11, -14, -18, -22, -24,
-26, -29, -31, -32, -33, -34, -35, -34, -35, -33, -32, -30, -28, -25, -22, -20,
-17, -14, -10, -7, -3, 2, 5, 9, 13, 16, 19, 22, 25, 27, 29, 30, 31, 33, 33, 32,
32, 32, 31, 30, 28, 25, 23, 19, 16, 13, 10, 6, 2, -1, -4, -8, -11, -15, -18,
-20, -22, -25, -27, -28, -29, -31, -31, -31, -30, -31, -29, -28, -26, -23, -21,
-18, -15, -13, -10, -6, -2, 1, 4, 8, 11, 14, 17, 19, 22, 23, 25, 28, 28, 29, 29,
29, 29, 28, 28, 26, 24, 22, 20, 17, 14, 11, 9, 5, 2, -2, -4, -8, -11, -14, -16,
-19, -20, -23, -25, -25, -27, -27, -27, -28, -27, -26, -25, -24, -22, -21, -18,
-16, -13, -10, -7, -4, 0, 2, 5, 8, 11, 14, 16, 18, 21, 22, 23, 24, 26, 26, 27,
26, 25, 24, 23, 22, 20, 18, 15, 13, 11, 7, 5, 3, -1, -3, -6, -9, -12, -15, -16,
-18, -20, -22, -23, -24, -24, -25, -25, -24, -23, -22, -22, -19, -18, -16, -14,
-11, -8, -6, -3, -1, 2, 5, 8, 10, 12, 15, 17, 19, 19, 22, 22, 22, 23, 23, 21,
21, 20, 20, 18, 17, 16, 13, 11, 9, 7, 3, 2, -1, -4, -6, -9, -11, -13, -15, -17,
-18, -19, -19, -20, -20, -21, -21, -19, -19, -18, -17, -15, -15, -12, -10, -8,
-6, -4, -1, 1, 4, 6, 8, 10, 12, 13, 15, 16, 17, 18, 19, 19, 19, 19, 18, 18, 16,
15, 13, 12, 11, 9, 7, 5, 3, 1, -1, -4, -6, -8, -9, -12, -14, -15, -15, -16, -17,
-18, -17, -18, -17, -16, -15, -15, -14, -13, -11, -9, -7, -5, -3, -1, 0, 2, 5,
6, 8, 10, 12, 12, 14, 15, 16, 15, 16, 17, 16, 15, 15, 14, 13, 11, 11, 10, 8, 6,
4, 2, 1, -1, -3, -6, -7, -8, -10, -12, -12, -14, -14, -15, -15, -15, -16, -15,
-14, -14, -13, -13, -11, -9, -7, -6, -4, -2, -1, 1, 3, 5, 5, 7, 9, 10, 12, 12,
14, 14, 14, 15, 14, 15, 14, 14, 13, 12, 11, 9, 8, 6, 5, 4, 2, 0, -2, -3, -6, -7,
-8, -10, -11, -13, -13, -13, -14, -14, -14, -14, -14, -14, -13, -12, -11, -10,
-9, -7, -6, -4, -3, 0, 1, 3, 4, 6, 7, 9, 9, 12, 12, 13, 14, 14, 14, 14, 14, 14,
14, 12, 12, 10, 9, 7, 6, 5, 3, 1, 0, -1, -4, -4, -6, -8, -9, -10, -11, -12, -12,
-13, -13, -15, -14, -13, -13, -12, -12, -12, -10, -9, -7, -6, -4, -2, -1, 1, 2,
3, 4, 6, 8, 9, 10, 10, 12, 12, 13, 14, 13, 13, 13, 13, 12, 11, 9, 9, 8, 6, 6, 4,
2, 0, -1, -2, -4, -5, -6, -8, -8, -9, -11, -11, -12, -12, -12, -12, -12, -12,
-12, -11, -10, -9, -8, -8, -6, -4, -2, -2, 0, 2, 3, 4, 6, 6, 8, 8, 9, 10, 10,
12, 11, 11, 11, 11, 11, 10, 10, 9, 8, 8, 6, 5, 3, 2, 0, 0, -2, -3, -4, -6, -7,
-8, -8, -9, -10, -10, -10, -10, -11, -11, -10, -10, -10, -8, -8, -7, -6, -4, -4,
-2, -1, -1, 1, 3, 4, 4, 5, 6, 7, 7, 9, 9, 9, 9, 9, 10, 9, 9, 9, 8, 7, 7, 6, 5,
4, 3, 2, 1, -1, -2, -3, -4, -5, -6, -6, -6, -8, -8, -8, -9, -9, -9, -9, -9, -8,
-7, -7, -6, -6, -4, -4, -3, -2, -1, 0, 2, 2, 3, 4, 4, 6, 6, 6, 7, 7, 7, 7, 7, 8,
8, 7, 7, 6, 6, 5, 4, 3, 3, 3, 2, 0, 0, -1, -2, -3, -5, -4, -6, -5, -6, -6, -7,
-7, -7, -7, -7, -7, -7, -6, -5, -6, -4, -4, -3, -2, -1, 0, 0, 1, 2, 3, 3, 4, 4,
5, 6, 6, 7, 6, 7, 7, 7, 7, 6, 6, 5, 5, 5, 4, 3, 2, 1, 1, 1, -1, -2, -2, -3, -4,
-4, -5, -5, -5, -5, -6, -6, -6, -6, -6, -5, -5, -5, -5, -3, -3, -3, -2, -2, -2,
0, 1, 1, 1, 2, 2, 3, 4, 4, 4, 5, 5, 5, 6, 5, 6, 5, 4, 5, 4, 4, 3, 3, 2, 2, 1, 0,
0, 0, -1, -2, -1, -3, -3, -3, -3, -5, -4, -4, -5, -5, -4, -5, -5, -4, -4, -4,
-4, -2, -3, -3, -1, -1, 0, -1, 1, 1, 2, 2, 2, 3, 2, 3, 4, 4, 3, 4, 4, 4, 4, 3,
4, 3, 3, 2, 2, 2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -2, -3, -3, -3, -3, -4, -3, -3,
-3, -2, -4, -3, -3, -3, -2, -3, -2, -1, -2, -1, 0, 0, 0, 1, 0, 1, 1, 1, 2, 2, 2,
3, 2, 2, 2, 3, 3, 2, 3, 2, 2, 1, 2, 1, 1, 1, 1, 0, 0, -1, 0, 0, -1, -1, -1, -2,
-2, -2, -1, -3, -2, -2, -2, -2, -2, -2, -1, -2, -1, -2, -1, -1, -1, -1, -1, 1,
0, 1, 1, 0, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 1, 2, 1, 0, 1, 0, 0, 0, 0,
-1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -2, -1, -1, -1, -2, 0, -1, -2, -1, 0, 0,
0, 0, -1, -1, 0, 0, -1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 0, 0,
0, 1, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, 0, -1, 0, 0, -1, 0, -1,
0, -1, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 1, 0,
-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, -1,
0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-1, -1, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 1, 1, 0, -1, 0, -1, 0, 0, 0, 0, 0, 1, 0, -1, 0, 0, -1, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 1, 0, 0, -1, 1, -1, 0, 0, 0, 1, -1, 0, -1, 0, 1, 0, 0, 0,
1, 0, 0, 0, 0, 0, 1, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 1,
1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, 0, -1, 1, 0,
0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, -1, 0,
-1, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0, 0, -1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 1, -1, 0, 0, 1, 0, 0, 1,
0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 1, 0, -1, };

#endif /* s5_H_ */