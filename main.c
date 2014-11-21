#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "usart.h"
#include "lcd.h"
#include "beep.h"
#include "timer.h"
#include "shift_out.h"
#include "kbd.h"
#include "sensor.h"
#include "modbus.h"
#include "menu.h"
#include "menu_items.h"
#include "codenet.h"

modbus_cmd_s	cmd;
uint8_t			msg[MODBUS_MAX_MSG_LENGTH];
result_e		res;

int		g_print_on;
int		g_message_number;
long	g_product_count;

uint8_t	g_usart0_modbus_id;
long	g_usart0_baudrate;
long	g_usart1_baudrate;

extern	int	g_usart1_last_error;
extern  int	g_last_nak;

void process_usart(void);
void process_cmd(modbus_cmd_s * /* cmd */);
void process_codenet(void);

void loadFromEE(void);

int main(void)
{
	timer_init();
	kbd_init();
	shift_init();
	sensor_init();
	menu_init();
	menu_items_init();

	beep_init();

	loadFromEE();

	usart0_init(USART_RS485_SLAVE | USART_INT_ENABLE, g_usart0_baudrate);
	usart1_init(USART_RS232 | USART_INT_ENABLE, g_usart1_baudrate);
	usart1_setprotocol_codenet();
	
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
		process_codenet();
	}
	return 0;
}


void do_timer(void)
{
	do_kbd();
}

void process_cmd(modbus_cmd_s * cmd)
{
	int	value;

	if (cmd->device_id != g_usart0_modbus_id)
		return;
	
	if (MODBUS_WRITE == cmd->cmd_code)
	{
		value = cmd->value[0];
		switch (cmd->addr)
		{
		case 0x0000:				// print
			codenet_head_enable(1, value);
				
			break;
		
		case 0x0001:				// msg
			if (1 > value)
				value = 1;
			if (4 < value)
				value= 4;
				
			codenet_message_to_head_assignment(1, value);
				
			break;
			
		case 0x0002:				// reset count
			if (0xEFFE == value)
				codenet_product_counting(0);
			
			break;
		
		default:
			break;
		}
	}
	else if (MODBUS_READ == cmd->cmd_code)
	{
		cmd->device_id = g_usart0_modbus_id;
		cmd->cmd_code = MODBUS_READ;
		cmd->cmd_type = MODBUS_ACK;
		cmd->addr = 6;

		cmd->value[0] = g_print_on;
		cmd->value[1] = g_message_number;
		cmd->value[2] = g_product_count && 0x0000FFFF;
		cmd->value[3] = (g_product_count && 0xFFFF0000) >> 16;
		cmd->value[4] = g_usart1_last_error;
		cmd->value[5] = g_last_nak;
	}
	
	_delay_ms(10);
	modbus_cmd2msg(cmd, msg, MODBUS_MAX_MSG_LENGTH);
	usart0_cmd(msg, 0, 0, 300);
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
//					beep_ms(20);

			}
		}
}

void process_codenet(void)
{
	static int action = 0;
	static int codenet_timeout = CODENET_TIMEOUT;
	long	res;
	
	if (--codenet_timeout)
		return;
		
	codenet_timeout = CODENET_TIMEOUT;
	
	if (++action > 2)
		action = 0;
		
	switch (action)
	{
	case 0:
		if ((res = codenet_head_enable(1, '?')) >= 0)
			g_print_on = (int)res;
			
		break;
	
	case 1:
		if ((res = codenet_message_to_head_assignment(1, '?')) >= 0)
			g_message_number = (int)res;
			
		break;
		
	case 2:
		if ((res = codenet_product_counting('?')) >= 0)
			g_product_count = res;
			
		break;
	
	default:
		break;
	}
}
