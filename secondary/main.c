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
#include "../common/meto.h"
#include "../common/codenet.h"


char	ch, x, y;
char	buf[50];
char	buf2[50];
char	buf3[50];
int		idx;

modbus_cmd_s	cmd;
uint8_t			msg[MODBUS_MAX_MSG_LENGTH];
result_e		res;

uint8_t	timer_id = 0;

void process_usart(void);
void process_cmd(modbus_cmd_s * /* cmd */);
void process_kbd(void);

void sprintf_codenet(char *, char *);
void usart0_puts(char *);

int main(void)
{
	usart0_init(USART_RS485_MASTER | USART_INT_DISABLE, 115200);
	usart1_init(USART_RS232 | USART_INT_ENABLE, 9600);
	usart1_setprotocol_codenet();

	timer_init();
	kbd_init();
	shift_init();
	sensor_init();
	menu_init();
	menu_items_init();

	beep_init();
	
	GLOBAL_INT_ENABLE;

	beep_ms(1000);
	_delay_ms(500);
	beep_ms(200);
	_delay_ms(200);
	beep_ms(200);
	_delay_ms(200);

	lcd_init();
	
	for (;;)
	{
		do_lcd();
		menu_doitem();
		process_usart();
	}
	return 0;
}


void do_timer(void)
{
	do_kbd();
}


void process_cmd(modbus_cmd_s * cmd)
{
	uint8_t data;
	
	data = (uint8_t)(cmd->value[0] & 0x00FF);
	
	switch (cmd->addr)
	{
		break;
	}
	
	cmd->device_id = 0;
	cmd->cmd_code = MODBUS_READ;
	cmd->cmd_type = MODBUS_ACK;
	cmd->addr = 1;
//	cmd->value[0] = ((uint16_t)soft_sensors & 0x00FF) | ((((uint16_t)soft_controls) << 8) & 0xFF00);

	modbus_cmd2msg(cmd, msg, MODBUS_MAX_MSG_LENGTH);
	
	_delay_ms(10);
	
	usart0_cmd(msg, 0, 0, 0);
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

void sprintf_codenet(char * buf2, char * buf)
{
	int	idx;

	if (buf[0] == CN_ACK)
		sprintf(buf2, "ACK");
	else if (buf[0] == CN_NAK)
	{
		buf2[3] = buf[1];
		buf2[4] = buf[2];
		buf2[5] = buf[3];
		buf2[0] = 'N';
		buf2[1] = 'A';
		buf2[2] = 'K';
		buf2[6] = 0;	
	}
	else
	{
		for (idx = 0;  buf[idx+1] != CN_EOT; idx++)
			buf2[idx] = buf[idx+1];
		buf2[idx] = 0;	
	}
}

void usart0_puts(char * str)
{
	while (*str)
		usart0_putchar(*str++);
}