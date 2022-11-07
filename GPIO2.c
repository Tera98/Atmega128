/*
 * GPIO2.c
 *
 * Created: 2022-09-22 오전 12:21:29  01100111
 *  Author: USER
 */ 

// 0: 0x3f, 1: 0x06, 2: 0x5b, 3: 0x4f, 4: 0x66, 5: 0x6d, 6: 0x7d, 7: 0x27, 8: 0x7f, 9: 0x67, - : 0x40



//3.4 FND 전화번호 표시
/*
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

int main(void)
{
	char tel_num[20] = {0x00, 0x00, 0x00, 0x00, 0x3f, 0x06, 0x3f, 0x40, 
					    0x27, 0x5b, 0x67, 0x4f, 0x40, 0x3f, 0x7d, 0x5b,
						0x7d, 0x00, 0x00, 0x00};
	int i,j;
	DDRC = 0xff;
	DDRG = 0x0f;
	while(1)
	{
		for(i=0;i<17;i++)
		{
			for(j=0;j<50;j++)
			{
				PORTC = tel_num[i];		PORTG = 0x08;	_delay_ms(5);
				PORTC = tel_num[i+1];	PORTG = 0x04;	_delay_ms(5);
				PORTC = tel_num[i+2];	PORTG = 0x02;	_delay_ms(5);
				PORTC = tel_num[i+3];	PORTG = 0x01;   _delay_ms(5);
			}
		}
	}
}
*/

// DIY-3 [FND-2]
// H : 01110110 0x76, E : 01111001 0x79, L : 00111000 0x38, P : 01110011 0x73 


// DIY-3 [FND-3]
/*
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

int main(void)
{
	char tel_num[20] = {0x00, 0x00, 0x00, 0x00, 0x3f, 0x06, 0x3f, 0x40,
						0x27, 0x5b, 0x67, 0x4f, 0x40, 0x3f, 0x7d, 0x5b,
						0x7d, 0x00, 0x00, 0x00};
	int i,j;
	DDRC = 0xff;
	DDRG = 0x0f;
	while(1)
	{
		for(i=0;i<17;i++)
		{
			for(j=0;j<25;j++)
			{
				PORTC = tel_num[i];		PORTG = 0x08;	_delay_ms(5);
				PORTC = tel_num[i+1];	PORTG = 0x04;	_delay_ms(5);
				PORTC = tel_num[i+2];	PORTG = 0x02;	_delay_ms(5);
				PORTC = tel_num[i+3];	PORTG = 0x01;   _delay_ms(5);
			}
			
			PORTG = 0x00;	 
			_delay_ms(500);
		}
	}
}
*/

