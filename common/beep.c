#include "common.h"
#include "beep.h"
#include "timer.h"

void beep_init(void)
{
	BEEP_INIT;
}

void beep_ms(uint16_t delay)
{
	BEEP_ON;
	_delay_ms(delay);
	BEEP_OFF;
}

void beep_timer_ms(uint16_t delay)
{
	uint8_t	sreg;
	sreg = SREG;
	GLOBAL_INT_DISABLE;
	BEEP_ON;
	if ((timers[1] < delay) || (0xFFFF == timers[1]))
		timers[1] = delay;
	SREG = sreg;
}
