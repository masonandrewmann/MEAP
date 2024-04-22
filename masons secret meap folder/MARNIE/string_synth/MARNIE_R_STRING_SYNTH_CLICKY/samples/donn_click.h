#ifndef donn_click_H_
#define donn_click_H_
 
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "mozzi_pgmspace.h"
 
#define donn_click_NUM_CELLS 106
#define donn_click_SAMPLERATE 32768
 
CONSTTABLE_STORAGE(int8_t) donn_click_DATA [] = {11, -5, -6, 17, -25, 51, -77,
6, 15, -20, 16, -4, -7, 38, -12, 10, 3, -17, 26, -31, 31, -24, 8, 17, -21, 14,
-5, -6, 13, -20, 17, -9, -10, 16, -17, 12, -7, -2, 7, -11, 6, 4, -10, 10, -9, 3,
2, -8, 9, -8, -2, 5, -8, 5, -5, 0, 3, -7, 4, -1, -3, 3, -4, 1, -1, -4, 5, -4, 0,
1, -4, 2, -2, -1, 3, -6, 3, -2, -2, 2, -4, 2, -2, -2, 3, -3, 0, -1, -2, 1, -2,
1, -1, -1, 0, -1, -1, 0, -2, 1, -1, -1, 0, -1, 0, -1, };

#endif /* donn_click_H_ */
