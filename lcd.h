#if !defined _LCD_INCLUDED
	#define _LCD_INCLUDED

#include "common.h"

#if defined _SECONDARY

	#define LCD_DATA_PORT	PORTA
	#define LCD_DATA_DDR	DDRA
	#define LCD_CMD_PORT	PORTA
	#define LCD_CMD_DDR		DDRA
	#define LCD_CTRL_PORT	PORTC
	#define LCD_CTRL_DDR	DDRC
	#define LCD_RS_BIT		(2)
	#define LCD_RW_BIT		(1)
	#define LCD_EN_BIT		(0)

	#define LCD_CTRL_INIT	{LCD_CTRL_DDR |= _BV(LCD_RS_BIT) | _BV(LCD_RW_BIT) | _BV(LCD_EN_BIT); }

	#define LCD_SET_CMD_MODE	CLEARBIT(LCD_CTRL_PORT, LCD_RS_BIT)
	#define LCD_SET_DATA_MODE	SETBIT(LCD_CTRL_PORT, LCD_RS_BIT)

	#define LCD_SET_READ_MODE	SETBIT(LCD_CTRL_PORT, LCD_RW_BIT)
	#define LCD_SET_WRITE_MODE	CLEARBIT(LCD_CTRL_PORT, LCD_RW_BIT)

	#define LCD_LATCH_ON		SETBIT(LCD_CTRL_PORT, LCD_EN_BIT)
	#define LCD_LATCH_OFF		CLEARBIT(LCD_CTRL_PORT, LCD_EN_BIT)

#endif /* defined _SECONDARY */

#if defined _MASTER

	#define LCD_DATA_ADDR	(0x8085)
	#define LCD_CMD_ADDR	(0x8084)

	#define LCD_CMD_PORT	(*(volatile uint8_t *)LCD_CMD_ADDR)
	#define LCD_DATA_PORT	(*(volatile uint8_t *)LCD_DATA_ADDR)
	
#endif /* _MASTER */

void lcd_init(void);
void lcd_write_cmd(uint8_t);
void lcd_write_data(uint8_t);
void lcd_write_data_(uint8_t);
void lcd_clear(void);
void lcd_puts(uint8_t /* line */, const char * /* msg */);
void lcd_puts_(const char * /* msg */);
void do_lcd(void);
	
#define LCD_TIMEOUT	(1)

extern char lcd_line0[40];
extern char lcd_line1[40];
	
#endif /* _LCD_INCLUDED */