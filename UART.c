/*
 * UART.c
 *
 * Created: 2022-11-01(화) 오후 6:31:17
 *  Author: shyragon
 */ 


// 예제
/*
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

void init_uart0() 
{
	UCSR0B = 0x18; 
	UCSR0C = 0x06; 
	UBRR0H = 0;
	UBRR0L = 103; 
}
void putchar0(char c)
{
	while(!(UCSR0A & (1<<UDRE0))) 
	;
	UDR0 = c;
}
char getchar0()
{
	while (!(UCSR0A & (1<<RXC0)))
	; 
	return(UDR0);
}
int main()
{
	char value;
	init_uart0(); 
	
	while(1)
	{
		value = getchar0();
		if ((value >= 'A') && (value <= 'Z')) 
		{
			value = value + 4;
			if (value > 'Z') 
				value = value - 'Z' - 1 + 'A'; 
			putchar0(value);
		}
		else if ((value >= 'a') && (value <= 'z'))
		{
			value = value + 4;
			if (value > 'z')
				value = value - 'z' - 1 + 'a';
			putchar0(value);
		}
		else if (value == '\r')
			putchar0('\n');
		else 
			putchar0(value);
	}
}
*/

// 실습 2
/*
#include <avr/io.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>

void init_uart0() 
{
	UCSR0B = 0x18; 
	UCSR0C = 0x06; 
	UBRR0H = 0;
	UBRR0L = 103; 
}
void putchar0(char c)
{
	while(!(UCSR0A & (1<<UDRE0))) 
	;
	UDR0 = c;
}
char getchar0()
{
	while (!(UCSR0A & (1<<RXC0)))
	; 
	return(UDR0);
}
void puts0(char *ptr) 
{
	while(1)
	{
		if (*ptr != NULL) 
			putchar0(*ptr++);
		else
			return;
	}
}
int main()
{
	char value;
	int i,vaule_int;
	char st[20];
	
	init_uart0(); 
	
	while(1)
	{
		value = getchar0();
		

		for(i=1;i<10;i++)
		{
			vaule_int = value - '0';
			itoa(vaule_int*i,st,10);
			
			putchar0(value);
			putchar0('*');
			putchar0((char)(i+48));
			putchar0('=');
			puts0(st);
			putchar0('\n');
			putchar0('\r');
		}
	}
}
*/
 
// 실습 3
/*
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

unsigned char digit[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[4] = {0x08,0x04,0x02,0x01};

void display_fnd(int num);
int ppow(int a, int b);

volatile char data;
volatile int st[20]={0};
volatile int i = 0, j = 0, count = 0, count_fnd = 0;


void init_uart0() 
{
	UCSR0B = 0x98; 
	UCSR0C = 0x06; 
	UBRR0H = 0;
	UBRR0L = 103; 
	sei();
}

ISR(USART0_RX_vect)
{
	data = UDR0;
	
	UCSR0B |= (1<<UDRIE0);

	
	if(data == '\r')
	{
		for(j=0;j<i;j++)
		{
			count += st[i-j-1]*ppow(10,j);
		}
		
		i=0;
		
		for(j=0;j<20;j++)
		{
			st[j]=0;
		}
		
		count_fnd = count;
		count = 0;
		return;
	}
	
	st[i++] = data - 48;
	
}

ISR(USART0_UDRE_vect)
{
	UCSR0B &= ~(1<<UDRIE0);
}

int main()
{
	init_uart0(); 

	DDRC = 0xff;
	DDRG = 0x0f;
	
	while(1)	
		display_fnd(count_fnd);
}
void display_fnd(int num) // fnd 출력
{
	int i,fnd[4];
	
	fnd[0] = (num / 1000) % 10;
	fnd[1] = (num / 100) % 10;
	fnd[2] = (num / 10) % 10;
	fnd[3] = num % 10;
	
	for(i=0;i<4;i++)
	{
		PORTC = digit[fnd[i]];
		PORTG = fnd_sel[i];
		
		if(i%2)
			_delay_ms(2);
		else
			_delay_ms(3);
	}
}
int ppow(int a, int b) // 제곱함수
{
	int c;
	int d=1;
	for(c=0;c<b;c++)
		d*=a;
	return d;
}
*/

