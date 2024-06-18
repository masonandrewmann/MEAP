#ifndef click_H_
#define click_H_
 
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "mozzi_pgmspace.h"
 
#define click_NUM_CELLS 5
#define click_SAMPLERATE 16384
 
CONSTTABLE_STORAGE(int8_t) click_DATA [] = {58, 48, 1, -88, -104, };

#endif /* click_H_ */
