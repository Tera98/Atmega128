/*
 * GccApplication2.c
 *
 * Created: 2022-10-12(수) 오전 12:20:28
 * Author : shyragon
 */ 

//6.6
/*
#include <stdlib.h>
#include <avr/io.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>

#define STOP_SPEED 0
#define LOW_SPEED 77
#define MID_SPEED 153
#define HIGH_SPEED 230

#define MOTOR_CW 0xb0
#define MOTOR_CCW 0x70
#define MOTOR_BRAKE 0xd0
#define MOTOR_STOP 0x30
#define MOTOR_STANDBY 0x00
volatile int count=0;
volatile int value=0;
int motor_speed[4] = {STOP_SPEED, LOW_SPEED, MID_SPEED, HIGH_SPEED};

int main(void)
{
	DDRA = 0x0f;
	DDRB = 0xf0;
	PORTB = MOTOR_STANDBY;
	TCCR2 = 0x6b;
	PORTB = MOTOR_CW;
	while (1)
	{
		value = rand() % 4;
		OCR2 = motor_speed[value];
		PORTA = 1 << value;
		value = (rand() % 4) + 3;
		_delay_ms(value*1000);
	}
}
*/

//DIY-3
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>

#define STOP_SPEED 0
#define LOW_SPEED 77
#define MID_SPEED 153
#define HIGH_SPEED 230

#define MOTOR_CW 0xb0
#define MOTOR_CCW 0x70
#define MOTOR_BRAKE 0xd0
#define MOTOR_STOP 0x30
#define MOTOR_STANDBY 0x00

#define STOP 0
#define GO 1
#define SLOW 1
#define MEDIUM 2
#define FAST 3

#define  OFF	0
#define  ON		1		

volatile int state_sw1 = STOP;
volatile int state_sw2 = SLOW;
volatile int state, i, j;
volatile int msec = 0;
volatile int sec = 1000;

int motor_speed[4] = {STOP_SPEED, LOW_SPEED, MID_SPEED, HIGH_SPEED};
void TIMER_sec(int target);

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6;
	msec++;
}

ISR(INT4_vect)
{
	_delay_ms(100);
	EIFR = 1 << 4;
	
	if((PINE & 0x10) == 0x10)
		return;
		
	if(state_sw1 == GO)
	{
		PORTB = MOTOR_STANDBY;
		state_sw1 = STOP;
	}
	else
	{
		PORTB = MOTOR_CW;
		state_sw1 = GO;
	}
}

ISR(INT5_vect)
{
	_delay_ms(100);
	EIFR = 1 << 5;
	
	if((PINE & 0x20) == 0x20)
	return;
	
	if(state_sw2 == SLOW)
		state_sw2 = MEDIUM;
	else if(state_sw2 == MEDIUM)
		state_sw2 = FAST;
	else
		state_sw2 = SLOW;
}

ISR(INT6_vect)
{
	_delay_ms(100);
	EIFR = 1 << 6;
	
	if((PINE & 0x40) == 0x40)
	return;
	
	msec = 0;
	sec = 0;
	
}
	
int main(void)
{
	DDRA = 0x0f;
	DDRB = 0xf0;
	DDRE = 0x00;
	PORTB = MOTOR_STANDBY;
	
	
	TIMSK = 0x01;
	TCCR0 = 0x04;
	TCCR2 = 0x6b;
	
	sei();
	EICRB = 0x2a;
	EIMSK = 0x70;
	
	
	
	while (1)
	{
		OCR2 = motor_speed[state_sw2];
		
		if(state_sw1 == GO)
			PORTA = 1 << state_sw2;
		else
			PORTA = 0x01;	
			
		TIMER_sec(5);
	}
}

void TIMER_sec(int target)
{
	if(msec == 1000)
	{
		sec++;
		msec = 0;
	}
	
	if(sec == target)	
	{
		PORTB = MOTOR_STANDBY;
		state_sw1 = STOP;
		sec++;
	}
	
	if(sec == target + 5)  // sec의 상한선 지정
		sec = target + 1;
} 
*/
