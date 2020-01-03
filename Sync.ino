#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <TimerOne.h>

#include "Configuration.h"
#include "Abstractions/Rotary.h"
#include "Abstractions/Display.h"
#include "Abstractions/Helpers.h"

#define PIN_STARTSTOP	17			// Start/Stop-Button
#define PIN_SELECT_A	16			// Modifier for channel A
#define PIN_GPIO15		15			// unused
#define PIN_GPIO14		14			// unused
#define PIN_ROTARY_A	2			// Use interrupt pins!!
#define PIN_ROTARY_B	3			// Use interrupt pins!!
#define PIN_ROTARY_X	4			// Rotary button
#define PIN_LED			11			// LED

#define PIN_CH_0		5	
#define PIN_CH_1		6
#define PIN_CH_2		7
#define PIN_CH_3		8

// Hardware stuff.
uint8_t rotary_state;
volatile uint8_t selectedChannel;
bool lastRotWasDown = false;

// BL stuff.
volatile int bpm = 125;
volatile bool bpmChanged = true;

uint8_t step = 0;
uint8_t beat = 0; // 8th beat count

bool isRunning = false;				// Start/Stop.
bool isRedrawRequired = true;
bool lastStartStopWasDown = false;
bool lastSelectAWasDown = false;

#define BPM_TO_TIMER_US		117187  // :)
typedef struct Channel
{
	uint8_t		trigger		= 127;
	uint8_t		setValue	= 127;
	bool		skipOnce	= false;
	uint8_t		pin;
};

volatile Channel channels[CHANNEL_COUNT];

void setup()
{
	channels[0].pin = PIN_CH_0;
	channels[1].pin = PIN_CH_1;
	channels[2].pin = PIN_CH_2;
	channels[3].pin = PIN_CH_3;

	for (uint8_t n = 0; n < CHANNEL_COUNT; n++)
		pinMode(channels[n].pin, OUTPUT);

	pinMode(PIN_LED, OUTPUT);
	analogWrite(PIN_LED, 0);

	pinMode(PIN_ROTARY_A, INPUT_PULLUP);
	pinMode(PIN_ROTARY_B, INPUT_PULLUP);
	pinMode(PIN_ROTARY_X, INPUT_PULLUP);
	pinMode(PIN_SELECT_A, INPUT_PULLUP);
	pinMode(PIN_STARTSTOP, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_A), on_buttonsChanged_ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_B), on_buttonsChanged_ISR, CHANGE);

	LCD.init();
	LCD.setStatus("Stopped");

	Timer1.stop();
	Timer1.attachInterrupt(on_clock_ISR);
	Serial.begin(9600);
}

void loop()
{
	delay(10);

	checkSelectButton();

	checkRotaryPressed();

	checkStartStop();

	redraw();

	if (bpmChanged && isRunning)
	{
		bpmChanged = false;
		Timer1.initialize(BPM_TO_TIMER_US / bpm);
	}
}

void redraw()
{
	if (!isRedrawRequired)
		return;

	if (selectedChannel == 0)
		LCD.setBpm(bpm);
	else
		LCD.drawChannelValue(selectedChannel, channels[selectedChannel-1].setValue);
}

void checkStartStop()
{
	bool isPressed = !digitalRead(PIN_STARTSTOP);

	if (isPressed && !lastStartStopWasDown)
	{
		isRunning = !isRunning;
		if (isRunning)
		{
			LCD.setStatus("Running");
			Timer1.restart();
		}
		else
		{
			LCD.setStatus("Stopped");
			Timer1.stop();
		}
	}

	lastStartStopWasDown = isPressed;
}

void checkRotaryPressed()
{
	bool isDown = !digitalRead(PIN_ROTARY_X);
	if (isDown && !lastRotWasDown)
	{
		if (selectedChannel > 0)
		{
			channels[selectedChannel - 1].skipOnce = true;
		}
	}

	lastRotWasDown = isDown;
}

void checkSelectButton()
{
	bool isDown = !digitalRead(PIN_SELECT_A);
	uint8_t newSel = selectedChannel;
	if (isDown && !lastSelectAWasDown)
	{
		newSel = ((newSel + 1) % (CHANNEL_COUNT + 1));
	}
	lastSelectAWasDown = isDown;
	
	isRedrawRequired = newSel == selectedChannel;
	selectedChannel = newSel;
}

/* Event handlers */

/* Is invoked if the rotary is moved. */
void on_buttonsChanged_ISR()
{
	uint8_t currentState = digitalRead(PIN_ROTARY_A) * 2
						 + digitalRead(PIN_ROTARY_B);

	rotary_state = rotary_lookup[rotary_state & 0xf][currentState];
	int8_t offset = rotary_state == DIR_CW ? 1
				  : rotary_state == DIR_CCW ? -1
				  : 0;

	if (selectedChannel > 0)
	{
		channels[selectedChannel-1].setValue = checked_add(channels[selectedChannel-1].setValue, offset * DELAY_STEPS_MOD, 5, 250);
	}
	else
	{
		bpm = checked_add(bpm, offset, 60, 300);
		bpmChanged = true;
	}

	isRedrawRequired = true;
}

/* Is invoked 256 times within a 1/8 beat. */
void on_clock_ISR()
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
		bool trigger = channels[i].trigger == step;
		if (trigger && channels[i].skipOnce)
		{
			channels[i].skipOnce = false;
			trigger = false;
		}
		digitalWrite(channels[i].pin, trigger);
	}

	// Flash LED every two beats (1/4).
	if (beat % 2 == 0 && step < 128)
		analogWrite(PIN_LED, 255 - step * 2);
	else
		analogWrite(PIN_LED, 0);

	step++;
	if (step == 0)
		beat++;
}

