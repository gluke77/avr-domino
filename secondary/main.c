#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "../common/common.h"
#include "../common/usart.h"
#include "../common/lcd.h"
#include "../common/beep.h"
#include "../common/timer.h"
#include "shift_out.h"
#include "kbd.h"
#include "sensor.h"
#include "../common/modbus.h"
#include "../common/menu.h"
#include "menu_items.h"
#include "../common/soft_controls.h"
#include "../common/meto.h"


char	ch, x, y;
char	buf[50];

modbus_cmd_s	cmd;
uint8_t			msg[MODBUS_MAX_MSG_LENGTH];
result_e		res;

uint8_t	timer_id = 0;

uint8_t soft_controls = 0;
uint8_t soft_sensors = 0;

void process_usart(void);
void process_cmd(modbus_cmd_s * /* cmd */);
void process_kbd(void);
void process_soft_controls(void);
void process_siren(void);
void process_foil(void);

int main(void)
{
	usart0_init(USART_RS485_MASTER | USART_INT_DISABLE, 115200);
	usart1_init(USART_RS232 | USART_INT_DISABLE, 115200);

	timer_init();
	kbd_init();
	shift_init();
	sensor_init();
//	menu_init();
//	menu_items_init();

	beep_init();
	
	GLOBAL_INT_ENABLE;

	beep_ms(1000);
	_delay_ms(500);
	beep_ms(200);
	_delay_ms(200);
	beep_ms(200);
	_delay_ms(200);

	lcd_init();
//	sprintf(lcd_line0, "01234567890123456");


	for (;;)
	{
		_delay_ms(500);
		
		usart0_putchar('0');
		usart1_putchar('1');
		
		process_kbd();
		do_lcd();

		do_shift();
		do_sensor();
		
		sprintf(lcd_line0, "SENSORS %d      ", sensors);
		
		if (!controls)
			controls = 0xFF;
		else
			controls = 0x00;
	
/*		
		menu_doitem();

		process_usart();
		process_soft_controls();
		process_siren();
		
		soft_sensors = sensors;
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR) &&
			TEST_SENSOR(SENSOR_END_OF_FOIL))
			
			SOFT_CONTROL_OFF(SOFT_CONTROL_BUNKER_MOTOR);
			
		if (!TEST_SENSOR(SENSOR_END_OF_FOIL) &&
			TEST_SENSOR(SENSOR_SEC_REEL))
			
			SOFT_CONTROL_ON(SOFT_CONTROL_BUNKER_MOTOR);
*/
	}
	return 0;
}


void do_timer(void)
{
	do_kbd();
}

#define CMD_NOP			(0)
#define CMD_SETDATA		(1)
#define CMD_SETBITS		(2)
#define CMD_CLEARBITS	(3)

void process_cmd(modbus_cmd_s * cmd)
{
	uint8_t data;
	
	data = (uint8_t)(cmd->value[0] & 0x00FF);
	
	switch (cmd->addr)
	{
	case CMD_NOP:
		break;
	case CMD_SETDATA:
		soft_controls = data;
		break;
	case CMD_SETBITS:
		soft_controls |= data;
		break;
	case CMD_CLEARBITS:
		soft_controls &= ~data;
		break;
	}
	
	cmd->device_id = 0;
	cmd->cmd_code = MODBUS_READ;
	cmd->cmd_type = MODBUS_ACK;
	cmd->addr = 1;
	cmd->value[0] = ((uint16_t)soft_sensors & 0x00FF) | ((((uint16_t)soft_controls) << 8) & 0xFF00);

	modbus_cmd2msg(cmd, msg, MODBUS_MAX_MSG_LENGTH);
	
	_delay_ms(10);
	
	usart0_cmd(msg, 0, 0, 0);
}

void process_kbd(void)
{
	uint8_t key_id;
	
	for (key_id = 0; key_id < KEY_COUNT; key_id++)
		if (KEY_PRESSED(key_id))
		{
			CLEAR_KEY_PRESSED(key_id);
			sprintf(lcd_line1, "PRESSED %d         ", key_id);
			usart0_putchar('0' + key_id);
		}
}

void process_usart(void)
{
		if (usart0_msg_ready)
		{
			res = modbus_msg2cmd(usart0_inbuf, &cmd);
			usart0_msg_ready = 0;
			
			if (RESULT_OK == res)
				process_cmd(&cmd);
			else
			{
				//todo : process bad cmd here
//				beep_ms(20);

			}
		}
}

void process_soft_controls(void)
{
	static uint8_t old_soft_controls = 0;
	
	if (old_soft_controls != soft_controls)
	{
		old_soft_controls = soft_controls;
		
		controls &= 0xC0;	// 0xE0 if you want pulse foil led
		controls |= soft_controls;
	}
}

uint8_t		siren_step_count[] = {0, 1, 5};
uint16_t	siren_program[3][6] = {{500, 500},
									{100, 1000},
									{50, 50, 50, 50, 50, 1000}};

void process_siren(void)
{
	static uint16_t timer_id = 0;
	static uint8_t step = 0;
	
	uint8_t mode;
	
	mode = GET_SIREN_MODE;
	
	if (0 == mode)
	{
		if (timer_id)
			stop_timer(timer_id);
			
		timer_id = 0;
		step = 0;
		CONTROL_OFF(CONTROL_SIREN);
		CONTROL_OFF(LAMP_RED);
	}
	else
	{
		CONTROL_ON(LAMP_RED);
		
		if (0 == timer_id)
		{
			timer_id = start_timer(siren_program[mode - 1][step]);

			if (step & 0x01)
				CONTROL_OFF(CONTROL_SIREN);
			else
				CONTROL_ON(CONTROL_SIREN);
		}
		else
			if (!timer_value(timer_id))
			{
				stop_timer(timer_id);
				timer_id = 0;
				
				step++;
				if (step > siren_step_count[mode - 1])
					step = 0;
					
			}
	}	
}


#define FOIL_TIMEOUT	(500)

void process_foil(void)
{
	static uint16_t	timer_id = 0;
	

	if (TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))
	{
		if (0 == timer_id)
		{
			timer_id = start_timer(FOIL_TIMEOUT);
			
			if (TEST_CONTROL(CONTROL_FOIL_LED))
				CONTROL_OFF(CONTROL_FOIL_LED);
			else
				CONTROL_ON(CONTROL_FOIL_LED);
		}
		else
		{
			if (0 == timer_value(timer_id))
			{
				stop_timer(timer_id);
				timer_id = 0;
			}
			else if (timer_value(timer_id) < FOIL_TIMEOUT / 2)
			{
				if (TEST_CONTROL(CONTROL_FOIL_LED))
				{
					if (TEST_SENSOR(SENSOR_END_OF_FOIL))
						SETBIT(soft_sensors, SENSOR_END_OF_FOIL);
					else
						CLEARBIT(soft_sensors, SENSOR_END_OF_FOIL);
				
					if (TEST_SENSOR(SENSOR_PRI_REEL))
						SETBIT(soft_sensors, SENSOR_PRI_REEL);
					else
						CLEARBIT(soft_sensors, SENSOR_PRI_REEL);

					if (TEST_SENSOR(SENSOR_SEC_REEL))
						SETBIT(soft_sensors, SENSOR_SEC_REEL);
					else
						CLEARBIT(soft_sensors, SENSOR_SEC_REEL);
				}
				else
				{
					if (TEST_SENSOR(SENSOR_END_OF_FOIL))
						SETBIT(soft_sensors, ERROR_SENSOR_END_OF_FOIL);
				
					if (TEST_SENSOR(SENSOR_PRI_REEL))
						SETBIT(soft_sensors, ERROR_SENSOR_PRI_REEL);

					if (TEST_SENSOR(SENSOR_SEC_REEL))
						SETBIT(soft_sensors, ERROR_SENSOR_SEC_REEL);
				}
			}
		}
	}
	else
	{
		CONTROL_OFF(CONTROL_FOIL_LED);
		soft_sensors = sensors;
		
		if (0 != timer_id)
			stop_timer(timer_id);
		timer_id = 0;
		
		CLEARBIT(soft_sensors, ERROR_SENSOR_END_OF_FOIL);
		CLEARBIT(soft_sensors, ERROR_SENSOR_PRI_REEL);
		CLEARBIT(soft_sensors, ERROR_SENSOR_SEC_REEL);
	}
}