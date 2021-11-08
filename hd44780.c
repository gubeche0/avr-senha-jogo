#include <avr/io.h>
#include <util/delay.h>
#include "hd44780.h"

uint8_t mask(uint8_t halfbyte) {
  uint8_t res = 0;

  if (halfbyte & 0x01) res |= (1<<LCD_D4);
  if (halfbyte & 0x02) res |= (1<<LCD_D5);
  if (halfbyte & 0x04) res |= (1<<LCD_D6);
  if (halfbyte & 0x08) res |= (1<<LCD_D7);

  return res;
}

// Envia um comando (modo=0) ou um dado (modo=1) para o LCD
void hd44780_send(uint8_t val, uint8_t modo) {
  LCD_PORT &= ~(1<<LCD_D4 | 1<<LCD_D5 | 1<<LCD_D6 | 1<<LCD_D7);
  if (modo)  // modo == 1
    LCD_PORT |= (1 << LCD_RS);
  else
    LCD_PORT &= ~(1 << LCD_RS);
  _delay_us(1);
  LCD_PORT |= mask(val >> 4);
  LCD_PORT |= (1 << LCD_EN);
  _delay_us(1);
  LCD_PORT &= ~(1 << LCD_EN);
  _delay_us(1);
  LCD_PORT &= ~(1<<LCD_D4 | 1<<LCD_D5 | 1<<LCD_D6 | 1<<LCD_D7);
  LCD_PORT |= mask(val & 0xF);
  LCD_PORT |= (1 << LCD_EN);
  _delay_us(1);
  LCD_PORT &= ~(1 << LCD_EN);
  _delay_us(1);
}

void hd44780_init() {
  LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_D4) | (1 << LCD_D5) |
             (1 << LCD_D6) | (1 << LCD_D7);
  _delay_ms(15);
  hd44780_send(0x33, 0); // inicialização 4 bits
  hd44780_send(0x33, 0); // idem
  hd44780_send(0x32, 0); // idem (precisa repetir 3x)
  hd44780_send(0x28, 0); // function set: 2 linhas, caract. 5x7
  hd44780_send(0x08, 0); // display off
  hd44780_send(0x0F, 0); // display on, cursor on, blink on
  hd44780_send(0x01, 0); // clear display
}

void hd44780_clear() { hd44780_send(0x01, 0); }

void hd44780_puts(char *str) {
  while (*str) {
    hd44780_send(*str++, 1);
  }
}

void hd44780_gotoxy(uint8_t x, uint8_t y) {
  uint8_t cmd = 0x80;
  if(y == 1) cmd = 0xC0;
  hd44780_send(cmd + x, 0);
}
