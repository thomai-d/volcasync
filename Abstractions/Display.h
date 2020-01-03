#pragma once
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

class Display
{
public:
	void init();

	void setStatus(const char* status);
	void setBeat(uint8_t beat, uint8_t quarter);

	void setBpm(int bpm);

	void drawChannelValue(uint8_t channel, int value);
	void clearSecondRow();

private:

	void drawSkeleton();
	void print_uint8(uint8_t value);

	LiquidCrystal_PCF8574  lcd = LiquidCrystal_PCF8574(0x27);
	int bpm;

	/* length of status line for lcd clearing */
	uint8_t status_len = 0;

};

extern Display LCD;
