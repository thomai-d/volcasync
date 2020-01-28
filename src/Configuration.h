#pragma once

#define CHANNEL_COUNT			4			// Number of sync channels.

#define DELAY_APPROX_STEPS		15			// Some machines get out of sync if the sync signal changes too fast.
											// Set this to lower values to prevent out-of-sync-issues.

#define DELAY_STEPS_MOD			5			// Number of steps to skip with one rotary step.
#define STEPS_DISPLAY_OFFSET	3			// A pulse (uint8)127 is the reference sync.
											// To display 0-255 as -127 to 128 and apply rastering the display
											// in steps of 5, an offset is needed for rasterization.

// #define LED_ENABLED			true		// If set, the LED flashes every 1/4 note at reference sync time.
