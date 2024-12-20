#ifndef s22_H_
#define s22_H_
 
#include <Arduino.h>
#include "mozzi_pgmspace.h"
 
#define s22_NUM_CELLS 8098
#define s22_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) s22_DATA [] = {1, 0, -1, 0, 0, 0, 0, -1, 1, -1, 0, 0,
0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 0, 1, 1, 0, 2, 6, -6, 7, 4, -3, 1, 0, 1, 0, 2,
0, 1, 0, 1, 0, -1, -2, -1, -2, -2, -3, -2, -2, -3, -2, -1, -1, -2, 0, 1, 1, 0,
1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, -1, -1, -3, -4, -4, -4,
-4, -2, -2, -1, -1, -2, -1, -3, -3, -2, -2, -1, 1, 1, 1, 4, 3, 5, 6, 7, 7, 7, 9,
8, 7, 6, 5, 3, 2, 2, 1, 1, 0, -1, 0, -2, -3, -1, -2, -2, -3, -2, -2, 0, 1, 1, 2,
3, 4, 3, 4, 5, 5, 4, 4, 3, 2, 2, 1, 0, 0, 1, 1, 0, 2, 0, -1, -2, -4, -5, -5, -5,
-5, -3, -4, -4, -2, -3, -2, -3, -2, -1, -3, -1, 0, -2, 0, 0, 0, 0, 1, 0, 1, 1,
0, -1, -1, -3, -2, -4, -2, -3, 0, -1, 1, -2, 0, -1, 3, -11, -12, 21, -7, -16,
-6, 13, 5, 2, 3, -2, 0, -4, 0, -2, 1, -1, 1, 0, 1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1,
0, -1, 0, 0, 2, 3, 3, 3, 4, 4, 6, 6, 7, 8, 8, 8, 7, 5, 4, 3, 2, 2, 2, 1, 1, 0,
-1, -2, -1, -2, -3, -3, -3, -3, -2, -2, -1, -1, 0, 1, 3, 2, 2, 4, 4, 3, 1, 2, 2,
1, 1, 2, 0, -1, -1, 0, 0, 0, -1, -2, -3, -4, -3, -3, -3, -4, -4, -3, -2, -3, -2,
-3, -3, -4, -4, -5, -4, -3, -4, -4, -2, -1, -1, 0, 4, -1, -7, 25, 9, -34, 16,
18, -12, -1, -1, 2, -1, 5, 2, 1, 0, 0, 0, -2, -1, -2, -1, -2, -1, -1, 0, -1, 0,
0, 1, 2, 3, 3, 3, 2, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 2, 3, 2, 2, 1, 1, 1, 0, 1, 0,
0, -1, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 3, 1, 1, 2, 0, 0,
0, 0, 0, -2, -1, 0, -1, 0, -1, -1, -1, -1, -2, 0, -2, -2, -1, -1, -2, -2, -2,
-1, -1, -1, 0, 0, 1, -1, 0, -1, 2, -1, 4, -1, 7, -5, 2, -18, 15, 32, -6, 6, -32,
1, -4, 1, 13, 8, -1, -5, -2, -7, 1, -3, 1, -1, 1, -1, 1, -1, 2, -1, 1, 2, 2, 1,
0, 1, 1, 0, 0, 1, 1, 2, 2, 2, 2, 3, 2, 3, 2, 2, 4, 4, 3, 3, 2, 3, 2, 2, 1, 1, 0,
1, 1, 0, -1, -1, -1, -2, -1, -1, -1, 0, -2, -2, -1, -2, -2, -2, 0, 0, 1, 0, 0,
1, 1, 1, 3, 3, 1, 0, 1, 1, 1, 2, 2, 1, 1, 3, 1, 1, -1, -1, -3, -1, 1, -2, 1, 1,
0, -1, -1, 1, 1, 1, -1, -1, -2, -2, -1, 0, -1, -1, -1, 0, 1, 0, 0, 0, -2, -1,
-1, 1, 1, 2, 2, 1, 3, 2, 0, 2, 0, -3, 0, 0, 2, 1, 0, -3, -4, -3, -3, -1, 0, 1,
0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 0, 1, 0, 1, 0, -1,
1, 0, 1, 1, 1, 1, 0, 0, -1, -1, 0, -3, -2, -1, 0, 0, 0, 1, 0, 1, 3, 3, 2, 0, 1,
2, -2, -1, 1, -1, 0, 0, -2, -1, -1, -2, -2, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0,
1, -1, 0, 2, 0, 1, 2, 1, 1, 0, 0, 0, 0, -1, 0, 0, 0, 1, 4, 1, 3, 4, 5, 4, 4, 3,
1, 1, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0, 1, 1, 1, 1, 0, -1, -1, -1, 1, -1, -1, 1, -2,
-2, -2, -3, -3, -2, -3, -2, 0, -3, -2, 0, -1, 2, 1, 1, 1, 2, 1, 2, 1, 2, 1, 3,
0, 0, 3, 7, -2, -5, -5, -12, 11, -5, -8, 14, 4, -8, -6, -7, -8, 1, 1, 2, 0, 2,
-3, -2, -1, -1, 0, 0, 2, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, -1, -1,
-1, 0, 0, -1, -2, 0, -4, -2, 1, -3, 0, 2, 1, 1, 1, 0, 1, 1, 1, 0, -1, 3, 1, 0,
1, 2, 3, 1, 1, -1, -2, -1, -1, -1, -2, -2, -1, -1, -3, -2, -1, -2, -1, 1, 1, 2,
1, 1, 0, 0, 2, 2, 2, 1, 1, 1, 3, 2, 2, 1, 1, 1, 1, 1, -1, -1, 0, -1, -1, -1, -1,
-1, -1, -2, -2, 0, -1, -3, -3, -2, -1, -2, -2, -2, -1, -2, 0, 1, 0, 1, -1, 2,
-1, -1, 0, 2, 3, 0, 1, -1, -1, -1, 1, 2, 1, 0, 0, 0, -2, -1, -1, -1, 0, -1, -1,
0, 0, -1, -1, -1, 0, 1, 1, 0, -1, 1, 2, -1, -1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0,
-1, 0, 0, -1, 1, 0, -2, 0, 1, 0, 1, 1, 2, 2, 2, 0, 0, -1, 1, 2, 1, 0, 0, 0, 0,
-1, -1, 1, 0, 0, 1, -1, -2, -1, -1, -3, -3, -1, -1, -3, -3, -1, 0, -1, -1, 0, 0,
2, 0, 0, 1, -2, -2, -2, 1, 1, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1,
-2, -2, 2, 0, -3, 1, 0, -1, -2, -2, -2, 1, -1, -2, -1, 0, -1, -1, 0, -1, -1, 0,
-1, -1, 0, 0, -1, 3, 2, -5, 1, -8, -8, 24, 16, -18, 8, -4, -15, -5, -7, 7, 10,
6, -3, -1, -5, -2, 1, 1, 1, 0, 1, 0, 1, 2, 3, 1, 2, 0, 1, 2, 2, 1, 1, 0, 0, 0,
0, 0, -1, -3, -2, -1, 1, 0, -1, 1, 1, -2, 0, 2, 0, 1, 0, -1, -1, -1, -1, 0, 0,
0, -2, -2, 1, 1, -1, 1, 0, 1, 0, 0, -1, -2, 0, 1, 1, 0, 0, -2, -3, -2, 0, 1, 0,
1, 0, -2, -2, -1, 1, 1, 2, 2, 3, 0, -1, -1, -1, 1, -1, -1, -1, -2, -3, -1, -1,
0, 1, -1, -1, 0, -1, 0, -1, -2, -1, -1, 1, 0, -1, 1, 1, -1, -1, -2, -2, -1, -2,
-1, 0, 0, 0, -1, -1, -1, 1, 1, -2, -2, 1, 1, -1, 1, 1, -2, -1, -1, 0, -1, 1, 1,
1, 1, 0, 0, 1, 2, 0, 2, 2, 2, 1, 0, 1, 0, 1, 0, 0, 1, -1, -1, -1, -2, -2, -1, 0,
1, 1, -1, 1, 2, 3, 1, 2, 1, 0, -1, 0, 1, 0, 0, 0, 1, 0, -1, -2, -1, -2, -2, -2,
-2, -1, -1, -1, -1, -2, -1, -2, -1, -1, -1, -2, -1, -2, -1, -2, -2, -1, -1, -1,
1, -1, 2, 3, 1, 1, 1, 2, 3, 3, 4, 2, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, -1,
-2, -1, -1, 0, 0, 0, -1, 1, 1, 2, -1, -2, 3, 0, -2, -1, -2, -1, 0, 0, 0, 0, -1,
-1, 0, 1, 1, 1, 0, 0, 0, -1, 0, -1, 0, 1, 1, 2, 2, 0, 2, 2, -1, 2, 2, 0, 0, 0,
1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, -2, -1, -2, -2, -1, 0, 0, 1, 1, 1, 1, 1, 0, 0,
0, -1, 0, 0, -1, 0, 0, 1, 0, 1, 0, 3, -2, 1, 1, -2, -1, -3, 0, -2, 0, -2, 1, -3,
0, -3, 0, -3, -1, -4, 1, -3, -4, -8, 21, 6, -33, -16, -11, 56, 2, -22, -8, -4,
5, -1, 4, 1, 3, -3, 5, -2, 5, 0, 3, -1, 2, 1, 1, 1, 1, 1, -1, 0, -1, 0, -1, 0,
1, 2, -1, 1, 0, -1, -2, 1, 1, 1, 0, 1, 2, 2, 2, 1, 2, 1, 0, 0, 0, -1, -1, -1,
-1, 0, 0, 0, -1, -1, -2, -1, -1, -2, -3, -1, -1, -2, -4, -4, -1, -2, 1, 2, 2, 2,
2, 0, -1, 0, 2, 2, 1, 1, 0, 1, 0, 1, 0, -1, -1, 1, 4, 1, 1, -1, -1, -1, -1, -3,
-1, 0, -1, 0, -1, 2, 2, -1, 1, 0, -1, 0, 0, -1, 1, 2, 0, 1, 0, 3, 2, 1, 1, 1, 0,
1, 2, 0, -1, -1, 0, 2, -1, 1, -1, -3, 1, 0, 0, -1, -2, 0, 0, -1, 0, 3, 2, 0, -1,
1, 0, 0, 1, 0, 0, 1, 0, -1, -2, -1, -1, -1, 0, 1, 0, 0, 0, 0, -1, -1, 0, -1, -2,
0, 0, -2, -1, 2, 0, -1, 1, 3, 2, -1, 1, 1, -1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
0, -1, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, -1, 1,
0, 2, 0, 1, -3, 1, -1, 1, -1, 1, 0, 1, 0, -1, 0, 1, 2, 0, 1, -5, 2, -5, 5, -1,
4, -7, 6, -8, 9, -2, 11, -19, 4, -17, -1, 80, -7, -14, -73, 14, -16, 26, 45, -5,
0, -22, 1, -11, 6, -6, 8, -4, 3, -6, 2, 0, 2, 1, 1, -1, -1, 0, -2, 2, 1, 1, 0,
1, -2, 1, 1, 1, -1, 2, -1, 2, -1, -1, 0, -1, -1, 0, 0, 1, 1, -1, -2, -1, 0, -2,
-2, -2, -1, -2, -1, -1, -1, 0, 1, -1, 0, 1, 0, 2, 2, -1, 2, 3, 0, -2, -3, 0, 1,
4, 7, 6, 3, 0, -1, -1, 2, 4, 1, 1, -1, 1, 2, 1, -1, -3, -1, -1, 2, 1, 1, -1, -1,
-3, -1, 2, 3, 1, 0, 0, 0, 0, 2, 3, 1, 1, 2, -1, 1, 1, 1, 0, -3, 0, -2, 1, 0, 0,
1, -2, 2, 2, -1, -2, -3, -4, -3, 2, 4, 4, 3, 0, -3, -3, -4, 2, 2, 2, -1, -4, -2,
-2, 1, 2, 2, -1, -2, -2, -2, 1, 2, 1, -2, -2, -2, -1, 0, 0, -1, -1, 1, 1, 2, 0,
0, 0, 0, 0, 0, 1, 1, 0, 1, 2, 1, 2, 1, 1, 1, 0, 0, 0, 1, -1, 0, 0, 0, 0, -1, 0,
0, 0, 1, 0, 1, 0, -2, 0, -1, 1, 2, 1, 0, 2, -3, 1, 0, 2, -2, 3, -4, 4, -1, 2,
-1, 4, -1, 0, -6, -14, -5, 33, 44, -53, 25, -12, -71, -12, 66, 34, -5, -9, -31,
2, -4, 13, -1, 8, -2, 2, -1, 5, -1, 1, 0, -1, 1, 0, 2, -1, 1, -2, 1, -2, 2, -2,
0, -2, 1, -1, 0, -2, 0, -2, 0, -3, 0, 1, 0, -2, -3, -1, -1, 2, 1, 1, -1, 0, -1,
0, 0, 0, 0, 0, -1, 0, 0, -1, -1, 0, 1, 0, 1, 3, -1, -2, -1, -3, -3, 0, 3, 5, 2,
0, 0, -4, -3, 2, 3, 2, 1, -1, 2, 2, 1, 2, -3, -3, 0, 3, 3, 2, 0, -1, -2, 0, 1,
0, 0, 1, -4, -4, -1, 2, 3, 2, -3, -4, -2, 2, 3, -1, 0, -1, -5, -3, 2, 2, 1, 0,
-1, 1, 0, 2, 2, -2, -7, -3, 3, 6, 2, 0, -1, -6, -4, 1, 0, 0, 3, 0, -1, -3, -1,
0, 0, 1, 0, 0, 0, 1, 1, 2, 2, 0, -1, 0, 2, 1, -1, 0, 1, 0, 1, 0, 2, 0, 2, -1, 0,
1, 2, 0, -3, 0, 0, 1, -1, -2, -2, 0, -1, -1, 0, 3, -5, -1, -2, 4, -1, 3, -1, -1,
-4, 0, 3, -1, 2, 0, 3, 4, 6, -62, 28, 75, -22, -59, 5, 20, -5, 10, -7, 5, -3, 1,
-5, 3, 2, 2, -2, 1, 1, 0, 1, -1, 1, -1, 0, 0, 2, 0, 2, -1, 0, -1, 1, 1, 2, 1, 0,
-1, 0, 1, 0, -1, 0, 1, 0, 0, -2, 0, 0, -2, 1, 1, 2, -2, -4, 0, 4, 4, 0, 3, -1,
-1, -2, -1, 2, 2, 2, 0, -1, 0, -3, -3, 2, 2, -2, -1, 0, -1, -2, 1, 2, 0, -2, -1,
0, 0, 2, 1, 1, -5, -3, 1, 6, 1, -1, 1, -2, -1, 0, 1, 1, 0, 1, -1, -1, 3, 2, -2,
-2, -1, 0, -1, 0, -1, -1, 0, -1, -4, 0, 3, 0, -3, -1, 0, 0, 1, 0, 0, -1, -2, 2,
2, 0, -2, 0, -1, -2, -1, 4, -1, -4, -2, 1, 0, 0, 1, -2, -1, 1, -1, 1, 1, -1, 2,
-3, -3, 0, 3, 3, 3, -1, -4, -2, 2, 3, 2, 2, -2, -1, 0, 1, 0, 1, 1, 1, 0, 0, 1,
2, -1, -3, -3, 2, 3, 1, -1, 0, -4, -2, -1, 0, 0, 2, 2, -3, -3, -1, 5, -6, -3,
26, 17, -65, 6, 27, -6, 4, -3, 11, -3, 6, -2, -2, -9, 4, 2, 0, 4, 1, -5, -5, -2,
0, 5, 2, 1, -5, -2, -1, 4, 0, 1, -2, 0, -1, -5, -1, 3, 5, 1, 2, -2, -3, -1, 0,
0, 0, 0, -1, 0, 2, 0, 0, -1, -4, 0, 1, 4, 1, -1, 0, -3, -1, -1, 0, 0, 2, 0, 1,
1, 1, -1, -3, -3, -1, 4, 2, 2, -1, -2, 0, -1, 1, 2, -4, -1, 1, 1, 4, 5, 1, -4,
-5, -4, 4, 3, 2, 1, 0, -1, 2, 2, -2, 2, 1, 1, -3, 0, 2, -2, 0, -1, -1, -1, 0, 1,
2, 1, -2, -2, -4, 3, 3, 2, 4, 1, -2, -3, 0, 2, 5, 0, -1, 0, 0, -1, 0, 2, 2, -1,
-1, 1, -1, 1, 2, 1, 0, 0, -1, -2, -2, -1, -1, -1, 0, -1, -1, 0, -1, 1, -1, 0,
-1, 0, -2, -1, 1, 1, 2, 1, -3, -2, 2, -1, 3, 5, 1, -4, -4, -2, 0, 1, 2, 0, -2,
-4, -1, 2, -1, 3, 3, -3, -5, -1, -2, 3, 5, 1, -6, -4, -1, 2, 2, 0, -1, 1, 1, 1,
3, 1, -2, -3, 2, 1, 2, 4, 2, -2, -1, 0, 0, 2, 3, 2, -5, -4, 1, 5, -2, 2, 3, 2,
-3, -2, 0, 3, 2, 2, 1, 0, 1, 0, -3, -2, 2, -2, 1, 4, 1, -5, -4, -1, 1, 2, 2, 1,
-3, -4, 0, -1, 1, 4, 2, -3, -4, -3, -2, 4, 0, 1, 3, 1, -5, -5, -2, 1, 0, 0, 0,
0, -1, 0, 1, 0, -2, -4, 0, -1, 0, 3, 3, -1, -4, 0, 3, -1, -3, 3, 3, 1, 1, -1,
-3, 0, 3, -1, 1, 4, 2, -3, -3, 3, 3, 2, -3, -1, -2, 1, 3, 1, 1, 0, -1, -1, 0, 1,
1, 2, 1, 2, 3, -2, -3, 3, 6, -4, -2, 4, 0, 2, 2, -1, -4, -2, 0, 1, 1, 0, -2, -2,
1, 0, 0, 4, 2, -2, -3, -1, 0, 2, 1, 1, -1, -1, -1, 1, 1, -2, -2, -1, 2, 1, 0, 0,
-1, 0, 0, 0, -1, 0, 2, -2, -4, 0, 2, 0, 0, -1, -1, -1, 0, 0, -1, -2, 0, 3, -3,
-4, 2, 2, 1, 2, -3, -3, 1, 2, 0, 0, 2, 1, -2, -3, -1, 3, 1, -2, -1, -1, 1, 1, 3,
0, -2, -1, 2, 3, 2, -1, 0, 1, 1, 1, 0, 0, 1, 2, -2, -3, 3, 3, -2, -2, 0, 3, 2,
3, -1, -2, -2, 2, 3, 1, 3, -2, -3, 1, 4, 0, 0, 1, -1, 0, -1, 0, -1, 0, 3, -1,
-4, 0, 4, 2, -1, -1, 0, 4, -3, -5, 1, 3, 0, -1, -1, -1, 0, -1, -1, -1, 0, 0, 0,
1, 0, 0, 2, -2, -4, 1, 1, 1, -1, 0, -2, -1, 0, -1, -2, -1, 0, -1, -2, -1, 2, 0,
-4, 0, 3, 2, -1, 0, 0, 1, 0, 0, 0, 1, -1, -1, 0, 1, 0, 0, 2, 2, -1, 1, 0, 1, -1,
0, -1, 3, 7, -3, -1, 1, -4, -6, 4, 6, 1, -1, -2, -3, -1, 0, -1, -1, 2, 5, -2,
-1, -1, 1, -2, 2, 2, 1, 1, 2, -1, -2, -1, 0, 3, 0, 0, -2, -2, 0, 0, -3, 0, 3,
-1, -2, -1, -1, 0, 0, 0, -1, 0, -2, 2, 4, -1, -2, -3, -2, -3, 2, 1, 2, 3, -1,
-1, -1, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, -1, 1, 2, 0, -2, 3, -2, -1, 4, 2, -1, -4,
-1, -2, 0, 3, 2, -1, -2, -1, 2, 0, 0, 1, 0, 0, -1, 0, 0, 1, 0, 1, 1, 0, 0, -2,
0, 3, 1, 0, 0, 1, 1, -1, 1, 0, 1, 0, 1, -2, -2, 8, 3, -1, -4, -3, -1, 1, 1, 0,
3, -1, -2, -2, -1, -1, -1, 1, 1, 1, 0, 1, -1, -1, -1, 0, 1, -2, 0, 0, 3, 4, 1,
-3, -6, -2, -4, 6, 6, 4, -3, -5, -2, -1, 3, 1, 0, 2, 0, -1, 0, -1, -1, 0, 0, 0,
0, 0, 0, -1, 1, 1, 0, 2, -1, -1, 0, 1, 1, 1, 1, -1, -2, 0, 0, -2, 0, 2, -2, -1,
3, 0, -1, -4, 1, 4, -1, -4, -1, 2, 1, 0, -1, 3, 1, -1, -1, -1, -1, 0, -1, 0, 0,
-1, 0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 0, 0, 1, 1, -1, -1, -1, 2, 4, -2, 0, -1, -6,
-4, 3, 5, 3, 0, -1, -1, -1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0,
0, 0, -1, 0, 0, 4, 1, -4, -1, 1, -1, -2, 1, -1, 0, -1, 2, 1, 2, -1, -5, 0, 0, 1,
2, 2, -7, 9, -3, -7, 1, -1, 10, -2, 0, 1, 0, 2, -1, -1, -3, -5, -4, 3, 5, 2, -1,
-2, -1, -1, 0, 0, 1, 0, -1, 1, 1, 0, -1, 0, 0, 1, 0, -1, 0, -1, -1, 1, 0, 0, 0,
0, -1, 0, 0, 0, 0, -1, -2, 0, 3, -2, 1, 0, -5, -3, 3, 4, 3, 1, -1, 0, 0, 1, 0,
2, 1, 0, 0, 2, 1, 1, -1, -1, 1, 0, 1, -1, 0, 3, 1, -2, 1, -2, -5, -1, 9, 4, 1,
0, -5, -6, 0, 6, 3, 2, -1, -1, 0, 1, 0, 0, 2, 1, -3, 0, 1, 0, 0, 0, 1, 0, 0, -1,
0, -1, -1, 0, 2, 0, -3, -1, 0, 0, 2, 0, -1, 0, -1, -1, 0, 0, 2, -1, 1, 1, 0, 1,
0, -1, 0, 0, -1, 0, 0, -1, 0, 0, 1, -3, -1, 5, 0, -4, -3, 2, 1, 1, 0, 0, 0, 1,
1, 0, 0, 0, 0, 2, 1, -1, -1, -1, 2, 0, 0, -1, 1, 0, 0, 0, 2, -3, -1, 0, 4, -5,
-1, 4, 8, 12, -32, -6, 6, 9, 13, 3, 2, -2, 1, -3, -1, -2, 1, -2, 0, -1, 1, -1,
-1, -1, 3, 0, -3, 1, -3, -5, -5, 5, 7, 1, -7, -4, -1, 0, 2, 2, 1, 0, -1, 0, 0,
0, 1, 0, 1, 1, 1, -1, -1, 0, 1, 1, 0, 1, -1, -1, -1, 0, 0, 0, 0, -1, -1, 0, 0,
1, 0, 0, 0, -4, 0, 4, 0, -4, -1, 2, 1, 2, 2, -1, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
0, -1, 0, 0, 1, 0, 1, 1, -1, -5, 3, 5, 0, -4, -1, -3, -2, 5, 4, -1, -2, 2, 1, 0,
1, 1, 0, 1, -2, -1, 0, 1, 1, 1, 0, 0, 1, 1, 1, -1, -2, -1, 1, 2, 0, 1, 2, 1, -2,
-1, 0, 1, 2, 1, 1, 0, 1, -2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, -1, 0, 2, -3, -2,
1, 0, 0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 0, 0, 0, 1, 1, 0, 0, -1, 1, -1, 0, -1, 1,
2, 1, 1, 0, 0, 0, 0, 2, 1, -2, 1, 1, 1, 0, 1, 0, 1, 0, 1, -2, 1, 0, 1, -2, 1, 1,
2, -2, -3, 6, 4, -2, -4, -2, -3, 3, -3, 2, 2, 16, -13, 13, -22, -14, 0, 14, -47,
63, 100, -87, 46, -111, -27, 46, -38, 48, 37, 27, -11, -8, -22, -9, -11, 0, -3,
5, 5, 3, 1, -1, -3, 1, 1, 0, 0, 0, 1, -1, 0, -1, 0, 0, 3, -1, 0, -1, 0, -1, 0,
-1, 0, 1, 0, 2, 3, 2, -1, -1, -1, 0, 2, -1, -1, -3, 0, 0, 3, 1, 3, -1, 3, -1, 1,
-4, -1, -2, 4, 0, 5, 5, -3, -1, -8, -6, -4, 1, -8, 10, 7, 34, -15, 26, -33, -45,
-28, 12, 81, -9, 15, -21, -11, -18, -2, -2, 7, 7, 1, 5, 1, 4, -6, 2, 0, 4, 0, 0,
-7, 2, -5, 2, 0, 4, 1, 1, -2, 2, 3, 0, 1, -5, -3, -3, -2, 1, 3, 1, 2, 4, 2, -1,
-3, -7, -2, -5, 0, 8, 3, 5, 0, -6, -1, -4, -2, 1, 0, 1, 1, 0, 0, 0, 1, -1, -1,
-2, -3, -2, -1, 3, 4, 2, 2, -1, -4, -4, -2, 4, 4, 3, 0, 1, 0, -1, 1, -2, -3, -3,
1, 3, 1, 3, 0, 0, -1, 0, 0, 0, 0, -2, -2, -1, 4, 3, 4, 0, -3, -1, 0, 1, 1, 2,
-1, -2, -3, -1, 3, 2, 3, 1, 1, -1, -1, -1, -3, -3, -3, 3, 6, 3, 4, 0, -4, -5,
-4, 1, 1, 2, 2, 0, -1, -2, -2, -1, 0, 0, 1, 1, 2, 1, 1, -1, -1, -1, -1, 0, 1, 1,
2, 1, 2, 1, -1, -3, -1, 0, 0, -1, 1, 1, 0, 1, 0, -1, -3, -3, -1, 0, 0, 0, 1, 1,
0, 1, 2, 1, 1, -1, -2, -2, -1, 0, 2, 1, 0, 1, 1, 0, 0, 0, 0, 0, -1, -1, 1, -1,
-1, -1, 1, 1, 1, 0, 1, -1, -1, 1, 1, 1, 1, 0, -2, -1, 0, 1, 1, 2, 0, 0, -1, 1,
1, -1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 0, 0, 1, 0, 0, 0, 2, 0, 2, -2, 0,
-1, -1, 0, 1, -3, 14, -13, -50, 80, 2, -66, 33, 1, 4, 3, -5, -2, -3, 1, 1, 3, 0,
1, -1, -1, -1, 0, -1, -1, 0, 0, 0, 1, 0, 0, 0, 0, -1, -1, 0, 0, 0, -1, 0, 1, 0,
-1, 0, 0, 0, -1, -1, -1, 0, 1, 1, 1, 0, -1, -1, -1, 0, 1, 0, 1, 0, 1, 0, 1, -2,
0, -1, 0, 1, 1, -2, 3, 1, -12, 0, 1, 5, 8, -3, 2, 0, -6, -3, -1, -2, 1, 1, 1, 0,
1, 1, 0, -3, -2, 3, 0, 0, 1, -1, 0, 0, -1, 0, -1, 3, 1, -2, 0, 3, 0, 1, 0, -1,
0, 0, 0, 0, 0, 0, -1, -2, 3, 0, -2, 0, -2, 0, 0, 1, 0, 1, -2, -1, 1, -2, -1, 0,
1, 1, 1, 4, 1, -3, 2, -12, -5, 3, 2, 12, 3, 2, -1, -5, -6, -5, -1, 0, 3, 6, 6,
4, -6, -6, -4, -1, 2, 3, 3, 1, 1, -1, -2, -1, -1, 1, 2, 0, 0, 2, -1, -12, 1, -2,
6, 10, 0, 2, -2, -5, -5, -3, -1, 2, 2, 2, 3, 2, -1, -2, -1, -1, 1, 0, 0, -1, -2,
0, 0, 1, 1, 0, -1, 0, -2, -1, -1, 1, 1, 1, 0, 0, 0, 0, 1, -1, 0, 0, 0, -2, 9,
-3, -1, -3, -8, 8, 2, 2, 4, -2, -5, -2, 0, 0, 0, 3, -10, -1, 3, 0, 11, 4, -10,
-3, -4, -4, 9, 3, 3, 2, -3, -4, -6, -2, -1, 2, 10, 9, 3, -8, -2, -10, -9, 5, 1,
13, 5, -1, 0, -4, -7, -3, -2, 0, 3, 5, -8, -7, 2, 1, 14, 4, 0, 0, -7, -8, -5,
-1, 4, 3, 1, 1, 5, -7, -8, 3, -2, 12, 4, -2, 3, -6, -8, -5, -1, -2, 11, 1, -4,
-1, -10, 10, 8, 0, 4, 3, -13, -15, -2, -2, 13, 13, 6, 6, -11, -15, -8, 2, 8, 10,
1, -3, 0, -3, 3, -2, -12, 2, -1, 7, 10, -3, 3, -6, -7, -11, 2, 4, 10, 5, -5, -2,
2, -3, -10, 6, -3, 7, 5, 1, 3, 1, -2, 0, -5, -8, -3, 0, 4, 4, 5, 2, 2, -3, -4,
-6, -2, -5, 5, 7, 6, -1, -6, -12, -8, 9, -4, 9, 6, -4, 12, -5, -3, 1, 2, -1,
-10, -5, 1, 0, -11, 12, 8, 17, 3, -14, -1, -6, -5, -3, 2, 3, 3, 0, 2, 0, 4, -7,
-4, 3, 1, 6, 4, 1, -8, -6, -2, 2, 1, 3, 3, 2, 2, -2, -3, -5, -2, -2, 5, 8, 8, 1,
-11, -4, -2, -1, 2, 2, 1, 1, 1, 1, 2, 0, -2, -2, -2, 0, 3, 2, 1, 2, 0, -1, -1,
-2, 0, -4, 0, 5, 3, 0, -2, 3, -6, -5, -1, 0, 6, 4, 0, -3, -1, -5, -1, 6, 6, 4,
-8, -8, -3, 0, 2, 5, 3, 1, -1, -1, -1, 0, 0, -2, 0, 0, 0, 0, 1, 2, 1, -1, 0, -1,
0, -1, -2, -2, -1, 0, 0, 2, 1, 2, -1, 2, -4, -3, 2, 2, 0, -1, 0, 0, 0, 1, 0, -1,
-1, 0, 0, 1, 1, 1, 0, 1, 1, 0, -2, -2, 0, 0, -1, 2, 2, 0, -1, -1, 0, 1, 2, 1, 0,
0, -1, 2, 1, 0, -1, 0, -1, 0, 0, 1, 0, 1, 0, -3, 0, 3, 0, 0, -1, -1, -1, 1, 2,
0, 2, -3, 1, -1, -4, -4, 11, 9, 0, 2, -21, -5, 5, -3, 5, 8, 7, 2, -3, -5, -3,
-2, 0, 0, 0, 1, -1, 1, 2, -5, -1, 4, -1, -2, -1, -1, 0, 0, 0, 0, -1, -2, 2, 1,
-5, 2, 4, -3, 0, 1, -2, -2, 3, 3, -1, 0, -2, 0, -1, 3, 3, -4, 0, 2, -2, 1, 2, 2,
1, 0, 0, 0, -2, 0, 2, -4, -1, 5, 1, -1, -2, -3, 0, 3, -2, -2, 3, 1, 4, 1, 3, -4,
-7, -8, 1, 11, 4, 3, -2, -2, -5, -1, 2, -2, -1, 4, 1, 0, -1, -2, 0, 2, 2, 0, -1,
-2, -1, 0, 0, 1, 3, -3, -2, 3, 0, 0, 1, 1, -2, -2, 0, 1, 2, 1, 0, 0, 1, 1, -1,
-1, -2, -2, -1, 0, 1, 2, 1, 0, -2, 1, 1, -6, 2, 5, -5, 0, 3, 0, -1, 0, 1, 1, 1,
2, -1, -1, 4, -2, -1, 4, -1, -2, -1, 0, 0, 1, 0, 2, 0, -3, -1, 0, 0, 0, 0, 0, 0,
1, 1, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, -1, -1, 1, 1, 1, 0, 0, 0, 0,
1, 1, 1, 1, 0, 0, -1, 1, 0, -2, -1, 0, 1, 1, 2, 0, 0, -1, -1, 0, -1, 1, 0, 1, 0,
1, 0, -1, -1, 0, 1, 1, 0, 2, 0, 2, -1, -4, -1, 1, -4, 5, 14, -1, 5, -13, -13, 1,
-2, 4, 8, 8, 3, -1, -2, -4, -4, -1, 0, 0, 1, 1, 1, -1, 0, 2, 1, 0, 2, 2, 0, 1,
-3, -3, 0, 1, -6, 12, 10, -1, 2, -16, -5, 1, -3, 5, 9, 6, 1, -3, -3, -3, -4, -1,
2, 1, -1, 1, 1, -3, 0, 3, 3, -2, -3, -1, 0, -7, 9, 13, -2, 2, -13, -8, 1, -3, 4,
6, 7, 1, -3, -2, -1, 2, 0, 2, -3, -8, -7, -2, 12, 6, 4, -1, -2, -4, -2, 0, 2, 0,
1, 2, 1, 2, 2, 0, -5, 3, 2, -3, -4, 7, 13, -4, 3, -13, -11, 3, 0, 7, 10, 11, -5,
-9, -11, 0, 8, 2, 1, -1, -1, -3, 1, 1, -1, 1, -2, -3, 0, 4, -3, 13, 12, -8, -2,
-18, -10, -4, 15, 11, 13, 4, -12, -6, -14, -9, 5, 20, 7, 3, -4, -6, -7, -3, -1,
0, 1, 2, 1, 1, 2, -1, 1, 0, 1, -2, 0, -1, 0, 4, 0, 0, 2, -1, -2, -1, -2, 0, 1,
0, 0, 1, -1, 4, 1, 3, -4, -7, -7, -1, 14, 5, 2, -1, -3, -6, -1, 0, 2, 3, 2, 1,
2, 1, -3, 0, 0, 0, 0, 1, -1, 2, 2, 3, -2, 4, -8, -9, -7, 10, 16, 8, 6, -12, -12,
-12, 5, 8, 6, 2, -2, -1, -3, 0, -1, 0, -2, 1, 1, 1, 2, 2, -2, 0, -1, -1, -1, -1,
-1, -1, 2, 2, 3, -1, -2, 0, 1, -3, -2, 0, 0, -1, 16, 5, -1, -4, -19, -1, 0, 0,
9, 11, -1, -4, -2, -2, 3, 0, 3, -1, -3, -2, -2, -1, 1, 1, 0, 0, 1, 0, 0, 0, 1,
-1, -2, 0, -1, -1, 1, 3, 3, 0, -2, -1, -1, -2, -1, 1, 1, 1, 0, 1, 2, 1, -1, -2,
-1, 0, -2, 0, 2, 2, 0, -1, 0, 0, -2, 0, 1, 0, 0, 1, 1, 1, -1, -1, -2, -1, -2,
-1, -1, 2, 3, 3, -3, 6, 5, -12, -7, -6, 9, 12, 5, 0, -6, -5, -5, 2, 3, 5, -2,
-3, -1, 0, 2, 0, 0, 0, 2, 1, 0, -1, -2, -2, 1, 2, 1, -1, 0, -2, -1, 8, 0, -5, 0,
0, 0, 0, -1, -1, -1, 1, 2, 1, 0, -2, -2, -2, -2, 0, 0, 2, 2, 0, -1, -1, -1, 0,
0, 1, -1, -4, 6, 2, -4, 1, 0, 0, -2, -2, -1, 0, 2, 2, 1, 0, -1, -3, -2, 0, 1, 1,
0, 0, 0, 0, 1, -1, -1, -2, -1, -1, 1, 1, 1, 1, -1, -1, -1, 0, 0, 0, 1, -1, -4,
5, 3, -6, 0, 0, 1, 1, -1, 0, 0, 0, -1, -1, -1, 0, 1, 1, 1, -1, -1, 0, -1, 0, -1,
0, 1, 1, 2, -1, -3, 5, -2, -4, 3, 1, -4, 1, 5, -4, -1, 2, 1, 1, -1, 0, 1, 0, -1,
0, 0, -1, -1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, -1, -4, -3, 1, 0, 6, 2, 0, 1,
-1, -2, -1, 0, 1, 0, 0, 1, 0, -1, 0, 0, 0, 1, -1, 0, 0, -1, 0, 0, -1, 0, 0, 0,
0, 0, 0, 0, -1, -1, -1, -1, 1, 1, -1, 1, 0, -1, 2, 0, -1, 1, -1, -5, 1, -2, 3,
5, -1, 1, 0, -3, -2, -2, 1, 1, 2, 1, 1, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, -1, -2, -1, -2, 2, 4, 3, -2, -3, 0, -6, 0, 2, -1, 4, 6, -2, -3, -1, -3, -1,
0, 0, 1, 0, -1, 0, 1, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1, 0, 1, -1, 0, 0, 0, 1,
1, 0, 0, 0, -1, -1, -2, 0, 1, 2, 3, 4, -2, -8, -1, -2, 5, 3, 0, 2, -1, -3, -3,
-1, 0, 1, 1, 1, -1, 0, 0, 0, 1, 0, -1, 1, -1, 0, 0, 1, 0, 1, 0, -1, -2, 0, -3,
1, 4, 0, 3, -1, -4, -3, -4, -1, 2, 2, 3, 0, -1, -2, 1, -1, 2, -3, 4, -3, -3, -3,
7, -5, -30, 35, 70, -82, -73, 92, 12, -29, -11, 3, 8, 0, 8, 3, 3, -5, 1, -4, -1,
0, 1, 1, 3, -1, 0, 0, 1, 0, 0, -3, -1, 0, 1, 1, 0, 1, 0, 0, -1, 0, 0, 0, -1, 0,
0, 1, 0, 1, 1, 1, -3, -3, 0, 0, 2, 2, 1, 2, -4, -1, -2, -1, 1, -2, 4, 1, 0, -1,
-2, -2, 2, -3, -4, 4, 1, 0, 1, 1, 0, -3, -1, 2, -1, -2, 3, 3, 0, -2, -1, -1, -2,
0, 6, 5, -4, -5, -2, 0, 0, -1, 0, 1, 0, 2, 0, 1, 1, -2, -5, 1, 3, 0, 1, -1, -3,
1, 0, 3, 1, 1, 4, -4, -5, 0, 1, 0, 0, 0, 0, 0, -1, 3, 1, -5, -2, 4, 2, 0, 0, 0,
-3, -5, 3, 4, 1, 3, -1, -2, -2, 0, 2, 0, 1, 1, 1, -1, 1, -1, -1, 0, -3, 3, 2, 0,
2, 0, -1, -2, -1, 0, 0, 1, 1, 1, 1, 0, 0, -1, -1, 0, -1, 0, 0, 1, 2, -1, -1, 0,
-1, -1, -1, -1, 1, 1, 0, 0, 0, 0, 0, 2, 1, 0, -3, -1, -1, 2, 0, -1, 1, 0, 1, 0,
-1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 1, 0, 1, 1, 0, 0, 0, 0, -1, 0, 0, 1, 1, 1, 1,
0, -1, 0, -1, 2, 2, 2, 0, -4, -2, 1, 0, 2, 2, 1, -1, 1, -2, -4, 1, -1, 3, 3, 1,
1, -2, -2, -2, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, -1, 0, 0, 1, 0, 0, -1, 0, 0,
0, 0, 1, 1, 0, 0, 1, -1, -1, 1, 1, 0, 0, 1, -1, -3, 0, 0, 4, 3, -1, 1, -1, -3,
-1, -1, 1, 2, 1, 0, 0, -1, -1, -1, 0, 0, 0, -1, 0, -1, 0, -2, 1, 2, 0, 0, 1, 1,
0, 0, -2, 0, -1, 1, 2, 2, 1, -4, -1, -1, 1, 1, 1, 0, 0, 1, -1, 0, 0, 0, 0, 0, 1,
-1, -1, 0, 1, -1, 0, 0, 0, 0, 0, 0, -1, -1, 1, 1, 0, 0, 0, 0, 1, 0, 0, -1, -2,
-1, -1, 1, 2, 2, 1, -3, -2, 0, 0, 0, 1, 1, 0, -2, 0, -1, 1, 1, 1, -1, 0, 1, 0,
-1, -1, 2, -1, -1, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 3, -1, -6, 2, 7, -2, -4, 0, 1,
0, -1, 0, 0, 0, -1, 0, 1, 0, -1, 1, 0, 0, 1, 1, 1, 0, -1, 0, 0, -1, 0, 2, 1, 0,
0, 0, 1, 0, 0, 0, 1, 0, -1, 2, -3, -1, 0, 0, -2, 5, 1, -1, -1, -6, 0, -1, -1, 3,
3, 2, 1, 0, -2, -3, -2, 0, 1, 0, 0, 0, -1, 1, 0, -3, -2, 7, 2, -4, -1, 2, 0, -1,
-1, 0, 0, 1, 1, 1, -1, -1, 0, 0, -1, -1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, -1,
2, 0, -4, 1, 8, -1, -4, 0, 0, -1, 0, 0, -1, 3, 4, -1, 0, -5, -3, 0, -1, 3, 4, 3,
2, 0, 0, -3, -3, -2, 2, 4, 1, 1, 0, 0, -1, 0, 0, 1, 1, 1, 1, 0, 0, -1, 0, 1, 0,
0, 0, 1, -3, -4, 6, 3, -5, -2, 1, 3, 1, 1, -1, -1, -1, 1, 1, -3, -1, 8, 2, -4,
0, -3, -2, 7, 7, -4, 0, -4, -5, 1, -1, 0, 4, 2, 1, 0, -1, -2, -1, 1, -3, -4, 7,
7, -5, -2, -1, -2, -3, 1, 5, 2, 1, 0, 0, -5, -4, 6, 4, -5, -1, 1, 2, 0, 0, 0, 0,
0, -1, -1, 0, 2, -2, -6, 4, 6, -4, -2, 1, 1, 0, 0, 0, -1, -1, 1, -2, -5, 5, 7,
-4, -2, 0, 2, 1, -4, -5, 4, 5, -4, -3, 1, 2, 0, 1, 0, -2, 0, 0, -2, 3, 1, 3, 4,
-6, -5, -1, 7, -1, 0, 1, 4, 0, 1, -2, 0, -2, -1, -1, 0, 1, 1, 0, -8, 7, 2, 2,
-10, 7, -7, 18, -24, 14, 2, -24, 34, -16, 10, -9, 4, -5, 0, -2, -1, 1, -1, 2,
-2, 2, -4, 4, 0, -1, -4, 9, -3, -7, 3, 5, -3, 5, 1, 3, -5, -3, 0, -1, 3, -1, -3,
4, 7, -4, 2, 0, 2, -3, -6, 0, -1, 1, 3, 4, 4, 2, -4, -4, -3, 3, 3, 1, 0, -1, -1,
-1, -1, 0, 2, 0, 0, -1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, -1, -1, 0, 0, 0, 1,
0, 1, 1, 0, -1, -1, 1, 0, -1, -1, 1, 1, 0, -1, 0, 3, -2, -2, 1, -1, 1, 1, 2, 1,
2, -2, -3, -3, -2, 5, 0, 2, -1, -2, -1, 1, 1, 0, 0, -3, -3, -2, 4, 4, 1, 0, -1,
-2, -1, -1, -1, 0, -1, 1, 1, 0, 1, 0, 1, 0, 0, 0, -1, -1, -1, 0, 0, -1, -1, 0,
1, 0, 0, 0, -1, 1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -2, 2, 1, -2, 0, 0, 0, 1, 0, 0,
0, -1, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 1, -1, 0, 0, 2, 1, 0, -1, -2,
-1, 0, 0, 1, 1, 1, -1, 0, 0, 0, -1, -1, -1, 0, 1, 1, 0, 0, 0, 0, -1, -2, 3, 5,
-6, -8, 5, 1, -1, 0, -2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, -1, 1,
0, 0, -2, 0, -1, 2, 1, 0, -1, -1, -1, 1, 3, 3, -7, -10, 25, -9, -12, 3, -2, 8,
-2, 1, -3, 1, -1, 0, 1, -3, -1, 0, -2, -2, 4, 3, 1, -1, -5, -2, -1, -1, 2, 2, 2,
-1, 0, -1, -1, 0, -1, 0, 0, 0, 0, 0, 2, -1, -1, 1, -1, 2, 1, 0, 0, -2, -1, 2,
-1, 0, 2, 0, -1, -2, 0, 0, 1, 0, 0, 1, 0, -1, -1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1,
0, 0, 0, 1, 0, -1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, -1, -1, 0, -1, -2, 1, 1, 3, 2,
0, -4, -4, -3, 2, 3, 1, 0, 2, 0, -3, -1, 0, 0, -1, 0, 1, 0, 1, -1, -2, 0, 2, -1,
1, 1, 1, 0, 0, -1, -1, 0, 0, 1, 1, 0, 0, -1, 0, 0, 0, 0, 1, -1, 0, 0, 1, 0, 1,
1, 1, -1, 0, -1, -2, 1, -2, 1, -1, 3, -3, 3, -3, 3, -4, 3, -4, 5, -2, 7, 1, -12,
13, -43, 29, 10, 8, 7, -7, 4, -5, 2, -8, 1, -5, 2, -4, 2, -2, 1, -2, 1, -1, -1,
-1, 0, -1, -1, 0, 0, 0, -1, 0, -3, 3, 6, -2, -9, 4, 2, -1, -2, 1, 3, 0, 0, 0, 0,
0, -1, 0, 1, -1, 2, 0, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, 0, 0, 1, -1, -2, 5, 5,
-7, -4, 5, 2, -1, 0, 2, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, -1, 0, 0, 0,
0, 2, 0, -2, 2, 6, -5, -8, 3, 3, -2, 0, 0, 0, 0, 0, 2, 1, 0, 0, -1, -1, 0, -1,
0, 2, 0, 2, 1, 1, -1, -2, 0, 0, 1, -1, 0, 1, 1, 0, 2, -2, 2, -1, -1, 0, -2, 3,
7, -3, -7, 4, 4, -1, -1, -1, 0, 1, 1, -1, 0, 1, -2, -1, 0, -2, 2, 0, 1, 0, 0,
-1, 0, -1, 1, -1, -1, 5, 6, -9, -5, 5, -1, 0, 4, 4, -9, 0, 7, 0, -2, -1, 2, 2,
1, 2, 0, 1, 1, 0, 1, -1, -2, 1, -1, -2, 3, 2, 1, 0, -1, -4, -1, 2, 2, -1, 0, 1,
0, 1, 1, 0, 0, -1, 0, 0, 0, 0, 1, 0, -2, 1, -1, 0, 0, 2, -2, 1, 0, 0, 0, 0, -1,
0, -2, 0, 0, 1, 2, 1, -1, -1, -1, -2, 2, -3, 5, -7, 17, -14, -9, 11, -9, 10, -4,
5, -1, 3, 1, 0, -2, -1, 0, -1, 1, 1, 0, -1, 0, -1, 0, -2, 0, 0, 0, 0, 1, 0, 1,
1, 0, -3, -1, -1, -1, -1, 1, 0, 2, 1, -2, 0, -1, -3, -2, 2, -4, 0, 6, 2, -9, -3,
7, 1, -1, -1, 1, -1, 0, -1, 3, 1, -3, -1, 1, 0, 0, 2, 0, 1, 1, -2, -2, 0, 0, 1,
1, 1, 1, 0, -2, -2, 0, 0, 2, 0, 1, -1, 0, 0, -1, 0, -1, 1, 1, 1, 0, 0, 0, -1,
-1, -1, 0, 0, -2, 1, 1, -6, -3, 5, 4, 4, 3, 2, 0, -1, -2, -2, -3, -1, 3, 2, -1,
0, 2, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1, -1, -1, 0, -1, 0, -1, -1, 0, 0, -1, 0,
-1, -1, 1, 1, 1, -1, 0, 1, 0, 0, 0, 1, 1, 0, -2, -3, -2, 0, 1, -1, -2, 0, 3, 2,
1, 0, 0, 3, 1, -2, -5, -6, -1, 4, 7, 6, 3, -2, -2, -1, -1, -3, -1, 2, 5, 4, 3,
0, -3, -5, -3, -1, -1, 0, 1, 1, 1, 2, 1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 1, 2,
1, 1, 0, 0, 1, -1, 0, 0, 0, 0, 1, 1, 0, 0, -1, 0, -1, -1, -1, 1, 1, 3, 1, -2,
-3, -1, 0, 0, 0, 1, 2, 1, -1, 1, -2, 0, 2, 0, 0, -1, -2, -1, -1, 0, 0, 3, 4, 1,
1, 1, 0, 0, -1, -1, -1, 0, -1, 3, 2, 0, 0, -1, 1, 0, 1, 0, 1, 1, 1, 1, -1, 0, 0,
-1, 0, 0, 1, -1, 0, 0, 1, -1, -2, 0, -1, 1, 2, -2, -2, 2, 1, -1, 0, 2, 1, -1, 0,
0, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, -1, -1, -2, 0, -1, -1, 0, 1, 0, 1, 0, 0, 0,
1, 1, 0, 0, 1, -1, -1, 2, -3, 1, 0, 0, 1, 2, 0, 1, -4, -1, 3, -1, -1, 1, 2, 0,
0, 0, 2, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, -1, 1, -1, -1, -2, 1, 2, 0,
-2, 0, 0, 0, 0, 0, 1, 0, -2, -1, 5, -2, -2, -2, -1, 0, -1, 0, 1, -1, 0, 1, -1,
-1, 3, 0, -1, -1, -1, -1, 0, 1, 0, 2, -1, 0, -1, -1, 0, 0, -1, 0, 0, 2, 0, 1, 0,
-1, 0, 0, -1, -1, 1, 2, 1, -1, -3, 1, 3, -1, -2, 1, -1, 0, 0, -1, 1, -1, 0, 0,
1, -1, -1, -1, -1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, -1, -1, -1, 0, -1, 0, 3, -1,
-3, 1, 2, 0, -1, -1, 0, 1, -1, 0, 1, -3, 0, -1, 0, 0, 1, -1, 1, 1, -2, 2, -1, 3,
0, -1, -1, -1, -1, -2, 1, 2, 1, -2, -1, 1, 0, 0, 0, -1, -3, 2, 3, -2, -1, 1, 1,
1, 1, 0, -1, -3, -1, -1, 2, -1, -1, 1, 0, 0, 0, 0, -2, 0, -1, -1, 0, 0, -1, -1,
0, 1, -1, -2, 2, 2, -2, -1, 1, 1, -2, 0, 3, 1, -3, -1, 0, 0, 1, 0, 0, -1, 0, -2,
-2, 4, 1, -2, -2, 0, 0, 0, 1, 1, 1, -1, -1, -1, 0, -1, 0, -1, -1, 1, 0, 0, 0,
-1, 0, 1, 1, 0, 0, -1, -1, -1, 1, 0, 1, 0, -1, 2, -1, 0, 0, 0, -2, 1, 3, 0, -1,
0, 0, 0, -1, -1, 0, 0, 1, -1, 0, 0, -1, 1, 1, 0, 2, -1, 2, -1, 0, 1, 0, 0, -1,
1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 2, -2, -2, 0, 2, 0, 2, 2, 1, -1, -2, -1,
-1, 2, 1, 2, 0, -1, 0, -1, 0, 0, 0, 1, 0, 0, 2, -1, 0, -1, -1, 0, 0, 0, 2, 2, 0,
0, -3, 2, 0, 0, 0, 0, 0, 0, -1, 1, 1, -1, 0, -1, 0, -1, 0, 1, 0, -1, 0, 0, 0,
-1, -1, -1, 0, -1, 0, 0, 1, 0, -1, -1, 0, 0, -1, 0, 0, 1, 1, 0, 0, 0, 0, 1, -1,
0, -1, 1, 0, 0, 0, 0, -1, 0, 2, 1, 0, -1, 0, -1, -2, 0, 1, 0, -1, 0, 0, 1, 0, 0,
-1, 1, -2, 0, 1, 1, 0, -1, 0, -1, 1, -1, 0, -1, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0,
0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, -1, 2, 1, -1, -1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
0, 0, -1, 1, 1, -1, -1, -1, -1, 0, 0, 0, 1, 1, 0, -1, -1, 0, 0, 0, -1, 0, 0, -1,
0, 0, 0, -1, -1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, -1, 0, -1,
0, 0, -1, 1, 0, -1, 0, 0, 1, 0, 1, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0,
-1, 0, 0, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, -1, 0, -1, 1, 1,
0, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, 0, -1, -1, -1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
-1, 0, -1, -1, 1, 0, 0, -1, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0,
0, 1, 0, 1, 0, 0, -1, -1, 1, 0, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -1,
0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, -1, 1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 1,
0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0,
0, 1, 0, 0, -1, -2, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0,
-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 1, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
-1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, -1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, -1, 0, 0, 0, 1, 0, 0, };

#endif /* s22_H_ */
