#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\eeprom.h>
#include <string.h>
#include <stdio.h>
#include "../common/common.h"
#include "../common/menu.h"
#include "menu_items.h"
#include "../common/timer.h"
#include "kbd.h"
#include "../common/lcd.h"
#include "sensor.h"
#include "../common/soft_controls.h"
#include "../common/beep.h"

void menu_items_init(void)
{
	uint8_t		idx;
	
	menu_items[MENU_MODE_START][0] = menu_start;
	
//	menu_items[MENU_MODE_SENSORS][0] = menu_sensors;
	
	menu_items[MENU_MODE_SCAN_SENSORS][0] = menu_scan_sensors;

	idx = 0; 

	menu_items[MENU_MODE_CONTROLS][idx++] = menu_controls;

	menu_items[MENU_MODE_CONTROLS][idx++] = menu_stop_lamp;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_germo_lamp;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_pressure_lamp;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_tractor_lock;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_bunker_motor;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_foil_led;
	menu_items[MENU_MODE_CONTROLS][idx++] = menu_siren_mode;

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

void menu_sensors(void)
{
	uint8_t	idx;
	
	sprintf(lcd_line0, "¬’Œƒ€           ");
	sprintf(lcd_line1, "                ");
		
	for (idx = 0; idx < 3; idx++)
		lcd_line1[idx] = (TEST_SENSOR(idx))?'1':'.';

	menu_common();
}

char * sensor_text[][3] =
	{
		{" ŒÕ≈÷ ‘ŒÀ‹√»     ", "ƒ¿", "Õ≈“"},
		{"«¿Ã≈Õ¿ ¡Œ¡»Õ€    ", "ƒ¿", "Õ≈“"},
		{"—ÃŒ“ ¿ ‘ŒÀ‹√»    ", "ƒ¿", "Õ≈“"}
	};

#define SENSOR_COUNT	(3)


void menu_scan_sensors(void)
{
	static uint8_t scan_mode_on = 1;
	static uint8_t sensor_id = 0;
	static uint8_t timer_id = 0;
	
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		scan_mode_on ^= 0x01;
		beep_ms(50);
	}
	
	if ((scan_mode_on) && (0 == timer_id))
			timer_id = start_timer(1000);


	if (0 != timer_id)
	{
		if (!timer_value(timer_id))
		{
			stop_timer(timer_id);
			timer_id = 0;
			
			if (scan_mode_on)
				sensor_id++;
			
			if (2 < sensor_id)
				sensor_id = 0;
		}
	}


	if (!scan_mode_on)
	{
		if (KEY_PRESSED(KEY_UP))
		{
			CLEAR_KEY_PRESSED(KEY_UP);
			beep_ms(50);
			
			if (0 == sensor_id)
				sensor_id = SENSOR_COUNT;
				
			sensor_id--;
		}
		
		if (KEY_PRESSED(KEY_DOWN))
		{
			CLEAR_KEY_PRESSED(KEY_DOWN);
			beep_ms(50);
			
			sensor_id++;
			
			if (SENSOR_COUNT <= sensor_id)
				sensor_id = 0;
		}
	}

	sprintf(lcd_line0, "%s               ", sensor_text[sensor_id][0]);
		
	sprintf(lcd_line1, "%s               ",
		(TEST_SENSOR(sensor_id))?sensor_text[sensor_id][1]:sensor_text[sensor_id][2]);

	menu_common();
}

void menu_controls(void)
{
	sprintf(lcd_line0,"¬€’Œƒ€          ");
	sprintf(lcd_line1,"                ");
	
	lcd_line1[SOFT_LAMP_GERMO] = (TEST_SOFT_CONTROL(SOFT_LAMP_GERMO))?'√':'.';
	lcd_line1[SOFT_LAMP_PRESSURE] = (TEST_SOFT_CONTROL(SOFT_LAMP_PRESSURE))?'ƒ':'.';
	lcd_line1[SOFT_LAMP_STOP] = (TEST_SOFT_CONTROL(SOFT_LAMP_STOP))?'S':'.';
	lcd_line1[SOFT_CONTROL_TRACTOR_LOCK] = (TEST_SOFT_CONTROL(SOFT_CONTROL_TRACTOR_LOCK))?'œ':'.';
	lcd_line1[SOFT_CONTROL_BUNKER_MOTOR] = (TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR))?'¡':'.';
	lcd_line1[SOFT_CONTROL_FOIL_LED] = (TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))?'À':'.';
	
	lcd_line1[SOFT_SIREN_MODE0] = ' ';
	lcd_line1[SOFT_SIREN_MODE1] = '0' + GET_SIREN_MODE;

	menu_common();
}

void menu_germo_lamp(void)
{	
	sprintf(lcd_line0,"À¿Ãœ¿ '√≈–Ã≈“.' ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_LAMP_GERMO))?"¬ Àﬁ◊≈Õ¿":"¬€ Àﬁ◊≈Õ¿");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_LAMP_GERMO))
			SOFT_CONTROL_OFF(SOFT_LAMP_GERMO);
		else
			SOFT_CONTROL_ON(SOFT_LAMP_GERMO);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_pressure_lamp(void)
{	
	sprintf(lcd_line0,"À¿Ãœ¿ 'ƒ¿¬À≈Õ»≈'");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_LAMP_PRESSURE))?"¬ Àﬁ◊≈Õ¿":"¬€ Àﬁ◊≈Õ¿");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_LAMP_PRESSURE))
			SOFT_CONTROL_OFF(SOFT_LAMP_PRESSURE);
		else
			SOFT_CONTROL_ON(SOFT_LAMP_PRESSURE);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_stop_lamp(void)
{	
	sprintf(lcd_line0,"À¿Ãœ¿ '—“Œœ'    ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_LAMP_STOP))?"¬ Àﬁ◊≈Õ¿":"¬€ Àﬁ◊≈Õ¿");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_LAMP_STOP))
			SOFT_CONTROL_OFF(SOFT_LAMP_STOP);
		else
			SOFT_CONTROL_ON(SOFT_LAMP_STOP);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_foil_led(void)
{	
	sprintf(lcd_line0," ŒÕ“–ŒÀ‹ ‘ŒÀ‹√» ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))?"¬ Àﬁ◊≈Õ":"¬€ Àﬁ◊≈Õ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_FOIL_LED))
			SOFT_CONTROL_OFF(SOFT_CONTROL_FOIL_LED);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_FOIL_LED);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_bunker_motor(void)
{	
	sprintf(lcd_line0,"ƒ¬»√. Õ¿ Œœ»“≈Àﬂ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR))?"¬ Àﬁ◊≈Õ":"¬€ Àﬁ◊≈Õ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_BUNKER_MOTOR))
			SOFT_CONTROL_OFF(SOFT_CONTROL_BUNKER_MOTOR);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_BUNKER_MOTOR);
			
		beep_ms(50);
	}
	
	menu_common();
}

void menu_tractor_lock(void)
{	
	sprintf(lcd_line0,"œ–»∆»Ã “”       ");
	sprintf(lcd_line1,"%s                ", 
		(TEST_SOFT_CONTROL(SOFT_CONTROL_TRACTOR_LOCK))?"¬ Àﬁ◊≈Õ":"¬€ Àﬁ◊≈Õ");
		
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		if (TEST_SOFT_CONTROL(SOFT_CONTROL_TRACTOR_LOCK))
			SOFT_CONTROL_OFF(SOFT_CONTROL_TRACTOR_LOCK);
		else
			SOFT_CONTROL_ON(SOFT_CONTROL_TRACTOR_LOCK);
			
		beep_ms(50);
	}

	
	menu_common();
}

void menu_siren_mode(void)
{	
	uint8_t siren_mode;
	
	sprintf(lcd_line0,"–≈∆»Ã —»–≈Õ€    ");
	sprintf(lcd_line1,"%d                ", GET_SIREN_MODE);
	
	if (KEY_PRESSED(KEY_ENTER))
	{
		CLEAR_KEY_PRESSED(KEY_ENTER);
		
		siren_mode = GET_SIREN_MODE;
		siren_mode++;
		if (3 < siren_mode)
			siren_mode = 0;
		SET_SIREN_MODE(siren_mode);
		
		beep_ms(50);
	}
	
	menu_common();
}

void menu_version(void)
{
	sprintf(lcd_line0, "¬≈–—»ﬂ œŒ:      ");
	sprintf(lcd_line1, "%-16s", FW_VERSION);
	menu_common();
}
