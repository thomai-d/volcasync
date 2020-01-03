#pragma once

/* Increments a value and constains min/max. */
int checked_add(int value, int8_t increment, int min, int max)
{
	int newValue = value + increment;
	if (newValue > max)
		return max;
	else if (newValue < min)
		return min;

	return newValue;
}
