/*
 * ychen133_simon.c
 *
 * Created: 8/21/2017 
 * Author1 : Yu-Che Cheng <ychen133@ucr.edu>
 * Lab Sec: B21
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "io.c"

// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

const unsigned char MAX_ROUNDS = 9;
unsigned char array[9];
unsigned char rounds = 0;
unsigned char index = 0;
unsigned char output = 0;
unsigned char input;
unsigned char wrong = 0;
unsigned char cnt = 0;
unsigned char seed = 0;

enum testStates {INIT, MENU, FLASH_OFF, FLASH_ON, WAITING, CHECK, LOSE, WIN} curr_state;

void tick2()
{
	input = ~PINA & 0x0F;
	
	//Transitions
	switch(curr_state)
	{
		case INIT:
		{
			//rounds = 1;
			//curr_state = FLASH_OFF;
			//array[0] = (rand() % 4);
			LCD_DisplayString(1, "Welcome! Press a button to start");
			curr_state = MENU;
			break;
		}
		case MENU:
		{
			if(input != 0)
			{
				srand(seed);
				seed = 0;
				rounds = 1;
				array[0] = (rand() % 4);
				cnt = 0;
				LCD_DisplayString(1, "Flashing");
				curr_state = FLASH_OFF;
			}
			break;
		}
		case FLASH_OFF:
		{
			if (index < rounds && cnt >= 5)
			{
				cnt = 0;
				curr_state = FLASH_ON;
			}
			break;
		}
		case FLASH_ON:
		{
			if (index >= rounds && cnt >= 5)
			{
				cnt = 0;
				index = 0;
				LCD_DisplayString(1, "Waiting");
				curr_state = WAITING;
			}
			else if (index < rounds && cnt >= 5)
			{
				//index++;
				cnt = 0;
				curr_state = FLASH_OFF;
			}
			break;
		}
		case WAITING:
		{
			if (input != 0x00 && index < rounds)
			{
				curr_state = CHECK;
			}
			else if (index >= rounds && rounds < MAX_ROUNDS)
			{
				index = 0;
				rounds++;
				array[rounds - 1] = rand() % 4;
				LCD_DisplayString(1, "Flashing");
				curr_state = FLASH_OFF;
			}
			else if (index >= rounds && rounds >= MAX_ROUNDS)
			{
				LCD_DisplayString(1, "YOU WIN!        Rounds done: 9");
				curr_state = WIN;
			}
			break;
		}
		case CHECK:
		{
			if (input == 0x00 && wrong == 0)
			{
				index++;
				curr_state = WAITING;
			}
			else if (wrong == 1 && input == 0x00)
			{
				LCD_DisplayString(1, "YOU LOSE!       Rounds done: ");
				LCD_WriteData((rounds - 1) + '0');
				curr_state = LOSE;	
			}
			break;
		}
		case LOSE:
		{
			if(input != 0x00)
			{
				LCD_DisplayString(1, "Press a button  to start");
				curr_state = MENU;
			}
			break;
		}
		case WIN:
		{
			if (input != 0x00)
			{
				LCD_DisplayString(1, "Press a button  to start");
				curr_state = MENU;
			}
			break;
		}
	}
	//Actions
	switch(curr_state)
	{
		case INIT:
		{
			seed = 0;
			break;
		}
		case MENU:
		{
			rounds = 0;
			index = 0;
			output = 0x00;
			wrong = 0;
			seed++;
			break;
		}
		case FLASH_OFF:
		{
			PORTB = 0x00;
			cnt++;
			break;
		}
		case FLASH_ON:
		{
			cnt++;
			//index++;
			if (array[index] == 0)
			{
				output = 0x01;
			}
			else if (array[index] == 1)
			{
				output = 0x02;
			}
			else if(array[index] == 2)
			{
				output = 0x04;
			}
			else if(array[index] == 3)
			{
				output = 0x08;
			}
			PORTB = output;
			if(cnt >= 5)
			{
				index++;
			}
			break;
		}
		case WAITING:
		{
			//PORTB = 0x00;
			PORTB = input;
			break;
		}
		case CHECK:
		{
			PORTB = input;
			if (input != (0x01 << (array[index])))
			{
				wrong = 1;
			}
			break;
		}
		case LOSE:
		{
			PORTB = 0x00;
			break;
		}
		case WIN:
		{
			PORTB = 0x00;
			break;
		}
	}
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	//DDRA = 0xFF; PORTA = 0x00;
	//DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	TimerSet(100);
	//TimerSet(500);
	TimerOn();
	LCD_init();
	curr_state = INIT;
    while (1) 
    {
		while(!TimerFlag);
		tick2();
		TimerFlag = 0;
    }
}

