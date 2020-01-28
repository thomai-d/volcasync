#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <TimerOne.h>
#include <eeprom.h>
#include <util/atomic.h>

#include "Configuration.h"
#include "Abstractions/Rotary.h"
#include "Abstractions/Display.h"
#include "Abstractions/Helpers.h"
#include "Abstractions/Storage.h"
#include "Abstractions/Hardware.h"
#include "Abstractions/Channel.h"

volatile uint8_t selectedChannel;	// current channel selection (0 = BPM, 1 = CH1, 2 = CH2, ...)
volatile bool isRunning = false;	// current run/stop-state.
volatile int bpm = 125;				// current BPM setting.
volatile bool bpmChanged = true;	// indicates the BPM changed during an ISR.
volatile uint8_t step = 0;			// current step (1/256 of an 8th beat).
volatile uint8_t beat = 0;			// 8th beat count since start.
#define BPM_TO_TIMER_US(ARG_BPM)	(117187/ARG_BPM) // short form of (1M uS * 60) / (BPM * 256steps * 2)

bool is1stRowRedrawRequired = true;
volatile bool is2ndRowRedrawRequired = true;

volatile Channel channels[CHANNEL_COUNT];

void setup()
{
	LCD.init();

	initializeStorage();
	initializeChannels();
	initializePins();

	Timer1.stop();
	Timer1.attachInterrupt(onClock_ISR);

	delay(2000);
	LCD.go();
}

void loop()
{
	delay(10);

	hardware_pollButtons();

	redraw();

	updateTimer();
}

void redraw()
{
	if (is1stRowRedrawRequired)
	{
		redraw_firstRow();
		is1stRowRedrawRequired = false;
	}

	if (is2ndRowRedrawRequired)
	{
		redraw_secondRow();
		is2ndRowRedrawRequired = false;
	}
}

void redraw_firstRow()
{
	if (isRunning)
	{
		uint8_t quartOutOfFour = (beat / 2) % 4;
		uint8_t beatOutOfFour = (beat / 8) % 4;
		LCD.setBeat(beatOutOfFour, quartOutOfFour);
	}
	else
	{
		LCD.setStatus("Stopped");

		// Save on stop.
		storage_setBpm(bpm);
		for (uint8_t c = 0; c < CHANNEL_COUNT; c++)
			storage_setChannelTrigger(c, channels[c].trigger);

		beat = 0;
		step = 0;
	}
}

void redraw_secondRow()
{
	if (selectedChannel == 0)
		LCD.setBpm(bpm);
	else
	{
		bool isHalfStep = channels[selectedChannel - 1].halfStep;
		LCD.drawChannelValue(selectedChannel, channels[selectedChannel - 1].setValue, isHalfStep ? 'H' : ' ');
	}
}

void updateTimer()
{
	if (bpmChanged && isRunning)
	{
		int currentBpm;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			currentBpm = bpm;
		}

		bpmChanged = false;
		Timer1.initialize(BPM_TO_TIMER_US(currentBpm));
	}
}

/*********************/
/*  EVENT HANDLERS   */
/*********************/

void onRotaryChanged_ISR(int8_t offset)
{
	if (selectedChannel > 0)
	{
		channels[selectedChannel - 1].setValue = check_increment(channels[selectedChannel - 1].setValue, offset * DELAY_STEPS_MOD, 0, 255, DELAY_STEPS_MOD, STEPS_DISPLAY_OFFSET);
	}
	else
	{
		int currentBpm;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			currentBpm = bpm;
		}

		currentBpm = check_increment(currentBpm, offset, 60, 300, 1, 0);
		
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			bpm = currentBpm;
		}

		bpmChanged = true;
	}

	is2ndRowRedrawRequired = true;
}

void onRotaryButtonPressed()
{
	if (selectedChannel > 0)
	{
		channels[selectedChannel - 1].halfStep = !channels[selectedChannel - 1].halfStep;
		is2ndRowRedrawRequired = true;
	}
}

void onStartStopPressed()
{
	isRunning = !isRunning;
	if (isRunning)
		Timer1.restart();
	else
		Timer1.stop();

	is1stRowRedrawRequired = true;
}

void onSelectButtonPressed()
{
	selectedChannel = ((selectedChannel + 1) % (CHANNEL_COUNT + 1));
	is2ndRowRedrawRequired = true;
}

// Is invoked 256 times within a 1/8 beat.
void onClock_ISR()
{
	if (!isRunning)
		return;

	for (uint8_t i = 0; i < CHANNEL_COUNT; i++)
	{
		// Move setpoints toward trigger step.
		if (step == 0 && channels[i].setValue != channels[i].trigger)
		{
			if (channels[i].setValue > channels[i].trigger + DELAY_APPROX_STEPS)
				channels[i].trigger += DELAY_APPROX_STEPS;
			else if (channels[i].setValue < channels[i].trigger - DELAY_APPROX_STEPS)
				channels[i].trigger -= DELAY_APPROX_STEPS;
			else
				channels[i].trigger = channels[i].setValue;
		}

		// Trigger pulses.
		bool isHalfStep = beat % 2 == 0;
		bool isTrigger = channels[i].trigger == step
			&& !(channels[i].halfStep && isHalfStep);

		if (isTrigger)
			channels[i].pulseStepsLeft = PULSE_LENGTH;

		if (channels[i].pulseStepsLeft > 0)
		{
			digitalWrite(channels[i].pin, HIGH);
			channels[i].pulseStepsLeft--;
		}
		else
		{
			digitalWrite(channels[i].pin, LOW);
		}
	}

#ifdef LED_ENABLED
	// Flash LED every two beats (1/4).
	if (beat % 2 == 0 && step > 128)
		analogWrite(PIN_LED, 255 - step * 2);
	else
		analogWrite(PIN_LED, 0);
#endif

	step++;
	if (step == 0)
	{
		beat++;
		is1stRowRedrawRequired = true;
	}
}


/*********************/
/*  INITIALIZATION   */
/*********************/

// Ensures EEPROM is initialized.
void initializeStorage()
{
	if (storage_isInitialized())
		return;

	for (uint8_t n = 0; n < CHANNEL_COUNT; n++)
		storage_setChannelTrigger(n, 127);

	storage_setBpm(bpm);
	storage_init();
}

// Sets channel pins and reads channel values from EEPROM.
void initializeChannels()
{
	channels[0].pin = PIN_CH_0;
	channels[1].pin = PIN_CH_1;
	channels[2].pin = PIN_CH_2;
	channels[3].pin = PIN_CH_3;

	for (uint8_t n = 0; n < CHANNEL_COUNT; n++)
	{
		pinMode(channels[n].pin, OUTPUT);
		channels[n].trigger = storage_getTrigger(n);
		channels[n].setValue = channels[n].trigger;
	}
}
