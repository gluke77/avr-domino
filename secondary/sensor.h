#if !defined _SENSOR_INCLUDED
	#define _SENSOR_INCLUDED
	
#include "..\common\common.h"

#define SENSOR_PORT			PINE
#define SENSOR_PORT_DDR		DDRE

#define TEST_SENSOR(idx)	(TESTBIT(sensors, (idx & 0x07)))

extern volatile uint8_t	sensors;
						
void sensor_init(void);
void sensor_scan(void);
void do_sensor(void);
//void sensor_scan1(uint8_t);
	
#endif /* _SENSOR_INCLUDED */