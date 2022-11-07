/*
 * Test.c
 *
 * Created: 2022-09-15 오전 12:23:35
 * Author : USER
 */ 
/*
#include <avr/io.h>
#include <stdlib.h>

#define __DELAY_BACKWARD_COMPATIBLE__

#define  F_CPU	16000000UL
#include <util/delay.h>



int main(void)
{
	unsigned char i;
	DDRA = 0xff;
	
	while (1)
	{
		for(i=0x80;i>0;i=i>>1)
		{
			PORTA = i;
			_delay_ms(1000);
		}
		
		for(i=0x02;i<0x80;i=i<<1)
		{
			PORTA = i;
			_delay_ms(1000);
		}		
	}
}
*/

/*
int main(void)
{
    
	DDRA = 0xff;
	
    while (1) 
    {
		PORTA = rand()%256;
		_delay_ms(((rand()%50)+1)*100);
		
    }
}
*/




