/*
	Channel data structures.	
*/
#pragma once

// Encapsulates data for a single sync channel.
typedef struct Channel
{
	uint8_t		trigger		= 127;			// step when the channel is triggered.
	uint8_t		setValue	= 127;			// new trigger value (set by user).
	bool		skipOnce	= false;		// if set, a single pulse should be skipped.
	uint8_t		pin;						// channel's pin.
};

