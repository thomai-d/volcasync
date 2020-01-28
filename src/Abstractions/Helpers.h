/* 
	Small helper methods 

*/
#pragma once

/* Increments a value and constains min/max. */
int check_increment(int value, int8_t increment, int min, int max, int align, int preAlignOffset)
{
	int newValue = value + increment;
	if (newValue > max)
		return max;
	else if (newValue < min)
		return min;

	newValue += preAlignOffset;
	newValue = newValue > 0
		? newValue - newValue % align
		: newValue + newValue % align;
	newValue -= preAlignOffset;

	return newValue;
}
