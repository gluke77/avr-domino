#include <avr\io.h>
#include "common.h"
#include "sensor.h"

volatile uint8_t	sensors;
uint8_t				inverse_mask;

void sensor_init(void)
{
	inverse_mask = 0x00; // igk: was 0x07 -- 2008-11-23
//	inverse_mask |= _BV(SENSOR_END_OF_FOIL);
	sensors = 0x00;
	SENSOR_PORT_DDR = 0;
}

void do_sensor(void)
{
	sensor_scan();
}

void sensor_scan(void)
{
	static uint8_t	tmp_sensors = 0xFF;
	static uint8_t	sensor_value_1 = 0xFF;

	uint8_t	sensor_value;
	uint8_t	sensor_id;

	sensor_value = SENSOR_PORT >> 4;

	for (sensor_id = 0; sensor_id < 8; sensor_id++)
	{
		if (TESTBIT(sensor_value_1, sensor_id) &&
			TESTBIT(tmp_sensors, sensor_id) &&
			!TESTBIT(sensor_value, sensor_id))

			CLEARBIT(sensor_value_1, sensor_id);

		else if (!TESTBIT(sensor_value_1, sensor_id) && //key pressed
			TESTBIT(tmp_sensors, sensor_id) &&
			!TESTBIT(sensor_value, sensor_id))

			CLEARBIT(tmp_sensors, sensor_id);

		else if (!TESTBIT(sensor_value_1, sensor_id) &&
			TESTBIT(tmp_sensors, sensor_id) &&
			TESTBIT(sensor_value, sensor_id))

			SETBIT(sensor_value_1, sensor_id);

		else if (!TESTBIT(sensor_value_1, sensor_id) &&
			!TESTBIT(tmp_sensors, sensor_id) &&
			TESTBIT(sensor_value, sensor_id))

			SETBIT(sensor_value_1, sensor_id);

		else if (TESTBIT(sensor_value_1, sensor_id) &&
			!TESTBIT(tmp_sensors, sensor_id) &&
			!TESTBIT(sensor_value, sensor_id))

			CLEARBIT(sensor_value_1, sensor_id);

		else if (TESTBIT(sensor_value_1, sensor_id) && // key released
			!TESTBIT(tmp_sensors, sensor_id) &&
			TESTBIT(sensor_value, sensor_id))

			SETBIT(tmp_sensors, sensor_id);
	}

	sensors = (tmp_sensors & ~(inverse_mask)) |
		(~(tmp_sensors) & inverse_mask);

}

