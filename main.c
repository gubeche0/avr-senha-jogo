#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "hd44780.h"

int main() {
  hd44780_init();
  hd44780_puts("Hello World!");
  hd44780_gotoxy(4,1);
  hd44780_puts("Working!");
  while (1)
    ;
}
