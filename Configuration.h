#pragma once

#define CHANNEL_COUNT		2			// Number of sync channels.

#define DELAY_APPROX_STEPS	15			// Some machines get out of sync if the sync signal changes too fast.
										// Set this to lower values to prevent out-of-sync-issues.

#define DELAY_STEPS_MOD		5			// Number of steps to skip with one rotary step.
