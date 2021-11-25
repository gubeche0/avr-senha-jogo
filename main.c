#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
// #include <math.h>
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
char *tentativa = "0000";
int16_t tempoTentativaAtual;
int16_t tentativas = 0;
int n;

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

void resetTempo() {
	tempoTentativaAtual = 30;
}

void drawDisplay() {
	hd44780_clear();
	// hd44780_puts("Tempo: ");
	hd44780_puts(("Tempo: "));
	char time[2];
	sprintf(time, "%d", tempoTentativaAtual); 
	hd44780_puts(time);
	hd44780_gotoxy(14,0);
	// hd44780_puts(("T: 2"));
	char ten[2];
	sprintf(ten, "%d", tentativas); 
	hd44780_puts(ten);

	hd44780_gotoxy(4,1);
	// char tent[5];
	// sprintf(tent, "%04d", tentativa); 
	// hd44780_puts(tent);
	hd44780_puts(tentativa);
	hd44780_gotoxy(cursorSelecionado + 4,1);

	changeDisplay = 0;
}

void verifica_senha() {
	
}

void startGame() {
	password = 1092;
	tentativas = 0;
	cursorSelecionado = 0;
	tentativa = "0000";
	LCD_RESP_PORT = 0x00;

	print("password: ");
	printint(password);
	print("\n");

	resetTempo();
}

int perdeu() {
	if (tempoTentativaAtual <= 0 || tentativas > 10) {
		return 1;
	}

	return 0;
}

void gamerOver() {
	hd44780_clear();
	hd44780_puts(("Voce perdeu!"));
	_delay_ms(3000);

}

void winGame() {
	hd44780_clear();
	hd44780_puts(("Voce Ganhou!"));
	for (int8_t i = 0; i < 3; i++) {
		LCD_RESP_PORT = 0xff;
		_delay_ms(500);
		LCD_RESP_PORT = 0x00;
		_delay_ms(500);
	}
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
	
	LCD_RESP_DDR = 0xff;

	startGame();
	drawDisplay();
	while(1){
		if (perdeu()) {
			gamerOver();
			startGame();
		} else {
			if (changeDisplay) drawDisplay();

			if (PINC & (1 << BUTTON)) { // Le BUTTON
				// print("Verifica se acertou");
				tentativas++;
				changeDisplay = 1;
				verifica_senha();

				while (PINC & (1 << BUTTON)) { // Espera soltar o botão
					_delay_ms(10);
				}

				
				resetTempo();
			}

			pos = readJoystick();
			if (pos != CENTER) {
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
						n = tentativa[cursorSelecionado] - '0';
						if (n == 9) {
							n = 0;
						} else {
							n++;
						}
						tentativa[cursorSelecionado] = n + '0';
						changeDisplay = 1;
						break;
					case DOWN:
						n = tentativa[cursorSelecionado] - '0';
						if (n == 0) {
							n = 9;
						} else {
							n--;
						}
						tentativa[cursorSelecionado] = n + '0';
						changeDisplay = 1;
						changeDisplay = 1;
					default:
						break;
				}
			}
		}
		
	}
}
