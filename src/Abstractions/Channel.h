/*
	Channel data structures.	
*/
#pragma once

// Encapsulates data for a single sync channel.
typedef struct Channel
{
	uint8_t		trigger		= 127;			// step when the channel is triggered.
	uint8_t		setValue	= 127;			// new trigger value (set by user).
	bool		halfStep	= false;		// if set, only every second pulse is emitted.
	uint8_t		pin;						// channel's pin.

	uint8_t		pulseStepsLeft	= 0;		// Number of steps the pulse should be kept on.
};

