#if !defined _SOFT_CONTROLS_INCLUDED
	#define _SOFT_CONTROLS_INCLUDED
	
#include "common.h"

#if defined _MASTER
extern uint16_t soft_controls;
#endif /* _MASTER */

#if defined _SECONDARY
extern uint8_t soft_controls;
#endif /* _SECONDARY */

// PUMPS

#define	SOFT_CONTROL_WATER_MODE0	(0)
#define SOFT_CONTROL_WATER_MODE1	(1)
#define	SOFT_CONTROL_COOLER_PUMP	(2)
#define SOFT_CONTROL_EXTRUDER_PUMP	(3)

// PNEVMO

#define SOFT_CONTROL_COOLER_TUBE	(4)
#define SOFT_CONTROL_PRESSURE		(5)
#define SOFT_CONTROL_SG				(6)
#define SOFT_CONTROL_DRIER			(7)
//

#define SOFT_CONTROL_GERMO			(8)

// these are on secondary

#if defined _MASTER

#define SOFT_LAMP_GERMO				(8)
#define SOFT_LAMP_PRESSURE			(9)

#define SOFT_LAMP_STOP				(10) 

#define SOFT_CONTROL_TRACTOR_LOCK	(11)
#define SOFT_CONTROL_BUNKER_MOTOR	(12)
#define SOFT_CONTROL_FOIL_LED		(13)

#define	SOFT_SIREN_MODE0			(14)
#define	SOFT_SIREN_MODE1			(15)

#endif /* _MASTER */

#if defined _SECONDARY

#define SOFT_LAMP_GERMO				(0) // igk: SOFT_CONTROL_GERMO == SOFT_LAMP_GERMO, I mean it

#define SOFT_LAMP_PRESSURE			(1)
#define SOFT_LAMP_STOP				(2)

#define SOFT_CONTROL_TRACTOR_LOCK	(3)
#define SOFT_CONTROL_BUNKER_MOTOR	(4)
#define SOFT_CONTROL_FOIL_LED		(5)

#define	SOFT_SIREN_MODE0			(6)
#define	SOFT_SIREN_MODE1			(7)

#endif /* _SECONDARY */

#define	SOFT_CONTROL_OFF(idx)	CLEARBIT(soft_controls, idx)
#define	SOFT_CONTROL_ON(idx)	SETBIT(soft_controls, idx)
#define	TEST_SOFT_CONTROL(idx)	(TESTBIT(soft_controls, idx))

#define WATER_MODE_MASK	(_BV(SOFT_CONTROL_WATER_MODE1) | _BV(SOFT_CONTROL_WATER_MODE0))
#define GET_WATER_MODE	(soft_controls & WATER_MODE_MASK)
#define SET_WATER_MODE(mode)	{soft_controls &= ~WATER_MODE_MASK; soft_controls |= (mode & WATER_MODE_MASK);}

#define SIREN_MODE_MASK	(_BV(SOFT_SIREN_MODE1) | _BV(SOFT_SIREN_MODE0))
#define SIREN_MODE_SHIFT	SOFT_SIREN_MODE0
#define GET_SIREN_MODE	((soft_controls & SIREN_MODE_MASK) >> SIREN_MODE_SHIFT)
#define SET_SIREN_MODE(mode)	{soft_controls &= ~SIREN_MODE_MASK; soft_controls |= ((mode << SIREN_MODE_SHIFT) & SIREN_MODE_MASK);}

typedef enum
{
	WATER_ACTION_NOP = 0,
	WATER_ACTION_KEEP_1,
	WATER_ACTION_FILLUP,
	WATER_ACTION_EMPTY,
	WATER_ACTION_MANUAL
} water_action_e;

typedef enum
{
	WATER_MODE_EMPTY = 0,
	WATER_MODE_LEVEL0,
	WATER_MODE_LEVEL1,
	WATER_MODE_MANUAL
} water_mode_e;

#endif /* _SOFT_CONTROLS_INCLUDED */