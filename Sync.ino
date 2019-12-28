#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <TimerOne.h>

#include "Rotary.h"

#include "Display.h"

#define PIN_ROTARY_A	2			// Use interrupt pins!!
#define PIN_ROTARY_B	3			// Use interrupt pins!!
#define PIN_SELECT_A	8			// Modifier for channel A


// Hardware stuff.
uint8_t rotary_state;

// BL stuff.
#define BPM_TO_TIMER_US		117187    // :)
volatile int bpm = 125;
volatile bool bpmChanged = true;

void setup()
{
	pinMode(22, OUTPUT);
	pinMode(23, OUTPUT);

	pinMode(PIN_ROTARY_A, INPUT_PULLUP);
	pinMode(PIN_ROTARY_B, INPUT_PULLUP);
	pinMode(PIN_SELECT_A, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_A), on_buttonsChanged_ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_B), on_buttonsChanged_ISR, CHANGE);

	LCD.init();

	Timer1.attachInterrupt(onClock);
	Serial.begin(9600);

}

uint8_t newT2 = 127;
bool t2Changed = false;


void on_buttonsChanged_ISR()
{
	uint8_t currentState = digitalRead(PIN_ROTARY_A) * 2
						 + digitalRead(PIN_ROTARY_B);

	bool isChannelA = !digitalRead(PIN_SELECT_A);

	rotary_state = rotary_lookup[rotary_state & 0xf][currentState];
	int8_t offset = rotary_state == DIR_CW ? 1
				  : rotary_state == DIR_CCW ? -1
				  : 0;

	if (isChannelA)
	{
		newT2 += offset;
		t2Changed = true;
	}
	else
	{
		bpm += offset;
		bpmChanged = true;
	}
}

uint8_t i = 0;

uint8_t t1 = 127;
uint8_t t2 = 127;

const int adjustStep = 15;

void onClock()
{
	digitalWrite(22, i == t1);
	digitalWrite(23, i == t2);

	if (i == 0)
	{
		if (newT2 > t2 + adjustStep)
			t2 += adjustStep;
		else if (newT2 < t2 - adjustStep)
			t2 -= adjustStep;
		else
			t2 = newT2;
	}

	i++;
}

void loop()
{
	delay(100);

	if (bpmChanged)
	{
		bpmChanged = false;
		Timer1.initialize(BPM_TO_TIMER_US / bpm);
		LCD.setBpm(bpm);
	}
}

