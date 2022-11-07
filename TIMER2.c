/*
 * TIMER2.c
 *
 * Created: 2022-10-01 오후 11:56:37
 *  Author: USER
 */ 

//5.7
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#define		DO		0
#define		RE		1
#define		MI		2
#define		FA		3
#define		SOL		4
#define		RA		5
#define		SI		6
#define		DDO		7
#define		REST	8
#define		EOS		-1
#define		ON		0
#define		OFF		1

#define		N2		1250
#define		N4		625
#define		N8N16	469
#define		N8		313
#define		N16		156
#define		R		1

volatile int state, tone;
char f_table[] = {17, 43, 66, 77, 97, 114, 117, 137, 255};
	
int song[] = {SOL, MI, REST, MI, SOL, MI, DO, RE, MI, RE, DO, MI, SOL, DDO,
			  SOL, DDO, SOL, DDO, SOL, MI, SOL, RE, FA, MI, RE, DO, EOS};
			  
int time[] = {N4, N8, R, N8, N8, N8, N4, N4, N8, N8, N8, N8, N4,
			N8N16, N16, N8, N8, N8, N8, N4, N4, N8, N8, N8, N8, N4};
			
char LED[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};
	
ISR(TIMER0_OVF_vect)
{
	TCNT0 = f_table[tone];
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
	int i=0;
	DDRA = 0xff;
	DDRB |= 0x10;
	TCCR0 = 0x03;
	TIMSK = 0x01; 
	TCNT0 = f_table[song[i]];
	sei();
	while(1)
	{
		i=0;
		do 
		{
			tone = song[i];
			PORTA = LED[tone];
			_delay_ms(time[i++]);
		}
		while(tone != EOS);
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

#define		DO		0
#define		RE		1
#define		MI		2
#define		FA		3
#define		SOL		4
#define		RA		5
#define		SI		6
#define		DDO		7
#define		REST	8
#define		EOS		-1

#define		ON		0
#define		OFF		1
#define		First_song		1
#define		Second_song		2
#define		Third_song		3

#define		N2N4	1875
#define		N2		1250
#define		N4N8	938
#define		N4		625
#define		N8N16	469
#define		N8		313
#define		N16		156
#define		R		1

volatile int state, tone;
volatile int song = First_song;
volatile int i,j,k =0;


char f_table[] = {17, 43, 66, 77, 97, 114, 
117, 137,255};

int song1[] = {SOL, MI, REST, MI, SOL, MI,
 DO, RE, MI, RE, DO, MI, SOL, DDO, SOL, DDO, SOL, DDO, SOL, MI, SOL, RE, FA, MI, RE, DO, EOS};

int song2[] = {MI, SOL, REST, SOL, FA,  MI,
				 SOL, REST, SOL, FA, MI, SOL, FA,
				RE, FA, REST, FA, MI, RE, FA, REST, FA, MI, RE, FA, MI,
				DO, REST, DO, MI, SOL, DDO, RA, SOL, REST, SOL, RA, SOL,
				MI, SOL, REST, SOL, FA, MI, SOL, REST, SOL, FA, MI, RE, DO, EOS};

int song3[] = {SOL, REST, SOL, RA, REST, 
RA, SOL, REST, SOL, MI, SOL, REST, SOL, MI, REST, MI, RE, SOL, REST, SOL, RA, REST, RA, SOL, REST, SOL, MI, SOL, MI, RE, MI, DO, EOS};


int time1[] = {N4, N8, R, N8, N8, N8, N4,
 N4, N8, N8, N8, N8, N4, N8N16, N16, N8, N8, N8, N8, N4, N4, N8, N8, N8, N8, N4};

int time2[] = {N8, N8, R, N8, N8, N8, N8,
 R, N8, N8, N4, N4, N2,
				N8, N8, R, N8, N8, N8, N8, R, N8, N8, N4, N4, N2,
				N8, R, N8, N8, N8, N4N8, N8, N8, R, N8, N4, N2,
				N8, N8, R, N8, N8, N8, N8, R, N8, N8, N4, N4, N2};

int time3[] = {N4, R, N4, N4, R, N4, N4,
 R, N4, N2, N4, R, N4, N4, R, N4, N2N4, N4, R, N4, N4, R, N4, N4, R, N4, N2, N4, N4, N4, N4, N2N4};

char LED[] = {0x01, 0x02, 0x04, 0x08, 
				0x10, 0x20, 0x40, 0x80, 0x00};

void set_buzzer();

ISR(TIMER0_OVF_vect)
{
	TCNT0 = f_table[tone];
	
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

ISR(INT4_vect)
{
	_delay_ms(100);
	EIFR = 1 << 4;
	
	if((PINE & 0x10) == 0x10)
		return;
	
	if(song == First_song)
		{
			song = Second_song;
			j=0;
		}
	else if(song == Second_song)
		{
			song = Third_song;
			k=0;
		}
	else
		{
			song = First_song;
			i=0;
		}
}
int main()
{
	set_buzzer();
	
	while(1)
	{
		while(1)
		{
			if(song == First_song)
			{
				tone = song1[i];
				PORTA = LED[tone];
				_delay_ms(time1[i++]);
				
				if(song1[i] == EOS)
					break;
			}
			else if(song == Second_song)
			{
				tone = song2[j];
				PORTA = LED[tone];
				_delay_ms(time2[j++]);
				
				if(song2[j] == EOS)
					break;
			}
			else
			{
				tone = song3[k];
				PORTA = LED[tone];
				_delay_ms(time3[k++]);
				
				if(song3[k] == EOS)
					break;
			}
		}
		i=0; j=0; k=0;
	}
}

void set_buzzer()
{
		
		DDRA = 0xff;
		DDRB |= 0x10;
		TCCR0 = 0x03;
		TIMSK = 0x01;
		TCNT0 = f_table[song1[i]];
		sei();
		
		EICRB = 0x02;
		DDRE = 0x00;
		EIMSK = 0x10;
}
*/
//DIY-3 do while
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>

#define		DO		0
#define		RE		1
#define		MI		2
#define		FA		3
#define		SOL		4
#define		RA		5
#define		SI		6
#define		DDO		7
#define		REST	8
#define		EOS		-1

#define		ON		0
#define		OFF		1
#define		First_song		1
#define		Second_song		2
#define		Third_song		3

#define		N2N4	1875
#define		N2		1250
#define		N4		625
#define		N4N8	938
#define		N8N16	469
#define		N8		313
#define		N16		156
#define		R		1

volatile int state, tone;
volatile int song = First_song;
volatile int i=0;
volatile int j=0;
volatile int k=0;


char f_table[] = {17, 43, 66, 77, 97, 114, 
117, 137, 255};

int song1[] = {SOL, MI, REST, MI, SOL, MI,
 DO, RE, MI, RE, DO, MI, SOL, DDO, SOL, DDO, SOL, DDO, SOL, MI, SOL, RE, FA, MI, RE, DO, EOS};

int song2[] = {MI, SOL, REST, SOL, FA,  MI,
				 SOL, REST, SOL, FA, MI, SOL, FA,
				RE, FA, REST, FA, MI, RE, FA, REST, FA, MI, RE, FA, MI,
				DO, REST, DO, MI, SOL, DDO, RA, SOL, REST, SOL, RA, SOL,
				MI, SOL, REST, SOL, FA, MI, SOL, REST, SOL, FA, MI, RE, DO, EOS};

int song3[] = {SOL, REST, SOL, RA, REST, 
RA, SOL, REST, SOL, MI, SOL, REST, SOL, MI, REST, MI, RE, SOL, REST, SOL, RA, REST, RA, SOL, REST, SOL, MI, SOL, MI, RE, MI, DO, EOS};


int time1[] = {N4, N8, R, N8, N8, N8, N4,
 N4, N8, N8, N8, N8, N4, N8N16, N16, N8, N8, N8, N8, N4, N4, N8, N8, N8, N8, N4};

int time2[] = {N8, N8, R, N8, N8, N8, N8,
 R, N8, N8, N4, N4, N2,
				N8, N8, R, N8, N8, N8, N8, R, N8, N8, N4, N4, N2,
				N8, R, N8, N8, N8, N4N8, N8, N8, R, N8, N4, N2,
				N8, N8, R, N8, N8, N8, N8, R, N8, N8, N4, N4, N2};

int time3[] = {N4, R, N4, N4, R, N4, N4,
 R, N4, N2, N4, R, N4, N4, R, N4, N2N4, N4, R, N4, N4, R, N4, N4, R, N4, N2, N4, N4, N4, N4, N2N4};

char LED[] = {0x01, 0x02, 0x04, 0x08, 
				0x10, 0x20, 0x40, 0x80, 0x00};

void set_buzzer();

ISR(TIMER0_OVF_vect)
{
	TCNT0 = f_table[tone];
	
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

ISR(INT4_vect)
{
	_delay_ms(100);
	EIFR = 1 << 4;
	
	if((PINE & 0x10) == 0x10)
		return;
	
	if(song == First_song)
		{
			song = Second_song;
			j=0;
		}
	else if(song == Second_song)
		{
			song = Third_song;
			k=0;
		}
	else
		{
			song = First_song;
			i=0;
		}
}
int main()
{
	set_buzzer();
	
	
	while(1)
	{	
		do 
		{
			tone = song1[i];
			PORTA = LED[tone];
			_delay_ms(time1[i++]);
			if(song1[i]==EOS)
				i=0;
		} while (song == First_song);
		do
		{
			tone = song2[j];
			PORTA = LED[tone];
			_delay_ms(time2[j++]);
		} while (song2[j+1] != EOS && song == Second_song);
		do
		{
			tone = song3[k];
			PORTA = LED[tone];
			_delay_ms(time3[k++]);
		} while (song3[k] != EOS && song == Third_song);
		
		i=0;j=0;k=0;
	}
	
}
void set_buzzer()
{
		
		DDRA = 0xff;
		DDRB |= 0x10;
		TCCR0 = 0x03;
		TIMSK = 0x01;
		TCNT0 = f_table[song1[i]];
		sei();
		
		EICRB = 0x02;
		DDRE = 0x00;
		EIMSK = 0x10;
}
*/