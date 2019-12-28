#ifndef ROTARY_H
#define ROTARY_H

// Usage:
//	rotary_state = rotary_lookup[rotary_state & 0xf][currentValue & 0x3];
//  if (rotary_state == DIR_CW || rotary_state == DIR_CCW) ...

#define R_START 0x0
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6
#define DIR_NONE	0x0
#define DIR_CW		0x10
#define DIR_CCW		0x20

const uint8_t rotary_lookup[7][4] = {
	{ R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START },
	{ R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW },
	{ R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START },
	{ R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START },
	{ R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START },
	{ R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW },
	{ R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START }
};

#endif
