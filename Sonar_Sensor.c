
// 예제

/*
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define SOUND_VELOCITY 340UL 
#define TRIG 6 
#define ECHO 7 
#define INT4 4 
#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define GO 1
#define STOP 0
#define NULL 0x00int distance_1st, distance_2nd, height; // 1, 2차 거리 측정값과 키 계산값
unsigned char state = STATE_0;
unsigned char Go_Stop = STOP;

#define BIT4_LINE2_DOT58 0x28 // 4 Bit Mode, 2 Lines, 5x8 Dot
#define DISPON_CUROFF_BLKOFF 0x0C // Display On,Cursor Off, Blink Off
#define DISPOFF_CUROFF_BLKOFF 0x08 // Display Off,Cursor Off, Blink Off
#define INC_NOSHIFT 0x06 // Entry Mode, Cursor Increment,
#define DISPCLEAR 0x01 
#define CUR1LINE 0x80 // Cursor Position Line 1 First
#define CUR2LINE 0xC0 // Cursor Position Line 2 First
#define CURHOME 0x02 // Cursor Home
#define E_BIT 0x04 // Enable Bit #
#define RW_BIT 0x03 // Read Write Bit #
#define RS_BIT 0x02 // Register Select Bit #

void CLCD_cmd(char); // 명령어 전송 함수
void CLCD_data(char); // 데이터 Write 함수
void CLCD_puts(char *); // 문자열 처리 함수
void display_fnd(unsigned int value); // FND Display 함수
void CLCD_num_display(int num);
void init_interrupt(); // Interrupt 처리 함수
void init_CLCD();
int read_distance();

char If_Ready[] = "If Ready"; // 측정시작 프롬프트(1열)
char Press_Switch[] = "Press Switch!"; // 측정시작 프롬프트(2열)
char First[] = "First : "; // 첫번째 측정(머리에서 천정까지)
char Second[] = "Second : "; // 두번째 측정(바닥에서 천정까지)
char Height[] = "Height : "; // 높이 계산 및 Display
char CLCD_NUM[] = "000.0"; // 초기값 = 000.0
char Error[] = "Error !"; 

int main(void)
{
	DDRA = ((DDRA|(1<<TRIG))&~(1<<ECHO));
	DDRE = 0x00; 
	DDRE = ((DDRE | (1<<TRIG)) & ~(1<<ECHO));
	// 초음파거리센서 TRIG(bit6, 출력), ECHO(bit7, 입력)
	DDRC = 0xff; // CLCD PORT(data & command)
	DDRD = 0xff; // CLCD PORT(control 출력 : RS=bit2, RW=bit3, E=bit4)
	init_interrupt(); // 인터럽트 초기화
	init_CLCD(); // CLCD 초기화
	while(1) // 초기화 후에는 모든 일을 인터럽트에서 처리
	;
}

void init_interrupt() // Interrupt 초기화
{
	EICRB = 0x02; // INT4 트리거 모드는 하강 에지(Falling Edge)
	EIMSK = 0x10; // INT4 인터럽트 활성화
	sei(); // SREG의 I(Interrupt Enable) 비트(bit7) ‘1’로 세트
}
void init_CLCD() // LCD 초기화
{
	_delay_ms(50); // 전원 인가후 CLCD 셋업 시간
	PORTC = 0x00; // 데이타 clear
	CLCD_cmd(BIT4_LINE2_DOT58); // 4 Bit Mode, 2 Lines, 5x8 Dot
	CLCD_cmd(DISPON_CUROFF_BLKOFF); // Display On, Cursor Off, Blink Off
	CLCD_cmd(INC_NOSHIFT); // Entry Mode, Cursor Increment, Display No Shift
	CLCD_cmd(DISPCLEAR); // Display Clear, Address 0
	_delay_ms(2); // 디스플레이 클리어 실행 시간 동안 대기
}
ISR (INT4_vect) // INT4 인터럽트 발생시
{
	static int distance_1st, distance_2nd; // 교재 수정 필요
	int height; // 교재 수정 필요
	
	_delay_ms(100); // 스위치 바운스 시간 동안 기다림
	EIFR |= 1 << 4; 
	
	if ((PINE & 0x10) != 0x00) 
		return; 
		
	switch (state) {
		case STATE_0: // 스위치를 1번째 누른 것 확인
		CLCD_cmd(DISPCLEAR);
		CLCD_cmd(CUR1LINE);
		CLCD_puts(If_Ready);
		CLCD_cmd(CUR2LINE);
		CLCD_puts(Press_Switch);
		state = STATE_1;
		break;
		
		case STATE_1: // 스위치를 2번째 누른 것 확인
		CLCD_cmd(DISPCLEAR);
		CLCD_cmd(CUR1LINE);
		CLCD_puts(First); // Display “First :”
		distance_1st = read_distance();
		CLCD_num_display(distance_1st);
		state = STATE_2;
		break;
		
		case STATE_2: // 스위치를 3번째 누른 것 확인
		CLCD_cmd(CUR2LINE);
		CLCD_puts(Second); // Display “Second :”
		distance_2nd = read_distance();
		CLCD_num_display(distance_2nd);
		state = STATE_3;
		break;
		
		case STATE_3: // 스위치를 4번째 누른 것 확인
		CLCD_cmd(DISPCLEAR);
		CLCD_cmd(CUR1LINE);
		CLCD_puts(Height); // Display “Height :”
		height = distance_2nd - distance_1st;
		if (height < 0) CLCD_puts(Error);
		else CLCD_num_display(height);
		state = STATE_0;
		break;
	}
}

int read_distance()
{
	unsigned int distance = 0;
	TCCR1B = 0x03; // Counter/Timer 1 클록 4us
	PORTA &= ~(1<<TRIG); // Trig = LOW
	_delay_us(10);
	PORTA |= (1<<TRIG); // Trig = HIGH
	_delay_us(10);
	PORTA&= ~(1<<TRIG); // Trig = LOW
	while(!(PINA & (1<<ECHO))) // wait until Echo = HIGH
	;
	TCNT1 = 0x0000; // Counter/Timer 1 값 초기화(=0)
	while (PINA & (1<<ECHO))
	;
	TCCR1B = 0x00; // Counter/Timer 1 클록 정지
	distance = (unsigned int)(SOUND_VELOCITY * (TCNT1 * 4 / 2) / 1000);
	return(distance);
}

void CLCD_puts(char *ptr)
{
	while(*ptr != NULL) // 문자열이 마지막(NULL)인지 검사
	CLCD_data(*ptr++); // 마지막이 아니면 1문자 디스플레이
}
void CLCD_num_display(int num)
{
	CLCD_NUM[0] = (num/1000)%10 + 0x30;
	CLCD_NUM[1] = (num/100)%10 + 0x30;
	CLCD_NUM[2] = (num/10)%10 + 0x30;
	CLCD_NUM[3] = '.';
	CLCD_NUM[4] = (num/1)%10 + 0x30;
	CLCD_NUM[5] = NULL;
	CLCD_puts(CLCD_NUM);
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
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define SOUND_VELOCITY 340UL
#define TRIG 6
#define ECHO 7
#define INT4 4

int read_distance();
int ppow(int a, int b);
void put_led(int dis);


int main(void)
{
	DDRE = 0x00; 
	DDRA = ((DDRA|(1<<TRIG))&~(1<<ECHO));
	DDRB = 0xff;
	
	int dis;

	while(1) 
	{
		dis = read_distance();
		put_led(dis);
	}
}

int read_distance()
{
	unsigned int distance = 0;
	TCCR1B = 0x03;
	PORTA &= ~(1<<TRIG);
	_delay_us(10);
	PORTA |= (1<<TRIG);
	_delay_us(10);
	PORTA &= ~(1<<TRIG);
	while(!(PINA & (1<<ECHO)));
	TCNT1 = 0x0000;
	while (PINA & (1<<ECHO));
	TCCR1B = 0x00;
	distance = (unsigned int)(SOUND_VELOCITY * (TCNT1 * 4 / 2) / 1000);
	return(distance);
}

void put_led(int dis)
{
	if(dis<900)
		PORTB = ppow(2,dis/100) - 1; 
	else
	{
		PORTB = 255;
		_delay_ms(500);
		PORTB = 0;
		_delay_ms(500);
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


// 실습 2


/*
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define SOUND_VELOCITY 340UL 
#define TRIG 6 
#define ECHO 7 
#define INT4 4 
#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define GO 1
#define STOP 0
#define NULL 0x00int distance_1st, distance_2nd, height; // 1, 2차 거리 측정값과 키 계산값
unsigned char state = STATE_0;
unsigned char Go_Stop = STOP;

#define BIT4_LINE2_DOT58 0x28 // 4 Bit Mode, 2 Lines, 5x8 Dot
#define DISPON_CUROFF_BLKOFF 0x0C // Display On,Cursor Off, Blink Off
#define DISPOFF_CUROFF_BLKOFF 0x08 // Display Off,Cursor Off, Blink Off
#define INC_NOSHIFT 0x06 // Entry Mode, Cursor Increment,
#define DISPCLEAR 0x01 
#define CUR1LINE 0x80 // Cursor Position Line 1 First
#define CUR2LINE 0xC0 // Cursor Position Line 2 First
#define CURHOME 0x02 // Cursor Home
#define E_BIT 0x04 // Enable Bit #
#define RW_BIT 0x03 // Read Write Bit #
#define RS_BIT 0x02 // Register Select Bit #

char CLCD_NUM[] = " 000.0 cm"; 

void CLCD_cmd(char); // 명령어 전송 함수
void CLCD_data(char); // 데이터 Write 함수
void CLCD_puts(char *); // 문자열 처리 함수
void display_fnd(unsigned int value); // FND Display 함수
void CLCD_num_display(int num);
void init_interrupt(); // Interrupt 처리 함수
void init_CLCD();
int read_distance();

volatile int msec = 0, dis = 0;

int main(void)
{
	DDRE = 0x00; 
	DDRA = ((DDRE | (1<<TRIG)) & ~(1<<ECHO));
	DDRC = 0xff; 
	DDRD = 0xff; 
	
	
	
	init_interrupt(); // 인터럽트 초기화
	init_CLCD(); // CLCD 초기화
	
	while(1) 
	{	
		CLCD_num_display(dis);
		
		if(state == STATE_1)
		{
			dis = read_distance();
			CLCD_num_display(dis);
			break;
		}
	}
}

void init_interrupt() // Interrupt 초기화
{
	TIMSK = 0x01;
	TCCR0 = 0x04;
	EICRB = 0x02; 
	EIMSK = 0x10;
	sei(); 
}

void init_CLCD() // LCD 초기화
{
	_delay_ms(50); // 전원 인가후 CLCD 셋업 시간
	PORTC = 0x00; // 데이타 clear
	CLCD_cmd(BIT4_LINE2_DOT58); // 4 Bit Mode, 2 Lines, 5x8 Dot
	CLCD_cmd(DISPON_CUROFF_BLKOFF); // Display On, Cursor Off, Blink Off
	CLCD_cmd(INC_NOSHIFT); // Entry Mode, Cursor Increment, Display No Shift
	CLCD_cmd(DISPCLEAR); // Display Clear, Address 0
	_delay_ms(2); // 디스플레이 클리어 실행 시간 동안 대기
}

ISR(TIMER0_OVF_vect) 
{
	TCNT0 = 6;
	msec++;
	
	if(msec == 100 && state == STATE_0)
	{	
		CLCD_cmd(DISPCLEAR);
		CLCD_puts(CLCD_NUM); 
		dis = read_distance();
		msec = 0;
	}
	
	
}

ISR (INT4_vect) // INT4 인터럽트 발생시
{
	_delay_ms(100); // 스위치 바운스 시간 동안 기다림
	EIFR |= 1 << 4;
	
	if ((PINE & 0x10) != 0x00)
	return;
	
	state = STATE_1;
	
}
int read_distance()
{
	unsigned int distance = 0;
	TCCR1B = 0x03; // Counter/Timer 1 클록 4us
	PORTA &= ~(1<<TRIG); // Trig = LOW
	_delay_us(10);
	PORTA |= (1<<TRIG); // Trig = HIGH
	_delay_us(10);
	PORTA &= ~(1<<TRIG); // Trig = LOW
	while(!(PINA & (1<<ECHO))) // wait until Echo = HIGH
	;
	TCNT1 = 0x0000; // Counter/Timer 1 값 초기화(=0)
	while (PINA & (1<<ECHO))
	;
	TCCR1B = 0x00; // Counter/Timer 1 클록 정지
	distance = (unsigned int)(SOUND_VELOCITY * (TCNT1 * 4 / 2) / 1000);
	return(distance);
}

void CLCD_puts(char *ptr)
{
	while(*ptr != NULL) // 문자열이 마지막(NULL)인지 검사
	CLCD_data(*ptr++); // 마지막이 아니면 1문자 디스플레이
}

void CLCD_num_display(int num)
{
	CLCD_NUM[1] = (num/1000)%10 + 0x30;
	CLCD_NUM[2] = (num/100)%10 + 0x30;
	CLCD_NUM[3] = (num/10)%10 + 0x30;
	CLCD_NUM[4] = '.';
	CLCD_NUM[5] = (num/1)%10 + 0x30;
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




/*

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define SOUND_VELOCITY 340UL 
#define TRIG 6 
#define ECHO 7 
#define INT4 4 


#define BIT4_LINE2_DOT58 0x28 
#define DISPON_CUROFF_BLKOFF 0x0C 
#define DISPOFF_CUROFF_BLKOFF 0x08 
#define INC_NOSHIFT 0x06 
#define DISPCLEAR 0x01 
#define CUR1LINE 0x80 
#define CUR2LINE 0xC0 
#define NULL 0x00

void CLCD_cmd(char); 
void CLCD_data(char); 
void CLCD_puts(char *); 
void display_fnd(unsigned int value); 
void CLCD_num_display(int num);
void init_interrupt(); 
void init_CLCD();
int read_distance();
void output_clcd();

char If_on_bottom[] = "If on bottom"; 
char If_on_head[] = "If on head"; 
char Press_Switch[] = "Press Switch!"; 
char Height[] = "Your height : "; 
char CLCD_NUM[] = "000.0 cm"; 
char Error[] = "Error !"; 

int distance_1st, distance_2nd, height; 
unsigned int state = 0;

int main(void)
{
	DDRE = 0x00; 
	DDRE = ((DDRE | (1<<TRIG)) & ~(1<<ECHO));
	DDRC = 0xff; 
	DDRD = 0xff;
	init_interrupt();
	init_CLCD();
	while(1) 
	{
		output_clcd();
	}
}
void output_clcd()
{
	if(state == 0)
	{
		CLCD_cmd(DISPCLEAR);
		CLCD_cmd(CUR1LINE);
		CLCD_puts(If_on_bottom);
		CLCD_cmd(CUR2LINE);
		CLCD_puts(Press_Switch);
		_delay_ms(25);
	}
	else if(state == 1)
	{
		CLCD_cmd(DISPCLEAR);
		CLCD_cmd(CUR1LINE);
		CLCD_puts(If_on_head);
		CLCD_cmd(CUR2LINE);
		CLCD_puts(Press_Switch);
		_delay_ms(25);
	}
	else
	{
		CLCD_cmd(DISPCLEAR);
		CLCD_cmd(CUR1LINE);
		CLCD_puts(Height);
		CLCD_cmd(CUR2LINE);
		CLCD_num_display(height);
		if(height >= 0 )
		{
			CLCD_puts(CLCD_NUM);
			_delay_ms(25);
		}
		else
		{
			CLCD_puts(Error);
			_delay_ms(25);
		}
	}
	
}
void init_interrupt() 
{
	EICRB = 0x02; 
	EIMSK = 0x10; 
	sei(); 
}
void init_CLCD() 
{
	_delay_ms(50); 
	PORTC = 0x00; 
	CLCD_cmd(BIT4_LINE2_DOT58); 
	CLCD_cmd(DISPON_CUROFF_BLKOFF); 
	CLCD_cmd(INC_NOSHIFT); 
	CLCD_cmd(DISPCLEAR); 
	_delay_ms(2); 
}
ISR (INT4_vect) 
{
	
	_delay_ms(100); 
	EIFR |= 1 << 4; 
	
	if ((PINE & 0x10) != 0x00) 
		return; 
	
	if(state == 0)
		distance_1st = read_distance();
	else
		distance_2nd = read_distance();
		
	height = distance_1st - distance_2nd;
	
	state++;
	
}

int read_distance()
{
	unsigned int distance = 0;
	TCCR1B = 0x03; 
	PORTA &= ~(1<<TRIG); 
	_delay_us(10);
	PORTA |= (1<<TRIG); 
	_delay_us(10);
	PORTA &= ~(1<<TRIG); 
	while(!(PINA & (1<<ECHO))) 
	;
	TCNT1 = 0x0000; 
	while (PINA & (1<<ECHO))
	;
	TCCR1B = 0x00;
	distance = (unsigned int)(SOUND_VELOCITY * (TCNT1 * 4 / 2) / 1000);
	return(distance);
}

void CLCD_puts(char *ptr)
{
	while(*ptr != NULL) 
	CLCD_data(*ptr++); 
}
void CLCD_num_display(int num)
{
	CLCD_NUM[0] = (num/1000)%10 + 0x30;
	CLCD_NUM[1] = (num/100)%10 + 0x30;
	CLCD_NUM[2] = (num/10)%10 + 0x30;
	CLCD_NUM[3] = '.';
	CLCD_NUM[4] = (num/1)%10 + 0x30;
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

