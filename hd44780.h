#ifndef _HD44780_H
#define _HD44780_H

#define LCD_PORT PORTC
#define LCD_DDR DDRC
#define LCD_RS PC5
#define LCD_EN PC4
#define LCD_D4 PC3
#define LCD_D5 PC2
#define LCD_D6 PC1
#define LCD_D7 PC0

void hd44780_init();
void hd44780_clear();
void hd44780_puts(char *str);
void hd44780_gotoxy(uint8_t x, uint8_t y);

#endif
