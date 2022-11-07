/*
 * GPIO3.c
 *
 * Created: 2022-09-28 오후 11:40:11
 *  Author: USER
 */ 

// 4.5
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define  IDLE	0
#define  STOP	1
#define  GO		2

volatile int cur_time = 0;
volatile int stop_time = 0;
volatile int state = IDLE;

unsigned char digit[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[4] = {0x01,0x02,0x04,0x08};

ISR(INT4_vect)
{
	_delay_ms(100);
	EIFR = 1 << 4;
	if((PINE & 0x10) == 0x10)
		return;
	if(state == IDLE || state == STOP)
		state = GO;
		
	else
	{
		state = STOP;
		stop_time = cur_time;
	}
}

ISR(INT5_vect)
{
	_delay_ms(100);
	EIFR = 1 << 5;
	if((PINE & 0x20) == 0x20)
		return;
	
	state = IDLE;
	cur_time = 0;
	stop_time = 0;
}

void int_stopwatch(void);
void display_fnd(int);

int main()
{
	int_stopwatch();
	while(1)
	{
		if(state == IDLE)
			display_fnd(stop_time);
		else if(state == STOP)
		{
			display_fnd(stop_time);
			cur_time++;
		}
		else
		{
			display_fnd(cur_time);
			cur_time++;
		}
		if(cur_time == 10000)
			cur_time = 0;
	}
	
	
}

void int_stopwatch(void)
{
	DDRC = 0xff;
	DDRG = 0x0f;
	DDRE = 0x00;
	sei();
	
	EICRB = 0x0a;
	EIMSK = 0x30;
}

void display_fnd(int count)
{
	int i,fnd[4];
	
	fnd[3] = (count / 1000) % 10;
	fnd[2] = (count / 100) % 10;
	fnd[1] = (count / 10) % 10;
	fnd[0] = count % 10;
	for(i=0;i<4;i++)
	{
		if(i==2)
			PORTC = digit[fnd[i]] | 0x80;
		else
			PORTC = digit[fnd[i]];
		PORTG = fnd_sel[i];
		if(i%2)
			_delay_ms(2);
		else
			_delay_ms(3);
		
	}
}
*/

// DIY_2
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define  STOP	0
#define  GO		1

volatile int cur_time = 9999;
//volatile int stop_time = 9999;
volatile int state = GO;

unsigned char digit[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[4] = {0x01,0x02,0x04,0x08};
void display_fnd(int);

ISR(INT4_vect)
{
	_delay_ms(100);
	EIFR = 1 << 4;
	
	if((PINE & 0x10) == 0x10)
		return;
		
	if(cur_time == 0)
		return;
		
	if(state == STOP)
		state = GO;
		
	else
	{
		state = STOP;
		//stop_time = cur_time;
		display_fnd(cur_time);
	}
	
}

ISR(INT5_vect)
{
	_delay_ms(100);
	EIFR = 1 << 5;
	
	if((PINE & 0x20) == 0x20)
		return;
	
	if(cur_time == 0)
		return;
	
	cur_time += 1000;
	
	if(cur_time > 9999)
		cur_time = 9999;
		
	if(state == STOP)
		display_fnd(cur_time);
	
}

void int_stopwatch(void);


int main()
{
	int_stopwatch();
	while(1)
	{
		if(state == STOP)
			display_fnd(cur_time);
		else
		{
			display_fnd(cur_time);
			cur_time--;
			if(cur_time < 0)
				cur_time = 0;
		}
	}
}

void int_stopwatch(void)
{
	DDRC = 0xff;
	DDRG = 0x0f;
	DDRE = 0x00;
	sei();
	
	EICRB = 0x0a;
	EIMSK = 0x30;
}

void display_fnd(int count)
{
	int i,j,fnd[4];
	
	fnd[3] = (count / 1000) % 10;
	fnd[2] = (count / 100) % 10;
	fnd[1] = (count / 10) % 10;
	fnd[0] = count % 10;
	
	for(i=0;i<10;i++)
	{
		for(j=0;j<4;j++)
		{
			PORTC = digit[fnd[j]];
			PORTG = fnd_sel[j];
			if(j%2)
			_delay_ms(2);
			else
			_delay_ms(3);
		}
    }

}
*/

// DIY_3

/*
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define  STOP	1
#define  GO		2
#define  UP		3
#define  DOWN	4

volatile int cur_floor = 1;
volatile int stop_floor = 1;
volatile int state = GO;
volatile int direction = UP;

unsigned char digit[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[2] = {0x02,0x04};
unsigned char basement = 0x40;

ISR(INT4_vect)
{
	_delay_ms(100);
	EIFR = 1 << 4;
	if((PINE & 0x10) == 0x10)
		return;
	if(state == STOP)
		state = GO;
	else
	{
		state = STOP;
		stop_floor = cur_floor;
	}
}

ISR(INT5_vect)
{
	_delay_ms(100);
	EIFR = 1 << 5;
	if((PINE & 0x20) == 0x20)
	return;
	
	if(direction==UP)
		direction = DOWN;
	else
		direction = UP;
}

void int_stopwatch(void);
void display_fnd(int);


int main()
{
	int_stopwatch();
	while(1)
	{
		if(state == STOP)
			display_fnd(stop_floor);
		else
		{
			if(direction == UP)
			{
				display_fnd(cur_floor);
				cur_floor++;
			}
			else
			{
				display_fnd(cur_floor);
				cur_floor--;
			}
		}
		
		if(cur_floor == 25)
			direction = DOWN;
		else if(cur_floor == 1)
			direction = UP;
	}
}

void int_stopwatch(void)
{
	DDRC = 0xff;
	DDRG = 0x0f;
	DDRE = 0x00;
	sei();
	
	EICRB = 0x0a;
	EIMSK = 0x30;
}

void display_fnd(int count)
{
	int i, j, stage, fnd[2];
	
	stage = count - 3;
	
	if(stage > 0)
	{
		fnd[1] = (stage / 10) % 10;
		fnd[0] = stage % 10;
		
		for(i=0;i<100;i++)
		{
			for(j=0;j<2;j++)
			{
				PORTC = digit[fnd[j]];
				PORTG = fnd_sel[j];
				_delay_ms(5);
			}
		}
	}
	else
	{	
		fnd[0] = (1 - stage);
		
		for(i=0;i<100;i++)
		{
			for(j=0;j<2;j++)
			{
				if(j)
					PORTC = basement;
				else
					PORTC = digit[fnd[j]];
					
				PORTG = fnd_sel[j];
				_delay_ms(5);
					
			}
		}
	}
}
*/












































































































