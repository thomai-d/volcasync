#include "Display.h"

/* LCD Content: 

	01234567890123456
  0 STATUS            0
  1 CH1: +100         1
	01234567890123456

*/

#define COL_BPM			5
#define ROW_BPM			1

#define COL_CH_ID		0
#define COL_CH_VALUE	5
#define ROW_CH_VALUE	1

/* Public */

void Display::init()
{
	lcd.begin(16, 2);
	lcd.init();
	lcd.setBacklight(1);
	drawSkeleton();
}

void Display::setStatus(const char* status)
{
	uint8_t len = strlen(status);

	lcd.setCursor(0, 0);
	lcd.print(status);

	for (uint8_t n = 0; n < this->status_len - len; n++)
		lcd.print("x");

	this->status_len = len;
}

void Display::setBpm(int bpm)
{
	lcd.setCursor(0, 1);
	lcd.print("BPM: ");
	print_uint8(bpm);
}

void Display::drawChannelValue(uint8_t channel, int value)
{
	lcd.setCursor(COL_CH_ID, ROW_CH_VALUE);
	lcd.print("CH");
	lcd.print(channel);
	lcd.print(": ");

	int display = value - 127;
	if (display > 0)
		lcd.print("+");
	if (display < 0)
		lcd.print("-");

	lcd.print(abs(display), 10);
	lcd.print("   ");
}

void Display::clearSecondRow()
{
	lcd.setCursor(0, 1);
	lcd.print("                ");
}

/* Helper methods */

void Display::drawSkeleton()
{
	lcd.print("BPM: ");
}

void Display::print_uint8(uint8_t value)
{
	uint8_t nFill = 0;
	if (value < 10) nFill = 2;
	else if (value < 100) nFill = 1;
	for (uint8_t n = 0; n < nFill; n++)
		lcd.print(" ");
	lcd.print(value, 10);
}

Display LCD;
