#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "hd44780.h"
#include "print.h"
#include "usart.h"
#include "adc.h"

#define LCD_RESP_PORT PORTD
#define LCD_RESP_DDR DDRD

#define UP 8
#define DOWN 2
#define LEFT 4
#define RIGHT 6
#define CENTER 5

#define BUTTON_PIN PINC
#define BUTTON_UP PC0
#define BUTTON_DOWN PC2
#define BUTTON_LEFT PC3
#define BUTTON_RIGHT PC4
#define BUTTON_CENTER PC1


int16_t password = 1543;
int8_t cursorSelecionado = 4;
int changeCursor = 0;

ISR(PCINT1_vect)
{
	print("bbbb");
    if (BUTTON_PIN & (1 << BUTTON_LEFT)) {       // lê PB1
        // PORTB ^= (1 << PB5);       // toggle em PB0
		changeCursor = 1;
	    print("aaaaa");
		LCD_RESP_PORT ^= 0xff;
		if (cursorSelecionado == 0)  {
			cursorSelecionado = 4;
		} else {
			cursorSelecionado += 1;
		}
        while (BUTTON_PIN & (1 << BUTTON_LEFT))
            _delay_ms(1);          // debounce
    }    
    _delay_ms(1);                  // debounce
}

float readJoystickX() {
	adc_set_channel(0);
	float x = adc_read() * 0.0009765625;
	adc_set_channel(1);	
	float y = adc_read() * 0.0009765625;

	print("\nx: ");
    printfloat(x);
    print("  y: ");
    printfloat(y);

	return y;
}

int main() {
	USART_Init();
  	adc_init();

	DDRC &= ~((1 << BUTTON_CENTER) | (1 << BUTTON_DOWN) | (1 << BUTTON_UP) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT)); // PB0 entrada
    PORTC |= (1 << BUTTON_CENTER) | (1 << BUTTON_DOWN) | (1 << BUTTON_UP) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT); // Ativar pull up

    PORTC &= ~(1 << BUTTON_LEFT);          // desabilita pull-up de PB1


	PCICR |= (1 << PCIE1);         // habilita vetor de interrupção para PCINT14 .. PCINTB8
    PCMSK1 |= (1 << PCINT11);       // habilita interrupção para PC3


    sei();

	
	hd44780_init();
	hd44780_puts("Hello World!");
	hd44780_gotoxy(4,1);
	hd44780_puts("Working!");
	hd44780_gotoxy(0,0);

	LCD_RESP_DDR = 0xff;

	LCD_RESP_PORT = 0xff;

	print("password: ");
	printint(password);
	print("\n");

	
}
