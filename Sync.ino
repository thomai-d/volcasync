#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <TimerOne.h>

#include "Configuration.h"
#include "Rotary.h"
#include "Display.h"

#define PIN_ROTARY_A	2			// Use interrupt pins!!
#define PIN_ROTARY_B	3			// Use interrupt pins!!
#define PIN_ROTARY_X	7			// Rotary button
#define PIN_SELECT_A	8			// Modifier for channel A

#define PIN_CH_0		22
#define PIN_CH_1		23

// Hardware stuff.
uint8_t rotary_state;
volatile uint8_t selectedChannel;
bool lastRotWasDown = false;

// BL stuff.
volatile int bpm = 125;
volatile bool bpmChanged = true;

uint8_t step = 0;

#define BPM_TO_TIMER_US		117187    // :)
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

	pinMode(PIN_CH_0, OUTPUT);
	pinMode(PIN_CH_1, OUTPUT);

	pinMode(PIN_ROTARY_A, INPUT_PULLUP);
	pinMode(PIN_ROTARY_B, INPUT_PULLUP);
	pinMode(PIN_ROTARY_X, INPUT_PULLUP);
	pinMode(PIN_SELECT_A, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_A), on_buttonsChanged_ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_B), on_buttonsChanged_ISR, CHANGE);

	LCD.init();

	Timer1.attachInterrupt(onClock);
	Serial.begin(9600);

}

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
}

void onClock()
{
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++)
	{
		bool trigger = channels[i].trigger == step;
		if (trigger && channels[i].skipOnce)
		{
			channels[i].skipOnce = false;
			trigger = false;
		}

		digitalWrite(channels[i].pin, trigger);

		if (step == 0 && channels[i].setValue != channels[i].trigger)
		{
			if (channels[i].setValue > channels[i].trigger + DELAY_APPROX_STEPS)
				channels[i].trigger += DELAY_APPROX_STEPS;
			else if (channels[i].setValue < channels[i].trigger - DELAY_APPROX_STEPS)
				channels[i].trigger -= DELAY_APPROX_STEPS;
			else
				channels[i].trigger = channels[i].setValue;
		}
	}

	step++;
}

void loop()
{
	delay(10);

	updateSelectedChannel();

	checkRotaryPressed();

	if (bpmChanged)
	{
		bpmChanged = false;
		Timer1.initialize(BPM_TO_TIMER_US / bpm);
		LCD.setBpm(bpm);
	}
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

void updateSelectedChannel()
{
	uint8_t newChSel = 0;
	if (!digitalRead(PIN_SELECT_A))
	{
		newChSel = 1;
	}
	selectedChannel = newChSel;
	
	if (selectedChannel == 0)
		LCD.clearSecondRow();
	else
		LCD.drawChannelValue(selectedChannel, channels[selectedChannel-1].setValue);
}

int checked_add(int value, int8_t increment, int min, int max)
{
	int newValue = value + increment;
	if (newValue > max)
		return max;
	else if (newValue < min)
		return min;

	return newValue;
}
