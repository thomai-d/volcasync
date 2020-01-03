#pragma once

#include <EEPROM.h>

#define STORAGE_MAGIC		0x30

#define ADDR_MAGIC			0
#define ADDR_BPM_H			1
#define ADDR_BPM_L			2

#define ADDR_TRIG_START		8

bool storage_isInitialized()
{
	return EEPROM.read(ADDR_MAGIC) == STORAGE_MAGIC;
}

void storage_init()
{
	EEPROM.update(ADDR_MAGIC, STORAGE_MAGIC);
}

void storage_setBpm(int bpm)
{
	EEPROM.update(ADDR_BPM_H, bpm >> 8);
	EEPROM.update(ADDR_BPM_L, bpm & 0xff);
}

void storage_setChannelTrigger(uint8_t chan, uint8_t trigger)
{
	EEPROM.update(ADDR_TRIG_START + chan, trigger);
}

int storage_getBpm()
{
	return ((int)EEPROM.read(ADDR_BPM_H) << 8)
			   + EEPROM.read(ADDR_BPM_L);
}

uint8_t storage_getTrigger(uint8_t chan)
{
	return EEPROM.read(ADDR_TRIG_START + chan);
}
