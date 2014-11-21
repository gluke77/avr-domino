#if !defined _KBD_INCLUDED
	#define _KBD_INCLUDED
	
#include <avr\io.h>
#include "../common/common.h"

// Это модуль работы с клавиатурой 4х4
// Входы матрицы подключены к старшей тетраде KBD_PORT
// По таймеру выполнять do_kbd()
// Нажатие клавиши определять макросом KEY_PRESSED(key_id)

#define KBD_PORT		PINF
#define KBD_PORT_DDR	DDRF
#define KBD_SCAN_PORT	PORTC
#define KBD_SCAN_DDR	DDRC
#define KBD_SCAN_BIT0	(4)
#define KBD_SCAN_BIT1	(5)
#define KBD_SCAN_BIT2	(6)
#define KBD_SCAN_BIT3	(7)
#define KBD_INIT		{KBD_PORT_DDR = 0; \
						SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT0); \
						SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT1); \
						SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT2); \
						SETBIT(KBD_SCAN_DDR, KBD_SCAN_BIT2);}
						
#define KBD_BANK_COUNT	(4)
#define KEY_COUNT		(16)						
						
#define KBD_REPEAT_COUNT	(10)
						
void kbd_select(uint8_t);
void kbd_init(void);
void kbd_clear(void);
void one_key_scan(uint8_t);
void kbd_scan(void);
void do_kbd(void);

#define KBD_TIMEOUT	(10)

#define KEY_PRESSED(key_id)				TESTBIT(key_pressed[(key_id) >> 2], (key_id & 0x03) + 4)
#define SET_KEY_PRESSED(key_id)			SETBIT(key_pressed[(key_id) >> 2], (key_id & 0x03) + 4)
#define CLEAR_KEY_PRESSED(key_id)		CLEARBIT(key_pressed[(key_id) >> 2], (key_id & 0x03) + 4)

//#define CURRENT_KEY_STATE(key_id)		TESTBIT(KBD_PORT, (key_id) & 0x07)
#define CURRENT_KEY_STATE(key_id)		current_key_state(key_id)

#define KEY_STATE(key_id)				TESTBIT(key_state[(key_id) >> 2], (key_id & 0x03) + 4)
#define SET_KEY_STATE(key_id)			SETBIT(key_state[(key_id) >> 2], (key_id & 0x03) + 4)
#define CLEAR_KEY_STATE(key_id)			CLEARBIT(key_state[(key_id) >> 2], (key_id & 0x03) + 4)

#define OLD_KEY_STATE(key_id)			TESTBIT(old_key_state[(key_id) >> 2], (key_id & 0x03) + 4)
#define SET_OLD_KEY_STATE(key_id)		SETBIT(old_key_state[(key_id) >> 2], (key_id & 0x03) + 4)
#define CLEAR_OLD_KEY_STATE(key_id)		CLEARBIT(old_key_state[(key_id) >> 2], (key_id & 0x03) + 4)

extern volatile uint8_t	key_pressed[KBD_BANK_COUNT];
extern volatile uint8_t	key_state[KBD_BANK_COUNT];
extern volatile uint8_t	old_key_state[KBD_BANK_COUNT];

#define KEY_MENU	(1)
#define KEY_ENTER	(3)
#define KEY_LEFT	(5)
#define KEY_RIGHT	(7)
#define KEY_UP		(2)
#define KEY_DOWN	(6)

#endif /* _KBD_INCLUDED */