#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "hd44780.h"

#define LCD_RESP_PORT PORTD
#define LCD_RESP_DDR DDRD



int main() {
  hd44780_init();
  hd44780_puts("Hello World!");
  hd44780_gotoxy(4,1);
  hd44780_puts("Working!");
  hd44780_gotoxy(0,0);

  LCD_RESP_DDR = 0xff;

  LCD_RESP_PORT = 0xff;

  while (1)
    ;
}
