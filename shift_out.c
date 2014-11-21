#include <avr\io.h>
#include "common.h"
#include "shift_out.h"

uint8_t	controls;
uint8_t old_controls;

void	shift_one(uint8_t data)
{
	int	i;
	for (i = 0; i < 8; i++)
	{
		
		if (TESTBIT(data, i))
			SETBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
		else 
			CLEARBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
		
		_delay_us(SHIFT_DELAY);
		SETBIT(SHIFT_CLK_PORT, SHIFT_CLK_BIT);
		_delay_us(SHIFT_DELAY);
		CLEARBIT(SHIFT_CLK_PORT, SHIFT_CLK_BIT);
		CLEARBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);	
		_delay_us(SHIFT_DELAY);
	}
}

void do_shift(void)
{
	if (old_controls != controls)
	{
		old_controls = controls;
		shift_all();
	}
}

void	shift_all(void)
{
	shift_one(controls);
	SHIFT_LATCH_ON;
	_delay_us(SHIFT_DELAY);
	SHIFT_LATCH_OFF;
}

void	shift_init(void)
{
	SETBIT(SHIFT_OUT_DDR, SHIFT_OUT_BIT);
	SETBIT(SHIFT_CLK_DDR, SHIFT_CLK_BIT);
	SETBIT(SHIFT_LATCH_DDR, SHIFT_LATCH_BIT);
	CLEARBIT(SHIFT_OUT_PORT, SHIFT_OUT_BIT);
	CLEARBIT(SHIFT_CLK_PORT, SHIFT_CLK_BIT);
	CLEARBIT(SHIFT_LATCH_PORT, SHIFT_LATCH_BIT);
	controls = 0x00;
	old_controls = 0xFF;
	
}