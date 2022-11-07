﻿// 예제
/*
#include <avr/io.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#define S_LEVEL_8 1024*3.2/5
#define S_LEVEL_7 1024*2.8/5 
#define S_LEVEL_6 1024*2.4/5 
#define S_LEVEL_5 1024*2.0/5
#define S_LEVEL_4 1024*1.6/5
#define S_LEVEL_3 1024*1.2/5
#define S_LEVEL_2 1024*0.8/5
#define S_LEVEL_1 1024*0.4/5

void init_adc();
unsigned short read_adc();
void show_adc_led(unsigned short data);

int main()
{
	unsigned short value;
	DDRA = 0xff;
	init_adc();
	while(1)
	{
		value = read_adc(); 
		show_adc_led(value); 
		_delay_ms(1000); 
	}
}

void init_adc()
{
	ADMUX = 0x41; 
	ADCSRA = 0x87;
}

unsigned short read_adc()
{
	unsigned char adc_low, adc_high;
	unsigned short value;
	ADCSRA |= 0x40;
	while ((ADCSRA & 0x10) != 0x10) 
	;
	adc_low = ADCL;
	adc_high = ADCH; 
	value = (adc_high << 8) | adc_low;
	return value;
}

void show_adc_led(unsigned short value)
{
	if (value >= S_LEVEL_8) PORTA = 0xff; 
	else if (value >= S_LEVEL_7) PORTA = 0x7f;
	else if (value >= S_LEVEL_6) PORTA = 0x3f;
	else if (value >= S_LEVEL_5) PORTA = 0x1f;
	else if (value >= S_LEVEL_4) PORTA = 0x0f; 
	else if (value >= S_LEVEL_3) PORTA = 0x07;
	else if (value >= S_LEVEL_2) PORTA = 0x03; 
	else if (value >= S_LEVEL_1) PORTA = 0x01;
	else PORTA = 0x00;
}
*/

// 실습 2
/*
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#define CDS_10 341 
#define CDS_20 450 
#define CDS_50 550 
#define CDS_80 650 
#define CDS_100 731

#define MOTOR_CW 0xb0
#define MOTOR_CCW 0x70
#define MOTOR_STANDBY 0x00



#define UP 0
#define DOWN 1
#define STOP 0
#define GO 100

void init_adc();
unsigned short read_adc(); 
void adc_motor(unsigned short data);

volatile int state = DOWN;

int main()
{
	unsigned short value;	
	DDRB = 0xf0;
	DDRF = 0x00;
	
	PORTB = 0x00;
	TCCR2 = 0x6b;
	sei();
	OCR2 = GO;
	
	init_adc();
	
	while(1)
	{	
		value = read_adc();
		adc_motor(value);
	}
	
}
void init_adc()
{
	ADMUX = 0x40;
	ADCSRA = 0x87;
}

unsigned short read_adc()
{
	unsigned char adc_low, adc_high;
	unsigned short value;
	ADCSRA |= 0x40; 
	while ((ADCSRA & 0x10) != 0x10);
	adc_low = ADCL;
	adc_high = ADCH; 
	value = (adc_high << 8) | adc_low;
	return value;
}

void adc_motor(unsigned short value)
{
	if(state == UP )
	{
		if (value <= CDS_10)
		{
			state = UP;
		    PORTB = MOTOR_STANDBY;
		}
		else if (value >= CDS_20)
		{
			state = DOWN;
		    PORTB = MOTOR_CCW;
		    _delay_ms(5000);		
		}
	}
	else if(state == DOWN)
	{	
		if (value <= CDS_10)
		{
			state = UP;
			PORTB = MOTOR_CW;	
		    _delay_ms(5000);
		}
		else if (value >= CDS_20)
		{
			state = DOWN;
		    PORTB = MOTOR_STANDBY;
			
		}
	}
}
*/

// 실습 3
// 실습3
/*
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#define OFF 0
#define ON 1

void init_adc();
unsigned short read_adc();
volatile int i=0,j=0;
volatile int state = OFF;
volatile int sound = OFF;
char f_table[] = {255, 114};

ISR(TIMER0_OVF_vect)
{
	TCNT0 = f_table[sound];
	
	if (state == OFF)
	{
		PORTB |= 1 << 4;
		state = ON;
	}
	else
	{
		PORTB &= ~(1 << 4);
		state = OFF;
	}
}

int main()
{
	unsigned short value;

	DDRB |= 0x10;
	TCCR0 = 0x03;
	TIMSK = 0x01;
	TCNT0 = f_table[sound];
	sei();
	
	init_adc();
	
	while(1)
	{
		value = read_adc();
		
		
		i+=1;
		
		if(value > 500)
		{
			j+=1;
			
		}
		
		if(i==5 && j>=3)
		{
			sound = ON;
			i=0;
			j=0;
		}
		
		if(i>5)
		{
			i=0;
			j=0;
		}
		
		if(sound == ON)
		{
			_delay_ms(5000);
			sound = OFF;
		}
		
		_delay_ms(1000);
		
	}
}

void init_adc()
{
	ADMUX = 0x41;
	ADCSRA = 0x87;
}

unsigned short read_adc()
{
	unsigned char adc_low, adc_high;
	unsigned short value;
	ADCSRA |= 0x40;
	while ((ADCSRA & 0x10) != 0x10);
	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	return value;
}
*/
//실습 3 연속적인 5초

/*
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#define OFF 0
#define ON 1
#define TIME1 0
#define TIME2 1
#define TIME3 2
#define TIME4 3
#define TIME5 4


void init_adc();
unsigned short read_adc();
volatile int i=0,j=0;
volatile int state = OFF;
volatile int sound = OFF;
volatile int TIME_state = TIME1;

char f_table[] = {255, 114};
	
void adc_sound(unsigned short value);
void TIME_state_changer();

ISR(TIMER0_OVF_vect)
{
	TCNT0 = f_table[sound];
	
	if (state == OFF)
	{
		PORTB |= 1 << 4;
		state = ON;
	}
	else
	{
		PORTB &= ~(1 << 4);
		state = OFF;
	}
}

int main()
{
	unsigned short value;

	DDRB |= 0x10;
	TCCR0 = 0x03;
	TIMSK = 0x01;
	TCNT0 = f_table[sound];
	sei();
	
	init_adc();
	
	while(1)
	{
		value = read_adc();
		adc_sound(value);
		if(sound == ON)
		{
			_delay_ms(5000);
			sound = OFF;
		}
		_delay_ms(1000);
		TIME_state_changer();
	}
}

void init_adc()
{
	ADMUX = 0x41;
	ADCSRA = 0x87;
}

unsigned short read_adc()
{
	unsigned char adc_low, adc_high;
	unsigned short value;
	ADCSRA |= 0x40;
	while ((ADCSRA & 0x10) != 0x10);
	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	return value;
}

void adc_sound(unsigned short value)
{
	int i,sum=0;
	int signal[5]={0,0,0,0,0};
	
	
	if(TIME_state == TIME1 && value > 500)
		signal[TIME_state]=1;
	else if(TIME_state == TIME2 && value > 500)
		signal[TIME_state]=1;
	else if(TIME_state == TIME3 && value > 500)
		signal[TIME_state]=1;
	else if(TIME_state == TIME4 && value > 500)
		signal[TIME_state]=1;
	else if(TIME_state == TIME5 && value > 500)
		signal[TIME_state]=1;

	for(i=0;i<5;i++)
		sum+=signal[i];
	
	if(sum >= 3)
		sound = ON;
}
void TIME_state_changer()
{
	if(TIME_state==TIME1)
		TIME_state=TIME2;
	else if(TIME_state==TIME2)
		TIME_state=TIME3;
	else if(TIME_state==TIME3)
		TIME_state=TIME4;
	else if(TIME_state==TIME4)
		TIME_state=TIME5;
	else if(TIME_state==TIME5)
		TIME_state=TIME1;
}	
*/