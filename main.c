#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "hd44780.h"
#include "print.h"
#include "usart.h"
#include "adc.h"
#include <stdio.h>

#define TIMER_CLK		F_CPU / 256

#define LCD_RESP_PORT PORTD
#define LCD_RESP_DDR DDRD

#define UP 8
#define DOWN 2
#define LEFT 4
#define RIGHT 6
#define CENTER 5

#define BUTTON PC2


int16_t password = 1092;
int8_t cursorSelecionado = 0;
int changeDisplay = 0;
int8_t pos;
// int8_t tentativa[] = [0,0,0,0];
int16_t tentativa = 0;
int16_t tempoTentativaAtual = 30;

ISR(TIMER1_OVF_vect){
	tempoTentativaAtual--;
	changeDisplay = 1;
	// printint(tempoTentativaAtual);
	TCNT1  = 65536 - TIMER_CLK;
}

int8_t readJoystick() {
	adc_set_channel(0);
	float x = adc_read() * 0.0009765625;
	adc_set_channel(1);	
	float y = adc_read() * 0.0009765625;

	// print("\nx: ");
    // printfloat(x);
    // print("  y: ");
    // printfloat(y);

	if (x > 0.65) return RIGHT;
	if (x < 0.35) return LEFT;
	if (y > 0.65) return UP;
	if (y < 0.35) return DOWN;

	return CENTER;
}

void drawDisplay() {
	hd44780_clear();
	// hd44780_puts("Tempo: ");
	hd44780_puts(("Tempo: "));
	char time[2];
	sprintf(time, "%d", tempoTentativaAtual); 
	hd44780_puts(time);
	hd44780_gotoxy(4,1);
	char tent[5];
	sprintf(tent, "%04d", tentativa); 
	hd44780_puts(tent);
	hd44780_gotoxy(cursorSelecionado + 4,1);

	changeDisplay = 0;
}

int main() {
	USART_Init();
  	adc_init();
	
	cli();

	// resseta contadores para TIMER1. OC2A e OC2B desconectados, modo normal
	TCCR1A = 0;
	TCCR1B = 0;
	// frequência: 1 Hz (65536 - 16MHz / 256)
	TCNT1  = 65536 - TIMER_CLK;
	// seta bit para prescaler 256
	TCCR1B |= (1 << CS12);
	// habilita máscara do timer1 para overflows
	TIMSK1 |= (1 << TOIE1);
	
	DDRD |= (1 << PD5);

	// habilita interrupções
	sei();

	DDRC &= ~(1 << BUTTON); // PC2 entrada
    // PORTC |= (1 << BUTTON_CENTER) | (1 << BUTTON_DOWN) | (1 << BUTTON_UP) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT); // Ativar pull up

	hd44780_init();
	drawDisplay();
	
	LCD_RESP_DDR = 0xff;

	LCD_RESP_PORT = 0xff;

	print("password: ");
	printint(password);
	print("\n");

	while(1){
		if (changeDisplay) drawDisplay();

		pos = readJoystick();
		if (pos != CENTER) {
			printint(pos);

			while (readJoystick() != CENTER) {
				pos = readJoystick();
				_delay_ms(1);
			}

			switch (pos){
				case RIGHT:
					if (cursorSelecionado == 3) { 
						cursorSelecionado = 0;
					} else {
						cursorSelecionado += 1;
					}
					changeDisplay = 1;
					break;
				case LEFT:
					if (cursorSelecionado == 0) { 
						cursorSelecionado = 3;
					} else {
						cursorSelecionado -= 1;
					}
					changeDisplay = 1;
					break;
				case UP:
					// int p = 10 ^ (4-cursorSelecionado);
					// n / 1000
					// n / 100
					// n / 10
					// n % 10
					break;
				default:
					break;
			}
		}
	}
}
