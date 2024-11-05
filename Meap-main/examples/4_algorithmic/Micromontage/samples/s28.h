#ifndef s28_H_
#define s28_H_
 
#include <Arduino.h>
#include "mozzi_pgmspace.h"
 
#define s28_NUM_CELLS 6370
#define s28_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) s28_DATA [] = {0, 7, -16, -12, 44, 6, -55, -6, 54, 5,
-51, 1, 38, -2, -22, -2, 9, 13, -3, -24, 5, 10, 6, 5, -20, -14, 24, 31, -27,
-44, 26, 50, -31, -44, 34, 40, -29, -41, 31, 28, -27, -4, 19, -16, -4, 24, -9,
-23, 22, 21, -32, -28, 32, 42, -37, -41, 28, 36, -7, -40, -2, 34, 12, -35, -10,
38, 4, -40, -13, 52, 12, -48, -5, 29, -12, 0, 33, -39, -37, 63, 24, -74, -9, 73,
-2, -64, 16, 36, -18, -5, -8, 0, 25, -3, -23, -2, 15, 10, -10, -15, 2, 17, 18,
-27, -31, 38, 39, -45, -32, 46, 7, -48, 13, 50, -25, -38, 17, 24, -6, -7, 10,
-31, -9, 62, 1, -63, -5, 59, 10, -55, -7, 47, -6, -40, 14, 38, -9, -40, 9, 30,
-12, -12, 12, -7, -10, 22, 9, -29, -12, 37, 23, -43, -32, 45, 35, -52, -25, 58,
7, -59, 10, 44, -15, -21, 2, -1, 19, 14, -35, -14, 30, 8, -16, 12, -9, -32, 38,
20, -39, -9, 39, -4, -55, 28, 60, -47, -48, 48, 23, -26, -6, -2, 3, 7, 9, -11,
-17, 18, 6, -18, 5, 8, -2, 0, -10, 5, 15, -2, -25, 8, 38, -25, -36, 27, 26, -15,
-11, 10, -14, -16, 48, 6, -63, 4, 55, -3, -52, 11, 53, -25, -55, 50, 35, -59,
-8, 47, -10, -30, 34, 13, -49, -4, 43, 2, -29, -1, 17, 3, -12, -7, 20, 8, -33,
-5, 40, -7, -37, 19, 31, -24, -24, 26, 10, -24, 8, 17, -22, -8, 31, -5, -22, 10,
2, -1, 10, -3, -11, -3, 17, -5, -11, 32, -13, -44, 31, 30, -30, 4, 10, -27, 2,
33, 1, -43, 13, 27, -22, -2, 16, -6, -16, 4, 20, 4, -26, -4, 24, 11, -24, -7,
24, -15, 0, 13, -15, -6, 14, 9, 3, -21, -18, 36, 10, -34, -2, 36, -14, -48, 46,
40, -63, -10, 51, -20, -30, 45, 16, -74, 6, 80, -18, -64, 18, 45, -32, -16, 37,
6, -34, -8, 21, 10, 5, -23, -18, 20, 20, -8, -12, 0, -6, 13, 23, -30, -32, 48,
21, -58, 4, 52, -19, -35, 23, 16, -18, 3, 9, -19, 5, 27, -24, -26, 32, 24, -19,
-34, 3, 43, 0, -33, -2, 19, 0, -16, 5, 15, 1, -23, -10, 34, 17, -45, -15, 41, 5,
-24, 5, 9, -21, 9, 36, -36, -27, 49, 6, -43, -6, 40, 13, -39, -15, 35, 10, -23,
-19, 20, 33, -33, -23, 28, 8, -16, 12, 5, -39, 9, 58, -17, -65, 28, 62, -41,
-49, 60, 24, -76, 1, 73, -11, -47, 6, 12, 4, 10, -2, -24, -5, 27, 7, -13, -17,
10, 21, -12, -30, 20, 39, -36, -34, 33, 43, -32, -47, 31, 38, -25, -17, 11, -12,
17, 26, -36, -33, 48, 32, -53, -23, 50, 13, -50, -4, 48, 4, -38, -14, 19, 20,
11, -27, -36, 29, 42, -21, -39, 13, 28, -12, -3, 1, -14, 22, 3, -40, 12, 54,
-18, -70, 21, 79, -22, -60, 21, 22, -5, 5, -13, -14, 23, 13, -29, 3, 21, -17,
-1, 9, -13, 17, 14, -45, -4, 66, 1, -77, 3, 83, -15, -66, 13, 45, -5, -29, 15,
3, -20, 17, 13, -29, -1, 41, -17, -50, 28, 56, -20, -64, 9, 58, -1, -40, -10,
33, 10, -27, 0, 17, -6, -8, 9, -13, 5, 25, -13, -31, 6, 47, -5, -54, 1, 59, -1,
-59, 4, 57, -8, -45, 16, 8, -7, 20, 3, -38, 5, 50, -28, -34, 43, 14, -42, 1, 17,
7, 14, -32, -33, 42, 50, -45, -56, 47, 36, -38, -2, 15, -13, 0, 10, -4, -9, 18,
4, -30, 0, 30, 12, -27, -27, 23, 35, -18, -38, 12, 43, -12, -50, 20, 51, -29,
-48, 31, 38, -28, -17, 19, 2, -14, 14, 5, -36, 13, 45, -17, -57, 20, 58, -20,
-47, 15, 36, -28, -19, 36, 3, -30, 6, 13, -8, 11, 1, -30, -1, 38, 4, -31, -14,
19, 24, -10, -33, 3, 52, -17, -61, 41, 46, -46, -19, 36, -7, -17, 16, 6, -20,
10, 21, -27, -20, 35, 18, -29, -21, 15, 26, -13, -13, -8, 12, 20, -25, -5, 24,
1, -39, 6, 56, -16, -50, 14, 25, -10, 6, 7, -21, -15, 24, 26, -32, -23, 37, 10,
-37, -3, 39, 8, -47, -9, 48, 6, -37, -6, 19, 2, 1, 4, -7, -11, -1, 19, 10, -35,
-14, 57, 3, -62, 15, 50, -36, -22, 49, -10, -51, 29, 36, -38, -4, 37, -31, -24,
41, 20, -42, -19, 41, 7, -29, 6, 22, -21, -15, 19, 16, -6, -32, 5, 34, -7, -21,
13, 8, -24, -4, 40, -5, -49, 21, 41, -24, -34, 27, 26, -36, -7, 33, -11, -20,
13, 9, -7, -8, 4, 3, 4, -1, -13, 7, 7, -7, 6, 3, -15, -2, 6, 18, 1, -34, 17, 20,
-28, -7, 46, -6, -61, 24, 51, -30, -25, 29, -8, -17, 26, 6, -26, -8, 23, 9, -14,
-11, 4, 16, -2, -29, 13, 33, -19, -30, 20, 28, -33, -15, 44, -7, -41, 28, 14,
-34, 17, 33, -36, -37, 54, 20, -62, 3, 56, -26, -39, 41, 16, -31, -11, 18, 8,
-14, 10, 5, -27, -5, 38, 22, -41, -41, 43, 45, -44, -29, 34, 20, -30, -11, 27,
-1, -16, -4, 15, 8, -20, 2, 13, -22, 8, 33, -35, -28, 38, 24, -32, -21, 32, 6,
-16, 8, -10, -1, 22, -13, -14, 22, 7, -27, -16, 36, 24, -32, -23, 13, 8, 20, 1,
-40, -2, 37, 5, -43, 16, 35, -31, -19, 26, 16, -14, -13, -7, 3, 30, 9, -48, -2,
40, -11, -12, 9, -23, 16, 39, -50, -34, 76, 26, -96, -10, 102, -17, -86, 26, 59,
-17, -31, -1, 13, 4, 3, 13, -35, -25, 60, 37, -75, -40, 71, 34, -63, -17, 49,
-4, -26, -11, 34, 27, -44, -26, 33, 30, -29, -18, 24, -9, -2, 33, -24, -40, 34,
40, -34, -45, 49, 34, -59, -14, 48, 8, -24, -13, -5, 26, 21, -27, -45, 35, 71,
-57, -65, 63, 33, -44, 4, 11, -32, 19, 41, -50, -27, 73, -6, -75, 31, 61, -38,
-38, 30, 7, 2, 8, -31, -11, 42, 6, -34, 4, 10, 0, -5, 0, 10, 2, -7, -16, 7, 37,
-6, -59, 12, 72, -24, -64, 29, 38, -22, -11, 26, -17, -45, 60, 41, -93, -18, 96,
4, -95, 19, 90, -51, -75, 69, 61, -67, -52, 52, 39, -28, -12, -6, -1, 14, 5, -8,
-8, 1, 5, 6, -5, -5, 3, 15, -33, -19, 80, 2, -104, 19, 94, -35, -48, 29, -4,
-13, 44, -13, -60, 46, 42, -58, -15, 56, -10, -39, 19, 20, -6, -5, -4, -23, 17,
47, -22, -60, 20, 58, -21, -37, 14, 15, -6, -1, -5, 2, 12, -11, -22, 22, 41,
-36, -47, 28, 47, -7, -43, -4, 21, 23, -17, -27, 24, 9, -17, -1, 14, 3, -22, 7,
30, -28, -19, 38, 3, -47, 7, 72, -32, -83, 63, 72, -89, -43, 103, -2, -96, 42,
70, -70, -27, 67, -11, -34, 22, -1, -24, 15, 31, -14, -29, 1, 17, 27, -15, -47,
26, 41, -30, -26, 35, 10, -37, 5, 37, -14, -34, 32, 0, -31, 38, 17, -54, -10,
65, -3, -62, 16, 49, -25, -26, 25, 0, -13, 11, -6, -3, 26, -19, -28, 25, 25,
-14, -21, 7, 4, 3, 16, -10, -30, 11, 38, -3, -44, -4, 47, 5, -47, 3, 59, -39,
-46, 61, 13, -46, 6, 16, -5, 7, -4, -4, 1, -3, 7, 16, -15, -43, 37, 57, -69,
-32, 87, -9, -95, 48, 92, -81, -71, 96, 33, -80, -1, 54, -22, -41, 51, 25, -73,
-5, 64, 0, -35, -2, 5, 1, 6, 9, -4, -21, 1, 12, 23, -7, -49, 25, 38, -41, -2,
40, -24, -36, 32, 41, -41, -22, 38, -13, -24, 34, 23, -53, -17, 49, 19, -42,
-20, 40, 9, -35, 9, 30, -37, -13, 49, -3, -33, 10, 3, -5, 25, -11, -28, 22, 21,
-30, -4, 40, -33, -31, 66, 7, -77, 28, 45, -46, -11, 43, 9, -59, -7, 73, 0, -61,
1, 31, 11, -19, -11, 20, -6, -18, 21, 32, -43, -42, 53, 36, -58, -17, 64, -18,
-64, 53, 62, -88, -45, 99, 14, -79, 4, 48, -8, -27, 21, 19, -42, -3, 40, -11,
-18, 12, 16, -23, -28, 51, 27, -67, -19, 57, 14, -32, -16, 12, 18, -5, -9, 7,
-5, -20, 20, 24, -35, -1, 39, -33, -33, 59, 35, -79, -38, 93, 17, -77, 6, 41,
-15, -15, 29, -12, -30, 25, 24, -39, -6, 53, -30, -38, 42, 10, -20, 4, -18, 4,
44, -11, -68, 7, 100, -22, -99, 38, 66, -44, -23, 44, -14, -35, 26, 21, -20, -4,
0, -4, 14, -2, -2, 2, -25, -2, 49, 8, -61, -15, 72, 5, -69, 11, 50, -8, -50, 9,
56, -21, -47, 33, 15, -29, 26, 6, -55, 10, 84, -12, -104, 24, 99, -37, -71, 41,
42, -42, -24, 46, 9, -38, 1, 18, -2, -1, 9, -21, -21, 37, 34, -29, -47, 2, 65,
10, -79, 0, 72, -16, -67, 39, 55, -58, -35, 61, 4, -47, 37, 2, -62, 51, 58, -69,
-42, 52, 33, -20, -30, -8, 32, 27, -35, -33, 40, 28, -50, -9, 53, -7, -47, 7,
30, 11, -15, -20, -1, 13, 16, 4, -19, -32, 33, 29, -36, -11, 22, 8, -21, -6, 25,
15, -31, -32, 24, 50, -11, -59, 8, 46, -7, -18, 8, -14, -2, 30, 1, -43, 8, 53,
-36, -35, 58, 25, -82, -17, 103, -5, -93, 29, 58, -44, -9, 40, -29, -36, 61, 22,
-82, 9, 70, -20, -44, 14, 26, 13, -32, -40, 58, 41, -67, -37, 66, 27, -55, 0,
27, -20, 2, 32, -26, -34, 37, 32, -30, -34, 29, 34, -44, -24, 60, 7, -53, -3,
40, 6, -18, -17, -2, 40, 8, -47, -14, 38, 18, -29, -8, 17, -4, -10, 15, 2, -13,
10, 1, -28, 7, 52, -15, -68, 15, 70, -12, -49, 2, 9, 17, 7, -15, -16, 8, 13,
-19, 18, 18, -42, -12, 41, -4, -13, 17, -30, -21, 66, 10, -91, 21, 78, -55, -37,
75, -5, -71, 31, 50, -43, -25, 59, -17, -59, 44, 55, -50, -59, 56, 50, -48, -38,
36, 17, -14, -7, 6, 7, -16, -6, 24, 11, -37, -4, 30, -2, -13, 7, 5, -31, 9, 55,
-20, -75, 25, 78, -34, -55, 39, 29, -41, -8, 35, -3, -27, 17, 8, -31, 21, 34,
-47, -27, 63, 6, -51, 12, 18, -17, 14, 22, -51, -13, 71, -16, -59, 47, 13, -43,
24, 15, -28, 2, 22, -17, 0, 28, -25, -30, 36, 25, -36, -11, 18, 0, -5, 8, 3,
-17, 15, 2, -26, 26, 13, -40, -3, 45, -5, -47, 29, 27, -61, 12, 73, -41, -68,
55, 50, -59, -22, 57, -7, -54, 31, 43, -46, -29, 42, 32, -43, -31, 46, 14, -36,
-4, 32, -5, -37, 15, 36, -11, -27, -9, 29, 21, -40, -5, 40, -27, -31, 66, 8,
-82, 13, 70, -27, -45, 51, 2, -59, 39, 37, -51, -5, 37, -24, -12, 35, -6, -39,
14, 42, -23, -30, 23, 9, -13, 8, 3, -15, 7, 15, -27, 3, 40, -34, -29, 44, 13,
-40, -4, 35, -14, -27, 45, -4, -49, 25, 28, -9, -11, -15, 7, 27, -16, -25, 35,
18, -62, -4, 88, -16, -96, 30, 74, -24, -42, 17, 17, -23, 7, 33, -16, -51, 20,
64, -39, -52, 62, 17, -67, 22, 52, -47, -35, 44, 21, -25, -14, 1, 9, 19, -8,
-27, 15, 5, -18, 31, 1, -44, 9, 41, -13, -33, 29, 17, -55, 5, 65, -14, -60, 18,
37, -23, 7, 19, -46, -23, 81, 7, -92, 32, 62, -57, -25, 70, -12, -66, 36, 36,
-35, 4, 17, -33, 2, 36, -5, -29, 10, 9, -20, 27, 9, -43, 10, 30, -16, -7, 9, -2,
-11, -3, 37, -2, -56, 1, 60, 7, -52, -12, 38, 10, -17, -13, 5, 20, -19, -14, 34,
11, -48, -12, 61, -8, -54, 42, 23, -77, 17, 94, -46, -90, 59, 80, -70, -62, 83,
25, -76, 11, 48, -22, -17, 14, 2, -8, 14, -4, -29, 18, 31, -17, -24, 1, 12, 27,
-3, -63, 16, 71, -36, -47, 49, 14, -55, 28, 38, -56, 4, 49, -44, -26, 75, 10,
-90, 6, 78, -8, -55, 2, 34, -8, -19, 22, 12, -35, -5, 34, -11, -1, 8, -22, -10,
27, 24, -28, -24, 10, 15, 21, -9, -48, 18, 56, -19, -57, 16, 59, -29, -45, 33,
34, -30, -28, 23, 20, -5, -13, -19, 12, 39, -22, -37, 35, 13, -42, 25, 33, -55,
-10, 62, -20, -54, 70, 19, -116, 28, 126, -57, -103, 64, 60, -49, -9, 22, -32,
7, 50, -26, -46, 28, 33, -21, -23, 17, 24, -23, -30, 30, 31, -24, -31, 16, 24,
0, -17, -19, 16, 16, -2, -4, -26, 0, 48, 0, -59, 8, 66, -32, -64, 55, 60, -67,
-50, 62, 31, -28, -20, -7, 13, 12, 15, -12, -41, 20, 35, -17, -16, 14, -2, -18,
10, 12, -8, 8, -1, -43, 19, 72, -43, -81, 61, 60, -62, -24, 36, -6, -2, 18, -28,
-5, 35, -26, -12, 40, -10, -42, 26, 27, -32, 13, 11, -53, 19, 87, -48, -95, 58,
72, -48, -42, 38, 16, -34, 5, 36, -26, -26, 33, 8, -36, 24, 30, -57, -14, 66,
11, -65, -4, 44, -6, -11, 5, -7, 1, 6, -6, 2, 1, 2, -4, -10, 11, 17, -3, -39, 3,
52, 6, -56, -33, 63, 40, -55, -38, 45, 29, -45, -2, 44, -36, -34, 66, 4, -61,
35, 27, -62, 10, 74, -32, -69, 37, 39, -32, 13, 5, -42, 20, 42, -36, -26, 52,
-12, -47, 31, 39, -32, -38, 31, 28, -17, -17, 3, -4, 11, 19, -18, -29, 10, 26,
6, -18, -29, 18, 31, -9, -29, 9, 23, -38, 4, 56, -21, -55, 20, 43, -20, -10, 12,
-14, -16, 27, 29, -36, -27, 38, 5, -40, 30, 34, -52, -39, 65, 40, -60, -20, 37,
-14, -7, 48, -31, -54, 48, 38, -48, -8, 33, -12, -9, 7, 2, 7, -10, -27, 26, 45,
-37, -54, 48, 43, -62, 2, 53, -47, -32, 73, 20, -87, -2, 82, -25, -61, 59, 23,
-83, 20, 79, -41, -43, 32, -3, -10, 35, -9, -57, 31, 47, -42, -21, 37, -3, -32,
28, 19, -45, 0, 37, -7, -15, 0, 10, -1, -8, 9, 4, -13, 1, 14, -2, -11, 2, 0, 3,
12, -5, -14, 10, 7, -25, 25, 25, -54, -6, 49, -16, -18, 43, -34, -56, 78, 43,
-91, -26, 90, -13, -70, 61, 35, -88, -13, 92, -6, -69, 22, 33, -35, -4, 49, -12,
-60, 28, 55, -40, -23, 22, 9, 1, -22, 2, 37, -23, -49, 40, 58, -52, -60, 67, 41,
-75, 3, 61, -55, -27, 85, -14, -74, 29, 49, -25, -19, 19, -12, -4, 12, 5, -1,
-8, -6, -5, 20, 17, -26, -26, 19, 32, -11, -38, 20, 27, -40, 5, 45, -35, -26,
36, -7, -10, 30, -11, -45, 21, 50, -30, -37, 27, 11, -11, 11, -4, -23, 15, 22,
-19, 1, 7, -20, 5, 26, 1, -33, -4, 22, -5, 14, 11, -55, -17, 95, 7, -114, 27,
91, -58, -47, 77, 13, -87, 23, 74, -57, -32, 76, -15, -76, 37, 66, -26, -60, 5,
67, 6, -72, -7, 65, 8, -59, 2, 56, -21, -40, 30, 24, -29, -10, 17, -1, -5, 7, 4,
-23, -10, 47, 17, -77, -2, 76, -23, -40, 35, -4, -29, 28, 13, -27, -6, 31, -23,
-28, 63, 3, -70, 3, 58, 8, -40, -10, 14, 0, 22, 10, -48, 3, 42, -18, -25, 37, 1,
-49, 26, 33, -28, -8, 14, -20, 8, 45, -31, -56, 45, 49, -45, -27, 26, 4, 5, 3,
-38, 18, 51, -49, -44, 70, 26, -79, -7, 69, -12, -40, 29, 7, -50, 23, 63, -33,
-70, 33, 78, -51, -61, 75, 32, -92, -9, 95, -8, -80, 8, 54, 8, -34, -16, 16, 10,
4, -4, -19, -3, 23, 13, -21, -29, 22, 48, -33, -58, 54, 43, -59, -29, 57, 28,
-66, -8, 51, -30, 1, 51, -56, -52, 88, 47, -100, -23, 89, -14, -58, 42, 25, -56,
3, 41, -19, 7, 17, -56, 0, 73, -20, -45, 20, 4, -6, 20, -3, -32, 10, 36, -20,
-32, 35, 21, -42, -19, 44, 28, -44, -35, 35, 39, -19, -34, -6, 34, 25, -45, -13,
47, -20, -32, 47, 9, -54, 0, 50, -16, -36, 42, 14, -69, 3, 98, -19, -106, 29,
89, -26, -73, 39, 47, -58, -8, 51, -14, -39, 28, 7, -32, 36, 13, -52, 5, 35, -2,
-3, -20, -19, 34, 37, -50, -36, 64, 5, -50, 27, 18, -47, 20, 43, -52, -9, 62,
-23, -62, 52, 52, -65, -41, 60, 39, -57, -21, 46, -5, -21, 20, -1, -21, 6, 19,
-15, -4, 28, -24, -22, 27, 11, -11, 4, -16, -18, 47, 16, -55, -11, 43, 9, -30,
-4, 15, 4, -10, -4, 14, 6, -27, -4, 33, 2, -26, -5, 18, 3, 1, -12, -12, 14, 3,
7, 8, -38, -12, 66, 1, -77, 28, 68, -67, -33, 85, -4, -76, 30, 44, -46, 10, 30,
-48, -5, 55, -6, -40, 5, 13, 9, -1, -7, -7, -5, 12, 4, 4, -3, -17, 9, 12, -10,
5, 1, -13, -8, 22, 37, -46, -51, 60, 57, -75, -36, 86, -15, -67, 57, 35, -69,
-15, 65, -5, -49, 25, 23, -42, -5, 54, -1, -52, -4, 33, 16, -17, -15, 10, -8,
-1, 22, -3, -18, -3, 0, 13, 23, -30, -26, 35, 18, -37, 1, 38, -33, -22, 47, 6,
-40, 10, 11, -19, 25, 6, -44, 15, 39, -37, -14, 47, -17, -45, 39, 38, -54, -20,
57, -5, -46, 40, 25, -70, 5, 69, -18, -55, 18, 33, -16, 5, 14, -49, 4, 67, -22,
-52, 20, 36, -27, -12, 31, 0, -26, -12, 24, 21, -23, -20, 25, 10, -27, 6, 32,
-26, -39, 50, 28, -54, -15, 44, 8, -33, -3, 34, -8, -58, 39, 63, -67, -51, 71,
35, -65, -4, 42, -33, -10, 59, -22, -54, 38, 24, -32, 6, 16, -14, -10, 11, -1,
3, 23, -31, -32, 44, 32, -41, -31, 33, 17, -28, 13, 10, -34, 5, 32, -18, -8, 34,
-38, -27, 68, 11, -67, 5, 43, -26, 4, 33, -56, -20, 94, -10, -95, 31, 68, -40,
-33, 48, -9, -32, 32, 13, -34, 3, 30, -25, -26, 45, 33, -77, -33, 91, 28, -85,
-21, 74, -13, -48, 50, 7, -53, 19, 28, -20, -12, 22, -2, -24, 9, 18, 12, -18,
-50, 32, 81, -56, -84, 70, 63, -64, -36, 55, 12, -52, 16, 44, -42, -22, 49, -7,
-36, 36, 26, -68, -9, 88, -15, -64, 17, 26, -11, 11, 13, -48, 4, 57, -31, -29,
42, -5, -40, 29, 23, -26, -2, -9, -7, 44, 12, -62, -15, 64, 6, -41, 4, 5, -4,
20, -5, -33, 19, 28, -26, -19, 27, 20, -29, -27, 28, 40, -25, -42, 21, 25, -4,
5, -31, -19, 62, 9, -69, 8, 68, -33, -62, 74, 31, -82, -4, 57, 0, -27, 12, 0,
-35, 16, 60, -22, -74, 25, 65, -22, -41, 8, 29, 0, -29, -5, 35, 16, -61, -9, 83,
-14, -72, 33, 38, -46, 9, 50, -60, -32, 99, -8, -99, 39, 76, -51, -49, 62, 15,
-57, 9, 37, -12, -6, 5, -27, 4, 43, 2, -53, -14, 61, 16, -54, -11, 33, 8, -16,
0, 2, -3, 6, -9, -1, 19, -2, -25, -2, 24, 1, 1, -14, -24, 30, 29, -25, -37, 18,
46, -20, -34, 16, 20, -14, -18, 21, 11, -16, -7, -15, 25, 49, -50, -66, 62, 70,
-66, -47, 57, 5, -34, 44, 10, -83, 25, 76, -52, -36, 57, 8, -60, 8, 60, -9, -39,
-21, 24, 49, -13, -59, 14, 44, -16, -13, 22, -19, -28, 44, 16, -48, 7, 41, -30,
-40, 49, 48, -77, -44, 95, 18, -80, 5, 46, -17, -8, 27, -29, -27, 56, 13, -61,
10, 50, -21, -43, 34, 34, -39, -18, 23, 23, -13, -17, 6, -8, 11, 26, -17, -33,
13, 17, 2, 10, -21, -28, 38, 21, -44, 9, 38, -45, -30, 77, 17, -87, 2, 58, -12,
-6, 9, -44, 9, 69, -32, -61, 49, 31, -61, 11, 60, -45, -40, 58, 14, -50, 11, 34,
-35, -27, 64, 21, -78, -11, 73, -8, -47, 29, 10, -34, 5, 33, -2, -17, -13, -3,
35, 1, -29, 0, 7, 2, 9, 16, -31, -29, 49, 17, -45, 5, 22, -16, -6, 19, -1, -18,
11, 8, -17, -3, 26, -1, -40, 12, 50, -21, -54, 36, 41, -51, -13, 62, -31, -51,
69, 19, -75, 7, 55, -17, -20, 9, -10, -5, 27, 3, -22, 7, -12, -7, 40, 6, -49,
-2, 45, -16, -22, 28, -4, -30, 15, 36, -19, -38, 19, 29, -20, -5, 14, -17, 1,
30, -25, -17, 47, -13, -52, 36, 51, -59, -37, 71, 5, -53, 27, 21, -45, -1, 55,
-8, -50, 8, 36, 3, -34, 9, 30, -37, -9, 43, -3, -34, 5, 20, -5, 7, 2, -32, -3,
43, -3, -34, 16, 4, -17, 22, 4, -22, 15, -2, -30, 38, 25, -61, 0, 53, -16, -33,
32, 4, -52, 36, 54, -67, -33, 69, 16, -57, 0, 47, -24, -38, 47, 31, -57, -25,
48, 18, -21, -4, -18, 0, 33, -1, -15, -2, -10, 1, 32, -8, -38, 21, 27, -32, -17,
47, 1, -49, 6, 27, 10, -8, -25, -15, 34, 34, -35, -40, 34, 30, -30, -13, 23, 10,
-34, 3, 41, -23, -32, 34, 12, -40, 21, 39, -56, -31, 84, 8, -88, 19, 63, -28,
-27, 22, 1, -9, 19, -3, -37, 22, 37, -42, -22, 43, 18, -36, -16, 19, 16, 4, -24,
-16, 16, 32, -19, -36, 42, 0, -38, 43, 6, -58, 25, 48, -47, -21, 68, -15, -81,
46, 84, -65, -79, 72, 52, -60, -15, 25, 2, -5, 1, 7, -6, -11, 5, 16, -10, 1, 4,
-21, 2, 17, 14, -8, -32, -5, 35, 31, -32, -59, 38, 63, -44, -35, 34, 6, -15, 3,
-3, 17, 0, -44, 8, 58, -7, -57, 8, 36, -6, -17, 8, -3, -18, 34, 17, -59, 5, 63,
-27, -56, 33, 61, -51, -54, 68, 15, -44, 21, 2, -44, 51, 42, -91, -17, 98, -4,
-74, 13, 41, -18, -8, 18, -21, 5, 17, -27, 6, 34, -23, -29, 27, 3, -5, 22, -30,
-35, 52, 44, -58, -51, 57, 55, -65, -45, 81, 21, -82, -1, 76, -14, -63, 27, 34,
-42, 11, 40, -58, -16, 79, -1, -66, -1, 41, 7, -14, -13, -6, 16, 13, -23, -10,
35, -3, -43, 12, 50, -9, -58, 8, 48, -2, -23, -7, -3, 8, 25, -10, -35, 31, 14,
-49, 24, 35, -34, -9, 20, -18, 5, 46, -32, -66, 58, 60, -69, -25, 48, -12, -6,
29, -31, -22, 51, -2, -53, 24, 48, -35, -39, 41, 30, -41, -23, 36, 14, -22, -5,
12, -8, -9, 34, 7, -56, 1, 58, -10, -38, 4, 23, 14, -32, -22, 64, 3, -88, 23,
84, -31, -71, 34, 49, -44, -13, 54, -24, -54, 49, 42, -61, -24, 61, 8, -62, 12,
64, -45, -45, 63, 10, -42, 14, 6, -20, 25, 17, -49, 3, 49, -24, -31, 32, 11,
-34, 6, 28, -20, -4, 16, -28, 9, 47, -34, -41, 38, 15, -20, 8, 10, -28, -14, 50,
7, -54, 8, 33, -15, -11, 21, -6, -27, 16, 20, -12, -12, 2, -5, 7, 28, -26, -36,
50, 32, -68, -12, 63, -9, -45, 29, 29, -40, -23, 46, 20, -52, 6, 38, -31, -19,
41, 26, -54, -28, 57, 21, -42, -13, 18, 5, 0, -3, 8, 4, -38, 4, 67, -23, -66,
34, 43, -45, -11, 64, -32, -63, 60, 41, -71, -4, 56, -34, -21, 48, -3, -45, 16,
27, -20, 0, 15, -15, -24, 26, 29, -30, -24, 24, 9, -6, 4, -14, -7, 13, 15, -10,
-5, -3, -30, 39, 44, -60, -34, 56, 10, -38, 16, 16, -34, 1, 37, -11, -24, 20, 3,
-26, 18, 23, -21, -19, 11, 16, -5, -4, 3, -17, 1, 34, -9, -50, 30, 49, -59, -23,
75, -20, -70, 58, 46, -75, -17, 70, -12, -43, 34, 16, -50, 5, 52, -19, -28, 2,
10, 31, -14, -44, 21, 35, -28, -16, 33, 5, -44, 0, 57, -11, -49, 19, 22, -25, 8,
30, -31, -32, 50, 19, -53, 13, 33, -37, -17, 67, -6, -72, 31, 38, -27, -3, 11,
-14, -6, 20, 12, -23, -14, 20, 19, -11, -32, 13, 32, -15, -24, 19, 15, -22, -9,
8, 19, -1, -24, -3, 17, 4, -6, 10, -21, -18, 48, 8, -50, 6, 36, -20, -18, 37,
-4, -38, 18, 8, -4, 29, -22, -56, 52, 62, -67, -44, 56, 25, -36, -6, 26, -20,
-14, 42, 2, -47, 9, 40, -27, -22, 43, 7, -51, -1, 50, 0, -32, -9, -1, 37, 14,
-55, -4, 43, -7, -21, 23, -3, -34, 27, 16, -28, 13, 20, -36, -20, 55, 22, -66,
-20, 58, 6, -34, 5, 12, -19, -4, 41, -9, -56, 35, 45, -62, -15, 74, -6, -77, 16,
76, -28, -59, 33, 29, -22, 0, -3, -7, 14, 3, -11, 6, 0, -15, 14, 11, -13, -6,
10, -2, -15, 22, 21, -39, -32, 60, 28, -59, -17, 28, 14, 8, -9, -36, 7, 50, -14,
-53, 36, 36, -54, -6, 51, -16, -30, 27, -5, -21, 38, 10, -59, -5, 71, 1, -64, 2,
44, -5, -29, 8, 22, -13, -18, 16, 15, -11, -15, 4, 4, 5, 18, -23, -32, 26, 36,
-9, -34, -17, 38, 25, -45, -10, 39, -9, -33, 28, 28, -49, -2, 44, -32, -15, 48,
-4, -66, 13, 83, -22, -83, 34, 64, -39, -46, 47, 30, -55, -7, 46, -13, -22, 18,
-7, 0, 23, -26, -13, 30, -1, -18, 20, -8, -41, 42, 44, -68, -25, 63, 1, -39, 14,
14, -27, 9, 23, -14, -8, 6, -10, -3, 31, -2, -35, -2, 26, 8, -16, 0, 1, -17, 14,
29, -31, -28, 43, 9, -49, 23, 51, -55, -44, 63, 31, -51, -12, 19, 1, 11, 3, -21,
-14, 31, 16, -36, -6, 28, 2, -22, 0, 31, -12, -34, 23, 19, -10, -9, -2, 2, 0, 5,
8, 0, -31, -7, 57, 9, -76, 4, 83, -44, -59, 80, 19, -99, 11, 101, -40, -72, 54,
24, -43, 9, 25, -24, -15, 27, 6, -17, -5, 3, 11, 5, -19, 5, 15, -18, -6, 23, 5,
-24, -10, 21, 22, -28, -17, 26, 6, -16, 6, 10, -25, -7, 37, -4, -26, 11, -3,
-11, 21, 14, -30, -11, 22, -6, 7, 16, -25, -25, 36, 29, -37, -20, 24, 13, -23,
3, 32, -28, -30, 44, 5, -37, 28, 10, -47, 15, 57, -38, -47, 55, 20, -51, 6, 35,
-30, -11, 38, -3, -25, 6, 5, 4, 10, -17, -7, 23, -14, -18, 43, 7, -63, 5, 76,
-19, -74, 36, 49, -44, -21, 50, 1, -59, 16, 61, -27, -53, 38, 30, -45, -2, 43,
-23, -33, 33, 19, -28, -6, 14, -6, 4, 7, -5, -5, -9, 13, 15, -10, -13, -1, 14,
9, -11, -13, 6, 14, -4, 0, -2, -14, 10, 18, -14, -12, 24, -14, -31, 45, 27, -59,
-16, 54, -4, -35, 26, 4, -37, 24, 32, -36, -8, 34, -23, -17, 38, 5, -40, 6, 28,
-21, -2, 31, -22, -34, 36, 28, -41, -20, 46, 5, -44, 13, 33, -21, -28, 29, 17,
-23, -9, 1, 18, 9, -25, -8, 28, -12, -21, 35, 5, -53, 10, 64, -19, -62, 28, 46,
-43, -15, 48, -3, -49, 0, 61, -5, -49, 13, 20, -20, 13, 25, -44, -20, 57, 9,
-52, 11, 28, -28, -2, 34, -14, -30, 28, 5, -25, 22, 8, -30, 9, 22, -14, -4, -4,
-6, 21, 9, -35, -11, 44, 2, -41, 8, 38, -18, -33, 24, 19, -20, 0, 8, -18, 0, 40,
-6, -62, 21, 58, -28, -30, 19, -2, -4, 27, -22, -25, 45, -2, -49, 25, 46, -38,
-35, 44, 21, -37, -11, 25, 6, -12, -6, 8, 6, -9, 1, 16, -14, -22, 33, 9, -34,
-1, 27, 5, -27, -9, 35, 12, -53, -11, 69, 8, -74, 4, 59, -24, -19, 37, -22, -37,
50, 22, -58, -1, 55, -26, -41, 46, 19, -47, -9, 36, 0, -14, 9, -10, -14, 26, 18,
-35, -14, 38, 3, -35, 10, 27, -15, -18, 9, 11, 6, -7, -38, 22, 56, -36, -48, 31,
29, -26, 5, 17, -39, 1, 54, -18, -45, 26, 22, -24, -5, 18, 5, -15, -18, 13, 29,
-6, -33, -9, 29, 22, -19, -29, 19, 31, -31, -20, 40, 8, -45, 4, 44, -16, -29,
26, 2, -28, 28, 17, -40, -7, 27, 15, -12, -28, 4, 35, -3, -35, 1, 38, -12, -30,
28, 10, -27, -5, 22, 0, -15, 14, -10, -16, 26, 25, -39, -31, 56, 11, -58, 20,
43, -46, -16, 53, -8, -41, 26, 14, -36, 13, 40, -26, -45, 29, 41, -17, -34, -1,
33, 10, -36, -7, 35, -3, -23, 7, 19, -6, -27, 7, 35, -11, -29, 17, 11, -18, 11,
15, -26, -18, 38, 18, -40, -15, 36, 5, -25, 7, 5, -5, 12, -16, -11, 39, 3, -52,
0, 53, 0, -47, 4, 42, -24, -17, 42, -18, -40, 34, 33, -37, -30, 43, 12, -46, 18,
29, -29, -14, 18, 7, -2, -6, -13, 11, 22, -17, -16, 11, 3, 3, 5, -13, -12, 23,
14, -33, -1, 38, -22, -35, 37, 21, -36, -9, 21, 7, -15, 5, 14, -29, -10, 48, 3,
-52, 4, 46, -11, -36, 27, 24, -42, -12, 43, 4, -27, -7, 13, 20, -13, -21, 17,
13, -20, 1, 21, -14, -14, 18, 8, -19, -3, 23, -3, -30, 12, 33, -19, -30, 15, 29,
-15, -19, 8, 1, 11, 12, -27, -23, 48, 3, -46, 32, 19, -50, 11, 47, -27, -32, 33,
10, -35, 21, 30, -43, -20, 43, 13, -30, -5, 10, -1, 3, 3, -2, -4, -4, 9, 2, -11,
11, -1, -16, 17, 9, -16, -6, 5, 12, 2, -10, -5, 0, 13, 0, -11, 15, -10, -26, 35,
26, -57, -10, 54, -8, -35, 15, 27, -31, -20, 43, 9, -41, 0, 34, -13, -23, 33,
10, -40, -2, 38, -2, -29, 10, 12, -16, 11, 9, -17, 0, 2, -4, 20, 1, -40, 11, 33,
-20, -13, 19, -5, -20, 21, 20, -35, -9, 33, -8, -11, 11, -16, 0, 29, -7, -28,
12, 20, -17, -11, 23, 1, -29, 8, 24, -4, -21, 6, 7, -10, 18, 6, -30, -9, 33, 5,
-26, 12, 7, -29, 15, 26, -25, -12, 13, 4, -1, 0, -1, -3, -4, 8, 12, -13, -16, 9,
19, 0, -23, 0, 18, 0, -14, -1, 21, -8, -31, 27, 33, -45, -19, 48, -9, -41, 43,
18, -64, 3, 66, -15, -59, 28, 38, -41, -10, 43, -11, -33, 15, 17, -4, -3, -11,
-3, 13, 8, -6, -14, 2, 11, 1, 2, -6, -15, 15, 17, -26, -4, 25, -7, -18, 9, 14,
-14, -6, 16, -11, -2, 24, -20, -26, 32, 23, -34, -13, 25, 0, -7, 4, -3, -7, 0,
13, 0, -12, 5, 2, -12, 11, 24, -25, -27, 22, 25, -14, -14, 9, -9, 2, 29, -15,
-32, 28, 16, -29, -1, 23, -6, -20, 9, 13, 2, -12, -16, 11, 26, -15, -25, 27, 7,
-29, 13, 24, -31, -14, 38, 2, -39, 11, 34, -27, -27, 44, 20, -64, -4, 62, -8,
-43, 14, 21, -18, 4, 12, -20, -1, 21, -9, -15, 20, 0, -19, 9, 3, -2, 10, -12,
-13, 17, 14, -14, -14, 11, 8, -5, -5, -1, 7, 0, -3, -1, -3, 6, 5, -11, -5, 22,
-1, -33, 12, 37, -22, -28, 27, 4, -17, 21, -3, -35, 19, 36, -22, -31, 21, 16,
-17, 4, 8, -9, 1, 4, -13, 6, 22, -14, -28, 14, 38, -18, -38, 21, 25, -20, -12,
15, -1, -10, 5, 5, -2, 0, -8, -1, 13, -5, -7, 10, -4, -19, 28, 15, -50, -1, 56,
-13, -50, 23, 29, -23, -11, 19, 1, -15, 3, 11, -7, -4, 11, -11, -9, 15, 14, -14,
-28, 18, 27, -17, -16, 12, -4, 2, 12, -11, -5, 11, -5, -11, 22, 6, -28, -3, 21,
2, -2, -7, -22, 18, 33, -22, -37, 28, 26, -29, -3, 18, -10, -9, 14, 5, -15, 2,
18, -13, -18, 29, 9, -36, -1, 28, 0, -13, 0, -5, -1, 26, -6, -34, 21, 22, -30,
-7, 36, -6, -37, 15, 25, -11, -8, -1, -2, 8, 6, -3, -11, -9, 13, 13, -11, -14,
8, 17, -11, -17, 16, 15, -18, -11, 15, 13, -12, -16, 18, 10, -18, 0, 14, -12,
-9, 26, 0, -32, 9, 32, -17, -20, 21, 1, -15, 9, 10, -11, -11, 10, 8, -5, 4, -6,
-14, 13, 17, -10, -18, 7, 15, -6, -9, 0, 7, 5, -10, -11, 23, 7, -35, 4, 36, -13,
-29, 21, 14, -24, 5, 23, -19, -11, 19, 4, -12, -2, 7, -4, 1, 7, -10, 0, 7, -9,
-1, 16, -1, -25, 4, 28, -6, -26, 10, 18, -19, -6, 25, -2, -30, 13, 27, -23, -14,
20, 2, -15, 5, 17, -8, -18, 7, 15, -1, -13, -4, 12, 3, -9, 1, 8, -6, -5, 9, 4,
-10, -8, 16, 9, -21, 2, 19, -14, -9, 23, -3, -29, 16, 23, -26, -4, 24, -14, -20,
30, 11, -35, -1, 28, -8, -13, 13, 2, -12, 0, 11, 6, -10, -11, 13, 5, -11, -1,
11, -2, -16, 8, 21, -9, -20, 7, 18, -4, -15, 6, 10, -12, 1, 17, -10, -15, 13,
12, -15, -4, 11, -4, -3, 7, -7, 0, 7, -8, 3, 9, -4, -14, 9, 17, -14, -11, 13, 7,
-12, -5, 16, 1, -21, 4, 21, -7, -16, 12, 5, -15, 9, 13, -15, -7, 14, 1, -5, 3,
-5, -2, 12, -8, -9, 16, -2, -16, 14, 10, -21, 0, 17, -12, -7, 15, -4, -16, 13,
15, -19, -10, 18, 3, -15, 4, 14, -12, -10, 17, 8, -19, -3, 17, -8, -9, 19, 2,
-26, 1, 25, 2, -20, -6, 13, 8, -7, -10, 6, 9, -11, -7, 18, -1, -17, 2, 12, 0,
-9, 1, 3, -3, 2, 5, -3, -9, 5, 12, -11, -9, 15, -1, -12, 6, 4, -4, 2, 1, -11,
11, 12, -20, -10, 23, 4, -20, 5, 10, -8, 0, 6, -9, -5, 13, -1, -11, 7, 7, -10,
-4, 10, 2, -9, 0, 3, 2, 2, -3, -3, 1, 4, 2, -4, -5, 3, 7, -2, -6, -1, 9, 1, -16,
6, 20, -16, -21, 20, 17, -21, -10, 17, -1, -10, 9, 4, -16, 0, 21, -2, -21, 1,
20, -3, -21, 11, 16, -15, -10, 12, 9, -7, -7, -4, 9, 9, -11, -10, 10, 4, -7, 2,
3, -6, -1, 7, -3, -6, 10, -2, -16, 6, 16, -8, -11, 5, 4, -1, 2, -6, -6, 11, 7,
-13, -6, 13, 1, -14, 5, 12, -9, -7, 8, 2, -4, 3, 1, -10, 5, 13, -10, -12, 11, 8,
-8, -2, 1, -3, 2, 3, -3, -2, 3, 0, -4, 0, 6, -1, -8, 0, 10, 3, -12, -3, 10, 3,
-7, -1, 2, 1, -3, -3, 6, 0, -8, 4, 6, -8, -3, 11, -2, -14, 10, 14, -19, -10, 20,
5, -18, 0, 11, -3, -5, 2, 2, -1, -1, -1, 1, 3, -1, -3, -2, 4, 3, -2, -3, -2, 4,
6, -3, -7, 1, 8, -2, -6, 4, 1, -4, 4, 1, -5, 4, 5, -10, 0, 12, -5, -12, 9, 7,
-9, -3, 5, 1, -2, 1, -1, -2, 2, 2, -3, -1, 2, 1, -2, -1, 5, -1, -7, 2, 5, -2,
-1, 1, -7, 2, 9, -5, -9, 4, 7, -4, -3, 5, -1, -4, 4, 1, -1, 2, -3, 0, 4, 1, -3,
0, 1, -2, 1, 5, -3, -6, 6, 4, -7, -1, 6, -2, -7, 6, 6, -8, -4, 7, 0, -5, 2, 3,
-6, 1, 8, -4, -4, 3, 1, -2, 1, 0, -2, 2, 2, -1, 0, 0, -2, 2, 2, -1, -2, 1, -1,
1, 3, -1, -2, 1, 3, -2, -1, 3, 0, -3, 0, 3, 0, -3, -1, 4, -1, -1, 2, 0, -2, 3,
1, -3, 0, 3, -1, -2, 2, 0, 0, 3, -1, -2, 1, 2, -1, -1, 2, 0, -3, 0, 4, -1, -3,
1, 1, 0, 0, 1, -1, -1, 1, 1, 0, -1, -1, 1, 1, -1, -1, 1, 0, -2, 0, 1, -2, -2, 2,
0, -1, 0, 1, 0, -1, 2, 1, -1, 1, 0, 0, -1, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
0, 0, 1, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, -1, -1,
1, -1, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, 1, 1, -1, 0, -1, 0, 0, 0, 1, 0, 0, -1, 0,
-1, 0, 1, 0, 0, 0, 0, -1, -1, 0, 0, 0, -1, 0, -1, 0, 0, 0, -1, 1, 0, 0, 0, 0, 1,
0, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 1, 0, 0,
1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, };

#endif /* s28_H_ */