


// 예제

/*
#include <avr/io.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>
#define ON 1
#define OFF 0

#define STOP_SPEED 00 
#define MID_SPEED 153 

#define MOTOR_CW 0xb0 
#define MOTOR_Standby 0x00

void putchar1(char c) 
{
	while(!(UCSR1A & (1<<UDRE1))) ; 
	UDR1 = c; 
}
char available() 
{
	if (UCSR1A & (1<<RXC1))
	return(1);
	else return(0); 
}
char getchar1() 
{
	return(UDR1);
}
void buzzer(int hz, int count) 
{
	int i, ms, us;
	ms = 1000/(2*hz); 
	us = (1000.0/(2*hz) - 1000/(2*hz)) * 1000;
	
	for(i=0; i<count ;i++)
	{
		PORTE |= 1 << 4;
		_delay_ms(ms); 
		_delay_us(us); 
		PORTE &= ~(1 << 4); 
		_delay_ms(ms); 
		_delay_us(us); 
	}
}
void BUZZER_Control()
{
	int i;
	for(i=0; i<2; i++) 
	{
		buzzer(480, 12); 
		buzzer(320, 8); 
	}
}
void init_uart1() 
{
	UCSR1B = 0x18;
	UCSR1C = 0x06; 
	UBRR1H = 0;
	UBRR1L = 8; 
}
int main()
{
	char c;
	int st_light=OFF, st_buzz=OFF, st_fan=OFF; 
	DDRA = 0x0f; 
	DDRB = 0xf0; 
	DDRE = 0x10; 
	PORTB = MOTOR_Standby;
	init_uart1(); 
	
	while(1)
	{
		if (!available()) 
		{
			if (st_buzz == ON) 
			BUZZER_Control();
			continue; 
		}
		c = getchar1(); 
		putchar1(c);
		switch(c) 
		{
			case '1' : PORTA = 0x0f; st_light = ON; break; 
			case '2' : st_buzz = ON; break; 
			case '3' : PORTB = MOTOR_CW; st_fan = ON; break; 
			case '4' : PORTA = 0x00; st_light = OFF; break;
			case '5' : st_buzz = OFF; break;
			case '6' : PORTB = MOTOR_Standby; st_fan = OFF; break;
			case '7' :
			putchar1('\n'); 
			if (st_light)
			putchar1('O'); 
			else
			putchar1('X');
			if (st_buzz) 
			putchar1('O'); 
			else
			putchar1('X'); 
			if (st_fan) 
			putchar1('O'); 
			else
			putchar1('X'); 
			break;
			default : break;
		}
	}
}
*/


// 실습 2
/*
#include <avr/io.h>
#define F_CPU 16000000UL
#define F_SCK 40000UL
#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>
#include <avr/interrupt.h>

#define ON 1
#define OFF 0
#define NULL 0x00

#define		DO		0
#define		RE		1
#define		MI		2
#define		FA		3
#define		SOL		4
#define		RA		5
#define		SI		6
#define		DDO		7

volatile int state, tone = 255, i = 0;
volatile char array[12] = "000000000000";
volatile char data;
char f_table[] = {17, 43, 66, 77, 97, 114, 129, 136};

ISR(TIMER0_OVF_vect)
{
	TCNT0 = f_table[tone];
	
	if (state == OFF)
	{
		PORTE |= 1 << 4;
		state = ON;
	}
	
	else
	{
		PORTE &= ~(1 << 4);
		state = OFF;
	}
}

ISR(USART1_RX_vect)
{
	
	data = UDR1;
	
	UCSR1B |= (1<<UDRIE1);
	
	
	if(i<8)
	{
		array[i++] = data;
	}
	
	putchar1(data); // echo back

}
ISR(USART1_UDRE_vect)
{
	UCSR1B &= ~(1<<UDRIE1);
}
void putchar1(char c)
{
	while(!(UCSR1A & (1<<UDRE1))) ;
	UDR1 = c;
}
void init_uart1()
{
	UCSR1B = 0x98;
	UCSR1C = 0x06;
	UBRR1H = 0;
	UBRR1L = 8;
}
int main()
{

	DDRE = 0x10;
	
	TCCR0 = 0x03;
	TIMSK = 0x01;
	sei();
	
	init_uart1();

	TCNT0 = 255;
	
	while(1)
	{
		if(data==array[0]){
			tone = DO;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[1]){
			tone = RE;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[2]){
			tone = MI;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[3]){
			tone = FA;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[4]){
			tone = SOL;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[5]){
			tone = RA;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[6]){
			tone = SI;  _delay_ms(1000); tone = 255;
		}
		else if(data==array[7]){
			tone = DDO;  _delay_ms(1000); tone = 255;
		}
		
	}
}
*/
