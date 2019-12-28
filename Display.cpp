#include "Display.h"

/* LCD Content: 

	01234567890123456
  0 BPM: XXX          0
  1                   1
	01234567890123456

*/

#define COL_BPM 5
#define ROW_BPM 0

/* Public */

void Display::init()
{
	lcd.begin(16, 2);
	lcd.init();
	lcd.setBacklight(1);
	drawSkeleton();
}

void Display::setBpm(int bpm)
{
	print_uint8(COL_BPM, ROW_BPM, bpm);
}

/* Helper methods */

void Display::drawSkeleton()
{
	lcd.print("BPM: ");
}

void Display::print_uint8(uint8_t x, uint8_t y, uint8_t value)
{
	uint8_t nFill = 0;
	lcd.setCursor(x, y);
	if (value < 10) nFill = 2;
	else if (value < 100) nFill = 1;
	for (uint8_t n = 0; n < nFill; n++)
		lcd.print(" ");
	lcd.print(value, 10);
}

Display LCD;
