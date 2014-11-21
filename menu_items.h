#if !defined _MENU_ITEMS_INCLUDED
#define _MENU_ITEMS_INCLUDED

void menu_items_init(void);

void menu_common(void);

void menu_start(void);

void menu_status(void);
void menu_product_count(void);
void menu_print(void);
void menu_message_select(void);
void menu_counter_reset(void);

void menu_usart0_modbus_id(void);
void menu_usart0_baudrate(void);
void menu_usart1_baudrate(void);
void menu_last_error(void);
void menu_store_settings(void);
void menu_reset_settings(void);

void menu_version(void);


#endif /* _MENU_ITEMS_INCLUDED */