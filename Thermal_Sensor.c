

// 예제

/*
#define F_CPU 16000000UL // CPU 클록 값 = 16 MHz
#define F_SCK 40000UL // SCK 클록 값 = 40 KHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // interrupt 관련
#define LM75A_ADDR 0x90 // 0b10011000, 7비트를 1비트 left shift
#define LM75A_CONFIG_REG 1
#define LM75A_TEMP_REG 0
#define STOP 0 // 체온계 모드
#define GO 1 // 온도계 모드
int sw=0;

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
0x6d, 0x7c, 0x07, 0x7f, 0x67};
volatile int temperature = 0; // 전역변수(Global Variable)
volatile int stop_temp = 0; // 정지 시 온도
volatile int state = GO;
void init_twi_port();
int read_twi_2byte_nopreset(char reg);
void display_FND(int value);

int main(void)
{
	int i;
	DDRC = 0xff; // C 포트는 FND 데이터 출력 신호
	DDRG = 0x0f; // G 포트는 FND 선택 출력 신호
	DDRE = 0xef; // 0b11101111, PE4(SW1)는 입력
	DDRD = 0x00;
	EICRB = 0x02; // INT4 트리거 모드는 하강 에지(falling edge)
	EIMSK = 0x10; // INT4 인터럽트 인애이블
	sei();
	
	init_twi_port(); // TWI 및 포트 초기화
	while (1) // 온도값 읽어 FND 디스플레이
	{
		for (i=0; i<60; i++) // 0.6초 동안 처리
		{
			if (state == GO) // 온도계 모드인 경우
			{
				if ((i == 0) || (i == 30)) // 억세스시간(300ms) 동안 기다리기
				// 위하여 30번에 1번 꼴로 실제 억세스
				temperature = read_twi_2byte_nopreset(LM75A_TEMP_REG);
				display_FND(temperature); // 한 번 디스플레이에 약 10ms 소요
			}
			else // 체온계 모드인 경우
			{
				if (i < 30) // 0.3초는 디스플레이
				{
					temperature = stop_temp;
					display_FND(temperature); // 한 번 디스플레이에 약 10ms 소요
				}
				else // 0.3초는 디스플레이 끔
				{
						PORTG = 0x00;
						
						_delay_ms(10); // 10ms 딜레이
				}
			}
		}
	}
}

void init_twi_port()
	{
		DDRC = 0xff; DDRG = 0xff; // FND 출력 세팅
		TWSR = TWSR & 0xfc; // Prescaler 값 = 00 (1배)
		TWBR = (F_CPU/F_SCK - 16) / 2; // 공식 참조, bit rate 설정
	}
int read_twi_2byte_nopreset(char reg)
	{
		char high_byte, low_byte;
		TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN); // START 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x08) ;
		// START 상태 검사, 이후 ACK 및 상태 검사
		TWDR = LM75A_ADDR | 0; // SLA+W 준비, W=0
		TWCR = (1 << TWINT) | (1 << TWEN); // SLA+W 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x18) ;
		TWDR = reg; // LM75A Reg 값 준비
		TWCR = (1 << TWINT) | (1 << TWEN); // LM75A Reg 값 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x28) ;
		TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN); // RESTART 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x10) ;
		
		TWDR = LM75A_ADDR | 1; // SLA+R 준비, R=1
		TWCR = (1 << TWINT) | (1 << TWEN); // SLA+R 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x40) ;
		TWCR = (1 << TWINT) | (1 << TWEN | 1 << TWEA); // 1st DATA 준비
		while(((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x50) ;
		high_byte = TWDR; // 1st DATA 수신
		TWCR = (1 << TWINT) | (1 << TWEN); // 2nd DATA 준비
		while(((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x58) ;
		low_byte = TWDR; // 2nd DATA 수신
		TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // STOP 전송
		while ((TWCR & (1 << TWSTO))) ; // STOP 확인
		return((high_byte<<8) | low_byte); // 수신 DATA 리턴
	}
	void display_FND(int value)
	{
		char digit[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
		0x7c, 0x07, 0x7f, 0x67, 0x00, 0x40 }; 
		char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
		int value_int, value_deci, num[4], i;
		if ((value & 0x8000) != 0x8000) // Sign 비트 체크
		num[3] = 10; // 양수인 경우는 디스플레이 없음
		else
		{
			num[3] = 11; 
			value = (~value)+1; // 2’s Complement 값을 취함
		}
		value_int = (char)((value & 0x7f00) >> 8); // High Byte bit6~0 값 (정수 값)
		value_deci = (char)(value & 0x0080); // Low Byte bit7 값 (소수 첫째자리값)
		num[2] = (value_int / 10) % 10;
		num[1] = value_int % 10;
		num[0] = (value_deci == 0x80) ? 5 : 0; // 소수 첫째자리가 1이면 0.5에 해당하므로
		// 5를 디스플레이
		for(i=0; i<4; i++)
		{
			PORTC = digit[num[i]];
			PORTG = fnd_sel[i];
			if (i==1)
			PORTC |= 0x80; // 왼쪽에서 3번째 FND에는 소수점(.)을 찍음
			if (i%2)
			_delay_ms(2); // 2번은 2ms 지연
			else
			_delay_ms(3); // 2번은 3ms 지연, 총 10ms 지연
		}
	}
ISR(INT4_vect)
{
		_delay_ms(100); // 스위치 바운스 시간 동안 기다림
		EIFR |= 1 << 4; // 바운스에 의해 생긴 인터럽트는 무효화 - 디바운싱
		
		if ((PINE & 0x10) != 0x00) // SW1이 눌려진 것이 아니면
			return;
		
		if (state == STOP)
			state = GO; // STOP 상태라면 GO 상태로 변경
		else
		{
			state = STOP; // GO 상태라면 STOP 상태로 변경
			stop_temp = read_twi_2byte_nopreset(LM75A_TEMP_REG);
		}
}	
*/


// 실습 1
/*
#define F_CPU 16000000UL // CPU 클록 값 = 16 MHz
#define F_SCK 40000UL // SCK 클록 값 = 40 KHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // interrupt 관련
#define LM75A_ADDR 0x90 // 0b10011000, 7비트를 1비트 left shift
#define LM75A_CONFIG_REG 1
#define LM75A_TEMP_REG 0
#define STOP 0 // 체온계 모드
#define GO 1 // 온도계 모드
int sw=0;

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
0x6d, 0x7c, 0x07, 0x7f, 0x67};
volatile int temperature = 0; // 전역변수(Global Variable)
volatile int stop_temp = 0; // 정지 시 온도
volatile int state = GO;
void init_twi_port();
int read_twi_2byte_nopreset(void);
void display_FND(int value);
void pointer(char reg);

int main(void)
{
	int i;
	DDRC = 0xff; // C 포트는 FND 데이터 출력 신호
	DDRG = 0x0f; // G 포트는 FND 선택 출력 신호
	DDRE = 0xef; // 0b11101111, PE4(SW1)는 입력
	DDRD = 0x00;
	EICRB = 0x02; // INT4 트리거 모드는 하강 에지(falling edge)
	EIMSK = 0x10; // INT4 인터럽트 인애이블
	sei();
	
	
	init_twi_port(); // TWI 및 포트 초기화
	pointer(LM75A_TEMP_REG);
	
	while (1) // 온도값 읽어 FND 디스플레이
	{
		for (i=0; i<60; i++) // 0.6초 동안 처리
		{
			if (state == GO) // 온도계 모드인 경우
			{
				if ((i == 0) || (i == 30)) // 억세스시간(300ms) 동안 기다리기
				// 위하여 30번에 1번 꼴로 실제 억세스
				temperature = read_twi_2byte_nopreset();
				display_FND(temperature); // 한 번 디스플레이에 약 10ms 소요
				
			}
			else // 체온계 모드인 경우
			{
				if (i < 30) // 0.3초는 디스플레이
				{
					temperature = stop_temp;
					display_FND(temperature); // 한 번 디스플레이에 약 10ms 소요
				}
				else // 0.3초는 디스플레이 끔
				{
						PORTG = 0x00;
						
						_delay_ms(10); // 10ms 딜레이
				}
			}
		}
	}
}

void init_twi_port()
	{
		DDRC = 0xff; DDRG = 0xff; // FND 출력 세팅
		TWSR = TWSR & 0xfc; // Prescaler 값 = 00 (1배)
		TWBR = (F_CPU/F_SCK - 16) / 2; // 공식 참조, bit rate 설정
	}
	
void pointer(char reg)
{
	TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN); // START 전송
	while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x08) ;
	// START 상태 검사, 이후 ACK 및 상태 검사
	TWDR = LM75A_ADDR | 0; // SLA+W 준비, W=0
	TWCR = (1 << TWINT) | (1 << TWEN); // SLA+W 전송
	while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x18) ;
	TWDR = reg; // LM75A Reg 값 준비
	TWCR = (1 << TWINT) | (1 << TWEN); // LM75A Reg 값 전송
	while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x28) ;
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // STOP 전송
	while ((TWCR & (1 << TWSTO))) ; // STOP 확인
}
int read_twi_2byte_nopreset()
{
	char high_byte, low_byte;
	
	TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN); // START 전송
	while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x08) ;
	TWDR = LM75A_ADDR | 1; // SLA+R 준비, R=1
	TWCR = (1 << TWINT) | (1 << TWEN); // SLA+R 전송
	while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x40) ;
	TWCR = (1 << TWINT) | (1 << TWEN | 1 << TWEA); // 1st DATA 준비
	while(((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x50) ;
	high_byte = TWDR; // 1st DATA 수신
	TWCR = (1 << TWINT) | (1 << TWEN); // 2nd DATA 준비
	while(((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x58) ;
	low_byte = TWDR; // 2nd DATA 수신
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // STOP 전송
	while ((TWCR & (1 << TWSTO))) ; // STOP 확인
	return((high_byte<<8) | low_byte); // 수신 DATA 리턴
}

	void display_FND(int value)
	{
		char digit[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
		0x7c, 0x07, 0x7f, 0x67, 0x00, 0x40 }; 
		char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
		int value_int, value_deci, num[4], i;
		if ((value & 0x8000) != 0x8000) // Sign 비트 체크
		num[3] = 10; // 양수인 경우는 디스플레이 없음
		else
		{
			num[3] = 11; 
			value = (~value)+1; // 2’s Complement 값을 취함
		}
		value_int = (char)((value & 0x7f00) >> 8); // High Byte bit6~0 값 (정수 값)
		value_deci = (char)(value & 0x0080); // Low Byte bit7 값 (소수 첫째자리값)
		num[2] = (value_int / 10) % 10;
		num[1] = value_int % 10;
		num[0] = (value_deci == 0x80) ? 5 : 0; // 소수 첫째자리가 1이면 0.5에 해당하므로
		// 5를 디스플레이
		for(i=0; i<4; i++)
		{
			PORTC = digit[num[i]];
			PORTG = fnd_sel[i];
			if (i==1)
			PORTC |= 0x80; // 왼쪽에서 3번째 FND에는 소수점(.)을 찍음
			if (i%2)
			_delay_ms(2); // 2번은 2ms 지연
			else
			_delay_ms(3); // 2번은 3ms 지연, 총 10ms 지연
		}
	}
ISR(INT4_vect)
{
		_delay_ms(100); // 스위치 바운스 시간 동안 기다림
		EIFR |= 1 << 4; // 바운스에 의해 생긴 인터럽트는 무효화 - 디바운싱
		
		if ((PINE & 0x10) != 0x00) // SW1이 눌려진 것이 아니면
			return;
		
		if (state == STOP)
			state = GO; // STOP 상태라면 GO 상태로 변경
		else
		{
			state = STOP; // GO 상태라면 STOP 상태로 변경
			stop_temp = read_twi_2byte_nopreset();
		}
}	

*/


// 실습 2

/*
#define F_CPU 16000000UL // CPU 클록 값 = 16 MHz
#define F_SCK 40000UL // SCK 클록 값 = 40 KHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // interrupt 관련
#define LM75A_ADDR 0x90 // 0b10011000, 7비트를 1비트 left shift
#define LM75A_CONFIG_REG 1
#define LM75A_TEMP_REG 0
#define STOP 0 // 체온계 모드
#define GO 1 // 온도계 모드
int sw=0;

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
0x6d, 0x7c, 0x07, 0x7f, 0x67};
volatile int temperature = 0; // 전역변수(Global Variable)
volatile int stop_temp = 0; // 정지 시 온도
volatile int state = GO;
void init_twi_port();
int read_twi_2byte_nopreset(char reg);
void display_FND(int value);

int main(void)
{
	int i;
	DDRC = 0xff; // C 포트는 FND 데이터 출력 신호
	DDRG = 0x0f; // G 포트는 FND 선택 출력 신호
	DDRE = 0xef; // 0b11101111, PE4(SW1)는 입력
	DDRB |= 0x10;
	EICRB = 0x02; // INT4 트리거 모드는 하강 에지(falling edge)
	EIMSK = 0x10; // INT4 인터럽트 인애이블
	sei();
	
	init_twi_port(); // TWI 및 포트 초기화
	while (1) // 온도값 읽어 FND 디스플레이
	{
		for (i=0; i<60; i++) // 0.6초 동안 처리
		{
			if (state == GO) // 온도계 모드인 경우
			{
				if ((i == 0) || (i == 30)) // 억세스시간(300ms) 동안 기다리기
				// 위하여 30번에 1번 꼴로 실제 억세스
				temperature = read_twi_2byte_nopreset(LM75A_TEMP_REG);
				display_FND(temperature); // 한 번 디스플레이에 약 10ms 소요
			}
			else // 체온계 모드인 경우
			{
				if (i < 30) // 0.3초는 디스플레이
				{
					temperature = stop_temp;
					display_FND(temperature); // 한 번 디스플레이에 약 10ms 소요
				}
				else // 0.3초는 디스플레이 끔
				{
						PORTG = 0x00;
						
						_delay_ms(10); // 10ms 딜레이
				}
			}
		}
	}
}

void init_twi_port()
	{
		DDRC = 0xff; DDRG = 0xff; // FND 출력 세팅
		TWSR = TWSR & 0xfc; // Prescaler 값 = 00 (1배)
		TWBR = (F_CPU/F_SCK - 16) / 2; // 공식 참조, bit rate 설정
	}
int read_twi_2byte_nopreset(char reg)
	{
		char high_byte, low_byte;
		TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN); // START 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x08) ;
		// START 상태 검사, 이후 ACK 및 상태 검사
		TWDR = LM75A_ADDR | 0; // SLA+W 준비, W=0
		TWCR = (1 << TWINT) | (1 << TWEN); // SLA+W 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x18) ;
		TWDR = reg; // LM75A Reg 값 준비
		TWCR = (1 << TWINT) | (1 << TWEN); // LM75A Reg 값 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x28) ;
		TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN); // RESTART 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x10) ;
		
		TWDR = LM75A_ADDR | 1; // SLA+R 준비, R=1
		TWCR = (1 << TWINT) | (1 << TWEN); // SLA+R 전송
		while (((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x40) ;
		TWCR = (1 << TWINT) | (1 << TWEN | 1 << TWEA); // 1st DATA 준비
		while(((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x50) ;
		high_byte = TWDR; // 1st DATA 수신
		TWCR = (1 << TWINT) | (1 << TWEN); // 2nd DATA 준비
		while(((TWCR & (1 << TWINT)) == 0x00) || (TWSR & 0xf8) != 0x58) ;
		low_byte = TWDR; // 2nd DATA 수신
		TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // STOP 전송
		while ((TWCR & (1 << TWSTO))) ; // STOP 확인
		return((high_byte<<8) | low_byte); // 수신 DATA 리턴
	}
	void display_FND(int value)
	{
		char digit[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
		0x7c, 0x07, 0x7f, 0x67, 0x00, 0x40 }; 
		char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
		int value_int, value_deci, num[4], i;
		if ((value & 0x8000) != 0x8000) // Sign 비트 체크
		num[3] = 10; // 양수인 경우는 디스플레이 없음
		else
		{
			num[3] = 11; 
			value = (~value)+1; // 2’s Complement 값을 취함
		}
		value_int = (char)((value & 0x7f00) >> 8); // High Byte bit6~0 값 (정수 값)
		value_deci = (char)(value & 0x0080); // Low Byte bit7 값 (소수 첫째자리값)
		num[2] = (value_int / 10) % 10;
		num[1] = value_int % 10;
		num[0] = (value_deci == 0x80) ? 5 : 0; // 소수 첫째자리가 1이면 0.5에 해당하므로
		// 5를 디스플레이
		for(i=0; i<4; i++)
		{	
			if(num[2]<2||num[2]>=3){
				PORTB |= 0x10; 
				_delay_ms(1);
				PORTB &= 0xef; 
				_delay_ms(1);
			}
			PORTC = digit[num[i]];
			PORTG = fnd_sel[i];
			if (i==1)
			PORTC |= 0x80; // 왼쪽에서 3번째 FND에는 소수점(.)을 찍음
			if (i%2)
			_delay_ms(2); // 2번은 2ms 지연
			else
			_delay_ms(3); // 2번은 3ms 지연, 총 10ms 지연
		}
	}
ISR(INT4_vect)
{
		_delay_ms(100); // 스위치 바운스 시간 동안 기다림
		EIFR |= 1 << 4; // 바운스에 의해 생긴 인터럽트는 무효화 - 디바운싱
		
		if ((PINE & 0x10) != 0x00) // SW1이 눌려진 것이 아니면
			return;
		
		if (state == STOP)
			state = GO; // STOP 상태라면 GO 상태로 변경
		else
		{
			state = STOP; // GO 상태라면 STOP 상태로 변경
			stop_temp = read_twi_2byte_nopreset(LM75A_TEMP_REG);
		}
}	
*/


	