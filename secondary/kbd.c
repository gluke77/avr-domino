#include <avr\io.h>
#include "../common/common.h"
#include "../common/beep.h"
#include "kbd.h"
#include <stdio.h>

uint8_t		kbd_timeout;

uint16_t	key_pressed_counter[KEY_COUNT];

uint16_t	key_big_delay = 50;
uint16_t	key_small_delay = 5;

volatile uint8_t	key_pressed[KBD_BANK_COUNT];
volatile uint8_t	key_state[KBD_BANK_COUNT];
volatile uint8_t	old_key_state[KBD_BANK_COUNT];
//volatile uint8_t	key_released[KBD_BANK_COUNT];

void kbd_init(void)
{
	uint8_t	idx;
	
	for (idx = 0; idx < KBD_BANK_COUNT; idx++)
	{
		key_pressed[idx] = 0x00;
		key_state[idx] = 0xFF;
		old_key_state[idx] = 0xFF;
	}
	
	for (idx = 0; idx < KEY_COUNT; idx++)
		key_pressed_counter[idx] = 0;
	
	KBD_PORT_DDR = 0;
	SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT0);
	SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT1);
	SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT2);
	SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT3);
}

void do_kbd(void)
{
	if (!kbd_timeout--)
	{
		kbd_timeout = KBD_TIMEOUT;
		kbd_scan();
	}
}

void kbd_select(uint8_t bank)
{
	if (bank == 0)
		CLEARBIT(KBD_SCAN_PORT, KBD_SCAN_BIT0);
	else
		SETBIT(KBD_SCAN_PORT, KBD_SCAN_BIT0);
		
	if (bank == 1)
		CLEARBIT(KBD_SCAN_PORT, KBD_SCAN_BIT1);
	else
		SETBIT(KBD_SCAN_PORT, KBD_SCAN_BIT1);
		
	if (bank == 2)
		CLEARBIT(KBD_SCAN_PORT, KBD_SCAN_BIT2);
	else
		SETBIT(KBD_SCAN_PORT, KBD_SCAN_BIT2);

	if (bank == 3)
		CLEARBIT(KBD_SCAN_PORT, KBD_SCAN_BIT3);
	else
		SETBIT(KBD_SCAN_PORT, KBD_SCAN_BIT3);
}

void kbd_clear(void)
{
	uint8_t	idx;
	for (idx = 0; idx < KBD_BANK_COUNT; idx++)
		key_pressed[idx] = 0x00;
}

uint8_t current_key_state(uint8_t key_id)
{
	uint8_t		kbd_value;
	kbd_value = KBD_PORT;
	
	return TESTBIT(kbd_value, (((key_id) & 0x03) + 4));
}

void one_bank_scan(uint8_t bank_id)
{
	uint8_t 	kbd_value[KBD_REPEAT_COUNT];
	uint8_t		idx;
	uint8_t		count;
	uint8_t		key_id;
	
	for (idx = 0; idx < KBD_REPEAT_COUNT; idx++)
		kbd_value[idx] = KBD_PORT;
	
	old_key_state[bank_id] = key_state[bank_id];

	for (key_id = 4; key_id < 8; key_id++)
	{
		for (count = 0, idx = 0; idx < KBD_REPEAT_COUNT; idx++)
			if (TESTBIT(kbd_value[idx], key_id))
				count++;
		
		if (count > 7)
			SETBIT(key_state[bank_id], key_id);
		else if (count < 3)
			CLEARBIT(key_state[bank_id], key_id);
	}
}

void one_key_scan(uint8_t key_id)
{
	if (KEY_STATE(key_id) && (!OLD_KEY_STATE(key_id)))  // key released
	{
		SET_KEY_PRESSED(key_id);
		key_pressed_counter[key_id] = 0;
	}
	else if ((!KEY_STATE(key_id)) && OLD_KEY_STATE(key_id))
	{
		// key pressed
//		beep_timer_ms(50);
	}
	else if (!KEY_STATE(key_id) && !OLD_KEY_STATE(key_id)) // key still pressed
	{
		key_pressed_counter[key_id]++;
		
		if (key_small_delay + key_big_delay < key_pressed_counter[key_id]) 
		{
			SET_KEY_PRESSED(key_id);
			key_pressed_counter[key_id] = key_big_delay;
		}
	}
}

void kbd_scan(void)
{
	uint8_t		bank_id;
	uint8_t		key_id;

	for (bank_id = 0; bank_id < KBD_BANK_COUNT; bank_id++)
	{
		kbd_select(bank_id);
		_delay_us(5);
		one_bank_scan(bank_id);
	}

	for (key_id = 0; key_id < KEY_COUNT; key_id++)
		one_key_scan(key_id);
}
