/* 
	Hardware abstraction boilerplate code.
	======================================

	- Pin initialization
	- Rotary event handling
	- Button polling

*/
#pragma once

extern void onRotaryChanged_ISR(int8_t offset);
extern void onRotaryButtonPressed();
extern void onStartStopPressed();
extern void onSelectButtonPressed();

#define PIN_BTN_STARTSTOP	17			// Start/Stop-Button
#define PIN_BTN_SELECT		16			// Select-Button
#define PIN_BTN_ALT1		15			// unused
#define PIN_BTN_ALT2		14			// unused
#define PIN_BTN_ROTARY		4			// Rotary button
#define PIN_ROTARY_A		2			// Rotary A (must support interrupt)
#define PIN_ROTARY_B		3			// Rotary B (must support interrupt)
#define PIN_LED				11			// LED
#define PIN_CH_0			8			// Channel 1 sync
#define PIN_CH_1			7			// Channel 2 sync
#define PIN_CH_2			6			// Channel 3 sync
#define PIN_CH_3			5			// Channel 4 sync

uint8_t rotary_state;					// Last rotary state
bool lastRotWasDown = false;			// Last rotary button state
bool lastStartStopWasDown = false;		// Last start/stop button state
bool lastSelectAWasDown = false;		// Last select button state

void on_rotary_changed_ISR();

void initializePins()
{
#ifdef LED_ENABLED
	pinMode(PIN_LED, OUTPUT);
	analogWrite(PIN_LED, 0);
#endif

	pinMode(PIN_ROTARY_A, INPUT_PULLUP);
	pinMode(PIN_ROTARY_B, INPUT_PULLUP);
	pinMode(PIN_BTN_ROTARY, INPUT_PULLUP);
	pinMode(PIN_BTN_SELECT, INPUT_PULLUP);
	pinMode(PIN_BTN_STARTSTOP, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_A), on_rotary_changed_ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_ROTARY_B), on_rotary_changed_ISR, CHANGE);
}

/* Is invoked if the rotary is moved. */
void on_rotary_changed_ISR()
{
	uint8_t currentState = digitalRead(PIN_ROTARY_A) * 2
						 + digitalRead(PIN_ROTARY_B);

	rotary_state = rotary_lookup[rotary_state & 0xf][currentState];
	int8_t offset = rotary_state == DIR_CW ? 1
				  : rotary_state == DIR_CCW ? -1
				  : 0;
	
	if (offset != 0)
		onRotaryChanged_ISR(offset);
}

void hardware_pollButtons()
{
	bool isDown = !digitalRead(PIN_BTN_ROTARY);
	if (isDown && !lastRotWasDown)
		onRotaryButtonPressed();
	lastRotWasDown = isDown;

	isDown = !digitalRead(PIN_BTN_STARTSTOP);
	if (isDown && !lastStartStopWasDown)
		onStartStopPressed();
	lastStartStopWasDown = isDown;

	isDown = !digitalRead(PIN_BTN_SELECT);
	if (isDown && !lastSelectAWasDown)
		onSelectButtonPressed();
	lastSelectAWasDown = isDown;
}

