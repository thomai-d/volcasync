#pragma once
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include "Configuration.h"

class Display
{
public:
	void init();

	void setBpm(int bpm);

	void drawChannelValue(uint8_t channel, int value);
	void clearSecondRow();

private:

	void drawSkeleton();
	void print_uint8(uint8_t x, uint8_t y, uint8_t value);

	LiquidCrystal_PCF8574  lcd = LiquidCrystal_PCF8574(0x27);
	int bpm;

};

extern Display LCD;
