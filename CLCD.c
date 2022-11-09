/*
 * CLCD.c
 *
 * Created: 2022-11-07(월) 오후 11:25:30
 *  Author: shyragon
 */ 

// 예제
/*
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define NULL 0x00

#define BIT4_LINE2_DOT58 0x28
#define DISPON_CUROFF_BLKOFF 0x0C
#define DISPOFF_CUROFF_BLKOFF 0x08
#define INC_NOSHIFT 0x06 
#define DISPCLEAR 0x01 
#define CUR1LINE 0x80 
#define CUR2LINE 0xC0 
#define CURHOME 0x02 
#define E_BIT 0x04 
#define RW_BIT 0x03
#define RS_BIT 0x02 

void CLCD_cmd(char); 
void CLCD_data(char); 
void CLCD_puts(char *); 

char motto1[] = "Slow & Steady"; 
char motto2[] = "Wins the Game!"; 

int main(void)
{
	_delay_ms(50); 
	
	DDRC = 0xff; 
	DDRD = 0xff; 
	
	CLCD_cmd(BIT4_LINE2_DOT58); 
	CLCD_cmd(DISPON_CUROFF_BLKOFF); 
	CLCD_cmd(INC_NOSHIFT); 
	CLCD_cmd(DISPCLEAR); 
	
	_delay_ms(2); 
	
	CLCD_cmd(CUR1LINE); 
	CLCD_puts(motto1); 
	CLCD_cmd(CUR2LINE); 
	CLCD_puts(motto2); 
	
	while(1) 
	{
		_delay_ms(1000);
		CLCD_cmd(DISPOFF_CUROFF_BLKOFF);
		_delay_ms(1000);
		CLCD_cmd(DISPON_CUROFF_BLKOFF);
	}
}
void CLCD_puts(char *ptr)
{
	while(*ptr != NULL) 
	CLCD_data(*ptr++); 
}
void CLCD_data(char data)
{
	PORTD = 0x04; 
	_delay_us(1); 
	PORTD = 0x14; 
	PORTC = data & 0xf0; 
	PORTD = 0x04; 
	_delay_us(2); 
	PORTD = 0x14;
	PORTC = (data << 4) & 0xf0; 
	PORTD = 0x04; 
	_delay_ms(1); 
}
void CLCD_cmd(char cmd)
{
	PORTD = 0x00;
	_delay_us(1); 
	PORTD = 0x10; 
	PORTC = cmd & 0xf0; 
	PORTD = 0x00; 
	_delay_us(2); 
	PORTD = 0x10;
	PORTC = (cmd << 4) & 0xf0; 
	PORTD = 0x00; 
	_delay_ms(1); 
}
*/

// 실습 1
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define NULL 0x00

#define BIT4_LINE2_DOT58 0x28
#define DISPON_CUROFF_BLKOFF 0x0C
#define DISPOFF_CUROFF_BLKOFF 0x08
#define INC_NOSHIFT 0x06
#define DISPCLEAR 0x01
#define CUR1LINE 0x80
#define CUR2LINE 0xC0
#define CURHOME 0x02
#define E_BIT 0x04
#define RW_BIT 0x03
#define RS_BIT 0x02
#define ON	 1
#define OFF	 0

void CLCD_cmd(char);
void CLCD_data(char);
void CLCD_puts(char *);

char motto1[] = "Slow & Steady";
char motto2[] = "Wins the Game!";
volatile int state = ON;

ISR(INT4_vect)
{
	_delay_ms(100);
	
	EIFR = 1 << 4;
	
	if((PINE & 0x10) == 0x10)
	return;
	
	if(state==ON)
		state=OFF;
	else
		state=ON;
	
}
int main(void)
{
	_delay_ms(50);
	
	DDRC = 0xff;
	DDRD = 0xff;
	DDRE = 0x00;
	sei();
	EICRB = 0x0a;
	EIMSK = 0x30;
	
	CLCD_cmd(BIT4_LINE2_DOT58);
	CLCD_cmd(DISPON_CUROFF_BLKOFF);
	CLCD_cmd(INC_NOSHIFT);
	CLCD_cmd(DISPCLEAR);
	
	_delay_ms(2);
	
	CLCD_cmd(CUR1LINE);
	CLCD_puts(motto1);
	CLCD_cmd(CUR2LINE);
	CLCD_puts(motto2);
	
	while(1)
	{
		if(state == ON)
		{	
			CLCD_cmd(DISPON_CUROFF_BLKOFF);	
			_delay_ms(1000);
			CLCD_cmd(DISPOFF_CUROFF_BLKOFF);
			_delay_ms(1000);
		}
		else
		{
			CLCD_cmd(DISPOFF_CUROFF_BLKOFF);
			_delay_ms(100);
		}
	}
}
void CLCD_puts(char *ptr)
{
	while(*ptr != NULL)
	CLCD_data(*ptr++);
}
void CLCD_data(char data)
{
	PORTD = 0x04;
	_delay_us(1);
	PORTD = 0x14;
	PORTC = data & 0xf0;
	PORTD = 0x04;
	_delay_us(2);
	PORTD = 0x14;
	PORTC = (data << 4) & 0xf0;
	PORTD = 0x04;
	_delay_ms(1);
}
void CLCD_cmd(char cmd)
{
	PORTD = 0x00;
	_delay_us(1);
	PORTD = 0x10;
	PORTC = cmd & 0xf0;
	PORTD = 0x00;
	_delay_us(2);
	PORTD = 0x10;
	PORTC = (cmd << 4) & 0xf0;
	PORTD = 0x00;
	_delay_ms(1);
}
*/

// 실습 2
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define NULL 0x00

#define BIT4_LINE2_DOT58 0x28
#define DISPON_CUROFF_BLKOFF 0x0C
#define DISPOFF_CUROFF_BLKOFF 0x08
#define INC_NOSHIFT 0x06 
#define DISPCLEAR 0x01 
#define CUR1LINE 0x80 
#define CUR2LINE 0xC0 
#define CURHOME 0x02 
#define E_BIT 0x04 
#define RW_BIT 0x03
#define RS_BIT 0x02 

void CLCD_cmd(char); 
void CLCD_data(char); 
void CLCD_puts(char *); 

char motto1[20] = {0};
char motto2[20] = {0};
volatile int i = 0,j = 0;
volatile char data;

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
	
	
	if(data == '\t')
		{	
			for(j=0; j<20; j++)
			{
				motto1[j] = 0;
				motto2[j] = 0;
			}	
			i=0;
			CLCD_cmd(DISPCLEAR);
			return;
		}

	if(i<16)
		{
			motto1[i] = data;
			i++;
		}
	else
		{
			motto2[i-16] = data;
			i++;
		}
}
ISR(USART0_UDRE_vect)
{
	UCSR0B &= ~(1<<UDRIE0);
}

int main(void)
{
	
	DDRC = 0xff; 
	DDRD = 0xff; 
	init_uart0();
	
	_delay_ms(50);
	CLCD_cmd(BIT4_LINE2_DOT58); 
	CLCD_cmd(DISPON_CUROFF_BLKOFF); 
	CLCD_cmd(INC_NOSHIFT); 
	CLCD_cmd(DISPCLEAR); 
	_delay_ms(2); 
	
	while(1) 
	{
		CLCD_cmd(CUR1LINE);
		CLCD_puts(motto1);
		CLCD_cmd(CUR2LINE);
		CLCD_puts(motto2);	
	}
}

void CLCD_puts(char *ptr)
{
	while(*ptr != NULL) 
		CLCD_data(*ptr++); 
}

void CLCD_data(char data)
{
	PORTD = 0x04; 
	_delay_us(1); 
	PORTD = 0x14; 
	PORTC = data & 0xf0; 
	PORTD = 0x04; 
	_delay_us(2); 
	PORTD = 0x14;
	PORTC = (data << 4) & 0xf0; 
	PORTD = 0x04; 
	_delay_ms(1); 
}

void CLCD_cmd(char cmd)
{
	PORTD = 0x00;
	_delay_us(1); 
	PORTD = 0x10; 
	PORTC = cmd & 0xf0; 
	PORTD = 0x00; 
	_delay_us(2); 
	PORTD = 0x10;
	PORTC = (cmd << 4) & 0xf0; 
	PORTD = 0x00; 
	_delay_ms(1); 
}
*/

// 실습 3


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define NULL 0x00
#include <stdio.h>
#include <string.h>

#define BIT4_LINE2_DOT58 0x28
#define DISPON_CUROFF_BLKOFF 0x0C
#define DISPOFF_CUROFF_BLKOFF 0x08
#define INC_NOSHIFT 0x06 
#define DISPCLEAR 0x01 
#define CUR1LINE 0x80 
#define CUR2LINE 0xC0 
#define CURHOME 0x02 
#define E_BIT 0x04 
#define RW_BIT 0x03
#define RS_BIT 0x02 

#define CDS_10 341
#define CDS_20 450
#define CDS_50 550
#define CDS_80 650
#define CDS_100 731

unsigned short read_adc();
void init_adc();
void CLCD_cmd(char); 
void CLCD_data(char); 
void CLCD_puts(char *); 
void get_clcd(short a);
double ppow(double a, double b);

char motto1[20] = {0};
char motto2[20] = {0};
unsigned short num;
int i=0 ,j=0, dec, sign;
char charc[40] = {0};


int main(void)
{
	_delay_ms(50); 
	
	DDRC = 0xff; 
	DDRD = 0xff; 
	DDRF = 0x00;
	
	init_adc();
	
	CLCD_cmd(BIT4_LINE2_DOT58); 
	CLCD_cmd(DISPON_CUROFF_BLKOFF); 
	CLCD_cmd(INC_NOSHIFT); 
	CLCD_cmd(DISPCLEAR); 
	
	_delay_ms(2); 
	
	
	
	while(1) 
	{	
		num = read_adc();
		get_clcd(num);
		
		CLCD_cmd(CUR1LINE);
		CLCD_puts(motto1);
		CLCD_cmd(CUR2LINE);
		CLCD_puts(motto2);
		
	}
}
void CLCD_puts(char *ptr)
{
	while(*ptr != NULL) 
	CLCD_data(*ptr++); 
}
void CLCD_data(char data)
{
	PORTD = 0x04; 
	_delay_us(1); 
	PORTD = 0x14; 
	PORTC = data & 0xf0; 
	PORTD = 0x04; 
	_delay_us(2); 
	PORTD = 0x14;
	PORTC = (data << 4) & 0xf0; 
	PORTD = 0x04; 
	_delay_ms(1); 
}
void CLCD_cmd(char cmd)
{
	PORTD = 0x00;
	_delay_us(1); 
	PORTD = 0x10; 
	PORTC = cmd & 0xf0; 
	PORTD = 0x00; 
	_delay_us(2); 
	PORTD = 0x10;
	PORTC = (cmd << 4) & 0xf0; 
	PORTD = 0x00; 
	_delay_ms(1); 
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
	while ((ADCSRA & 0x10) != 0x10)
	;
	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	return value;
}
void get_clcd(short a)
{	
	short num;
	int k;
	
	num = a/10;
	
	sprintf(charc,"%dLUX",num);
	
	for(k=0;k<32;k++)
	{
		if(k<16)
			motto1[k] = charc[k];
		else
			motto2[k-16] = charc[k];
	}
	
}
double ppow(double a, double b) // 제곱함수
{
	double c;
	double d=1;
	for(c=0;c<b;c++)
	d*=a;
	return d;
}

