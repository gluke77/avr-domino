#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\eeprom.h>
#include <string.h>
#include <stdio.h>
#include "common.h"
#include "menu.h"
#include "timer.h"
#include "lcd.h"
#include "beep.h"
#include "codenet.h"
#include "menu_items.h"
#include "kbd.h"
#include "sensor.h"

extern int	g_print_on;
extern int	g_message_number;
extern long	g_product_count;

extern uint8_t	g_usart0_modbus_id;
extern long		g_usart0_baudrate;
extern long		g_usart1_baudrate;

extern int		g_usart1_last_error;
extern int		g_last_nak;

void loadFromEE(void);
void storeToEE(void);
void reset_settings(void);

void menu_items_init(void)
{
	uint8_t		idx;
	
	menu_items[MENU_MODE_START][0] = menu_start;
	
	idx = 0; 

	menu_items[MENU_MODE_CONTROLS][idx++] = menu_status;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_product_count;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_print;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_message_select;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_counter_reset;

	idx = 0;
	
	menu_items[MENU_MODE_USART][idx++] = menu_usart0_modbus_id;
	menu_items[MENU_MODE_USART][idx++] = menu_usart0_baudrate;
	menu_items[MENU_MODE_USART][idx++] = menu_usart1_baudrate;
	menu_items[MENU_MODE_USART][idx++] = menu_last_error;
	menu_items[MENU_MODE_USART][idx++] = menu_store_settings;
	menu_items[MENU_MODE_USART][idx++] = menu_reset_settings;

	menu_items[MENU_MODE_VERSION][0] = menu_version;
}

void menu_common(void)
{
	if (KEY_PRESSED(KEY_UP))
	{
		menu_item_prev();
		CLEAR_KEY_PRESSED(KEY_UP);
		beep_ms(50);
	}
	
	if (KEY_PRESSED(KEY_DOWN))
	{
		menu_item_next();
		CLEAR_KEY_PRESSED(KEY_DOWN);
		beep_ms(50);
	}

	if (KEY_PRESSED(KEY_MENU))
	{
		menu_mode_next();
		CLEAR_KEY_PRESSED(KEY_MENU);
		beep_ms(50);
	}
}

void menu_start(void)
{
	sprintf(lcd_line0, "Œ¿Œ Õœœ —»—“≈Ã¿ ");
	sprintf(lcd_line1, "     ÀÃœ“-1     ");
	
	menu_common();
}

void menu_status(void)
{
	sprintf(lcd_line0, "œ≈◊¿“‹:%5s    ", (g_print_on)?"¬ À.":"¬€ À.");
	sprintf(lcd_line1, "—ŒŒ¡Ÿ≈Õ»≈:%03d   ", g_message_number);
	
	menu_common();
}

void menu_product_count(void)
{
	sprintf(lcd_line0, "—◊≈“◊»  œ–Œƒ” “¿");
	sprintf(lcd_line1, "%06ld          ", g_product_count);
	
	menu_common();
}


void menu_print(void)
{
	if (KEY_PRESSED(KEY_ENTER))
	{
		if (codenet_head_enable(1, !g_print_on) >= 0)
			sprintf(lcd_line1, "Œ               ");
		else
			sprintf(lcd_line1, "Œÿ»¡ ¿          ");
			
		lcd_puts(1, lcd_line1);
		beep_ms(500);
		_delay_ms(500);
		sprintf(lcd_line1, "                 ");
	
		CLEAR_KEY_PRESSED(KEY_ENTER);
	}

	if (g_print_on)
		sprintf(lcd_line0, "¬€ Àﬁ◊»“‹ œ≈◊¿“‹ ");	
	else
		sprintf(lcd_line0, "¬ Àﬁ◊»“‹ œ≈◊¿“‹  ");	
	
	sprintf(lcd_line1, "                ");
	
	menu_common();
}

void menu_message_select(void)
{
	static int msg = 1;

	if (g_menu_changed)
	{
		g_menu_changed = 0;
		msg = g_message_number;
	}
	
	if (KEY_PRESSED(KEY_ENTER))
	{
		if (codenet_message_to_head_assignment(1, msg) >= 0)
			sprintf(lcd_line1, "Œ               ");
		else
			sprintf(lcd_line1, "Œÿ»¡ ¿          ");
			
		lcd_puts(1, lcd_line1);
		beep_ms(500);
		_delay_ms(500);
		sprintf(lcd_line1, "                 ");
	
		CLEAR_KEY_PRESSED(KEY_ENTER);
	}

	if (KEY_PRESSED(KEY_RIGHT))
	{
		if (++msg > 4)
			msg = 1;
			
		CLEAR_KEY_PRESSED(KEY_RIGHT);
		beep_ms(50);
	}

	if (KEY_PRESSED(KEY_LEFT))
	{
		if (--msg < 1)
			msg = 4;
		
		CLEAR_KEY_PRESSED(KEY_LEFT);
		beep_ms(50);
	}
	
	sprintf(lcd_line0, "”—“¿ÕŒ¬»“‹       ");	
	sprintf(lcd_line1, "—ŒŒ¡Ÿ≈Õ»≈:%d      ", msg);	
	
	menu_common();
}

void menu_counter_reset(void)
{
	if (KEY_PRESSED(KEY_ENTER))
	{
		if (codenet_product_counting(0) >= 0)
			sprintf(lcd_line1, "Œ               ");
		else
			sprintf(lcd_line1, "Œÿ»¡ ¿          ");
			
		lcd_puts(1, lcd_line1);
		beep_ms(500);
		_delay_ms(500);
		sprintf(lcd_line1, "                    ");
	
		CLEAR_KEY_PRESSED(KEY_ENTER);
	}
	
	sprintf(lcd_line0, "—¡–Œ—»“‹ —◊≈“◊» ");
	sprintf(lcd_line1, "                ");	
	
	menu_common();
}

void menu_usart0_modbus_id(void)
{
	sprintf(lcd_line0, "¿ƒ–≈— ”—“–Œ…—“¬¿");
	sprintf(lcd_line1, "%-16d", g_usart0_modbus_id);
	
	if (KEY_PRESSED(KEY_RIGHT))
	{
		g_usart0_modbus_id++;
			
		CLEAR_KEY_PRESSED(KEY_RIGHT);
		beep_ms(50);
	}

	if (KEY_PRESSED(KEY_LEFT))
	{
		g_usart0_modbus_id--;
		
		CLEAR_KEY_PRESSED(KEY_LEFT);
		beep_ms(50);
	}

	menu_common();
}

uint32_t	baud[] = {2400, 4800, 9600, 19200, 38400, 57600, 115200};

void menu_usart0_baudrate(void)
{
	uint8_t		idx;
	
	for (idx = 0; idx < 7; idx++)
		if (g_usart0_baudrate == baud[idx])
			break;
	
	sprintf(lcd_line0, "— Œ–Œ—“‹ œŒ–“¿  ");
	sprintf(lcd_line1, "PC:%-6ld           ", g_usart0_baudrate);

	if (KEY_PRESSED(KEY_RIGHT))
	{
		idx++;
		
		if (7 == idx)
			idx = 0;
			
		CLEAR_KEY_PRESSED(KEY_RIGHT);
		beep_ms(50);
	}

	if (KEY_PRESSED(KEY_LEFT))
	{
		if (0 == idx)
			idx = 7;
			
		idx--;
		
		CLEAR_KEY_PRESSED(KEY_LEFT);
		beep_ms(50);
	}
	
	g_usart0_baudrate = baud[idx];

	menu_common();
}

void menu_usart1_baudrate(void)
{
	uint8_t		idx;
	
	for (idx = 0; idx < 7; idx++)
		if (g_usart1_baudrate == baud[idx])
			break;
	
	sprintf(lcd_line0, "— Œ–Œ—“‹ œŒ–“¿  ");
	sprintf(lcd_line1, "œ–»Õ“≈–¿:%-6ld ", g_usart1_baudrate);
	
	if (KEY_PRESSED(KEY_RIGHT))
	{
		idx++;
		
		if (7 == idx)
			idx = 0;
			
		CLEAR_KEY_PRESSED(KEY_RIGHT);
		beep_ms(50);
	}

	if (KEY_PRESSED(KEY_LEFT))
	{
		if (0 == idx)
			idx = 7;
			
		idx--;
		
		CLEAR_KEY_PRESSED(KEY_LEFT);
		beep_ms(50);
	}
	
	g_usart1_baudrate = baud[idx];

	menu_common();
}

void menu_last_error(void)
{
	sprintf(lcd_line0, "œŒ—À. Œÿ»¡ ¿:%-3d", g_usart1_last_error);
	sprintf(lcd_line1, "œŒ—À. NAK:%03d   ", g_last_nak);
	
	menu_common();
}


void menu_store_settings(void)
{
	if (KEY_PRESSED(KEY_ENTER))
	{
		storeToEE();
		sprintf(lcd_line0, "”—“¿ÕŒ¬ »       ");
		sprintf(lcd_line1, "—Œ’–¿Õ≈Õ€       ");
		lcd_puts(0, lcd_line0);
		lcd_puts(1, lcd_line1);
		beep_ms(500);
		_delay_ms(500);
	
		CLEAR_KEY_PRESSED(KEY_ENTER);
	}
	
	sprintf(lcd_line0, "—Œ’–¿Õ»“‹       ");
	sprintf(lcd_line1, "”—“¿ÕŒ¬ »       ");
	
	menu_common();
}

void menu_reset_settings(void)
{
	if (KEY_PRESSED(KEY_ENTER))
	{
		reset_settings();
		storeToEE();
		sprintf(lcd_line0, "«¿¬Œƒ— »≈ ”—“¿Õ.");
		sprintf(lcd_line1, "¬Œ——“¿ÕŒ¬À≈Õ€   ");
		lcd_puts(0, lcd_line0);
		lcd_puts(1, lcd_line1);
		beep_ms(500);
		_delay_ms(500);
	
		CLEAR_KEY_PRESSED(KEY_ENTER);
	}
	
	sprintf(lcd_line0, "¬Œ——“¿ÕŒ¬»“‹    ");
	sprintf(lcd_line1, "«¿¬Œƒ— »≈ ”—“¿Õ.");
	
	menu_common();
}

void loadFromEE(void)
{
	g_usart0_modbus_id = eeprom_read_byte(USART0_MODBUS_ID_ADDR);

	g_usart0_baudrate = (uint32_t)eeprom_read_word(USART0_BAUD_LO_ADDR);
	g_usart0_baudrate |= ((uint32_t)(eeprom_read_word(USART0_BAUD_HI_ADDR))) << 16;
	
	g_usart1_baudrate = (uint32_t)eeprom_read_word(USART1_BAUD_LO_ADDR);
	g_usart1_baudrate |= ((uint32_t)(eeprom_read_word(USART1_BAUD_HI_ADDR))) << 16;
}

void storeToEE(void)
{
	eeprom_write_byte(USART0_MODBUS_ID_ADDR, g_usart0_modbus_id);

	eeprom_write_word(USART0_BAUD_LO_ADDR, (uint16_t)(g_usart0_baudrate & 0x0000FFFF));
	eeprom_write_word(USART0_BAUD_HI_ADDR, (uint16_t)(g_usart0_baudrate >> 16));
	
	eeprom_write_word(USART1_BAUD_LO_ADDR, (uint16_t)(g_usart1_baudrate & 0x0000FFFF));
	eeprom_write_word(USART1_BAUD_HI_ADDR, (uint16_t)(g_usart1_baudrate >> 16));
}

void reset_settings(void)
{
	g_usart0_modbus_id = 1;
	g_usart0_baudrate = 115200;
	g_usart1_baudrate = 9600;
}

void menu_version(void)
{
	sprintf(lcd_line0, "¬≈–—»ﬂ œŒ:      ");
	sprintf(lcd_line1, "%-16s", FW_VERSION);
	menu_common();
}
