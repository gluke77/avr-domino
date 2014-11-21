#if !defined _SHIFT_INCLUDED
#define _SHIFT_INCLUDED

#include "..\common\common.h"

#define SHIFT_OUT_PORT		PORTB
#define SHIFT_OUT_DDR		DDRB
#define SHIFT_OUT_BIT		(1)

#define SHIFT_CLK_PORT		PORTB
#define SHIFT_CLK_DDR		DDRB
#define SHIFT_CLK_BIT		(2)

#define SHIFT_LATCH_PORT	PORTB
#define SHIFT_LATCH_DDR		DDRB
#define SHIFT_LATCH_BIT		(4)

#define SHIFT_LATCH_ON		SETBIT(SHIFT_LATCH_PORT, SHIFT_LATCH_BIT)
#define SHIFT_LATCH_OFF		CLEARBIT(SHIFT_LATCH_PORT, SHIFT_LATCH_BIT)

#define SHIFT_DELAY			(20)

extern uint8_t	controls;

#define	CONTROL_OFF(idx)	CLEARBIT(controls, idx & 0x07)
#define	CONTROL_ON(idx)		SETBIT(controls, idx & 0x07)
#define	TEST_CONTROL(idx)	(TESTBIT(controls, idx & 0x07))

void	shift_one(uint8_t);
void	shift_all(void);
void	shift_init(void);
void	do_shift(void);

#endif /* _SHIFT_INCLUDED */