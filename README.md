# Korg Volca Sync 

Simple arduino based module for synthesizer/sequencer synchronization.

![Finished module](https://raw.githubusercontent.com/thomai-d/volcasync/master/doc/finished.png)

### Features

- 4 synchronization channels (easily extendable by `CHANNEL_COUNT`).
- every channel can be delayed in 256 steps up to +/- 1/16th note (positive and negative delay possible).
- every channel supports half-step mode (only pulses every second sync step).
- song tempo configuration (in BPM).
- song position display.
- global start/stop.
- settings are persisted in EEPROM.
- changing the channel delay will slowly change the delay (immediate changes confuse some devices).

### Why?

I've had a lot of problems syncing my devices. No matter which device is set to master, nothing really worked 100%.

- Volcas have huge problems with MIDI sync
- Volcas emit a single pulse upon startup (=> device chain is out of sync)
- Arturia Keystep stops forwarding syncs when sequencing is stopped
- Monologue emits an additional sync when play is pressed (WTF?)
- Sequenced synths with (relative) long attack times sound off-sync with the kick-drum. A negative sync delay would fix this issue.

### What do I need to build this?

- ATMEGA328 + Crystal
- PCF8574 I2C LCD Controller
- 16x2 LCD Display
- Some buttons
- A rotary encoder

![Module explanation](https://raw.githubusercontent.com/thomai-d/volcasync/master/doc/pcbonly.png)

![Assembled module](https://raw.githubusercontent.com/thomai-d/volcasync/master/doc/assembled.png)

### How does the sync pulse look like?

The sync pulse is just a small pulse every 1/8th note.

![Sync pulse](https://raw.githubusercontent.com/thomai-d/volcasync/master/doc/signal-equal.png)

Shifting the second channel will look like this:

![Sync pulse predelayed](https://raw.githubusercontent.com/thomai-d/volcasync/master/doc/signal-shiftmax.png)

### Ok, how does it look in action?

![Totally oversized gif](https://raw.githubusercontent.com/thomai-d/volcasync/master/doc/live.gif)
