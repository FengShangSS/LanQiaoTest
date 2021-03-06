#include "sys.h"

uchar tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,\
				0xc0-0x80,0xf9-0x80,0xa4-0x80,0xb0-0x80,0x99-0x80,\
				0x92-0x80,0x82-0x80,0xf8-0x80,0x80-0x80,0x90-0x80,\
				0xff,0xbf,0xc6,0x8E,0xC1,0xC7};

SMG_t Smg = {0, 11,11,11,11,11,11,11,11};
LED_t Led = {0xFF, 0x00, 0,0,0, 0,0,0};
LED_t RelayOrBuzz = {0x00, 0x00, 0,0,0, 0,0,0};
TIME1FLAG_t Time1Flag = {0x00, 0x00};

void sysInit(void)
{
	IOinit();
	Timer0Init();
//	Timer2Init();
	UartInit();
	ES = 1;
	EA = 1;
}

void IOinit(void)
{
	P2 &= NONE;
	P2 |= LEDS;P0 = 0xFF;P2 &= NONE; //LED   VCC
	P2 |= OTHS;P0 = 0x00;P2 &= NONE; //RELAY & BUZZER
	P2 |= BITS;P0 = 0xFF;P2 &= NONE; //BIT   
	P2 |= SEGS;P0 = 0XFF;P2 &= NONE; //SEG   VCC
}

void Timer0Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;		//使能定时器0中断
}


void Timer1Init(uchar command)		//2毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
//	TL1 = 0x9A;		//设置定时初值
//	TH1 = 0xA9;		//设置定时初值
//	TF1 = 0;		//清除TF1标志
//	TR1 = 1;		//定时器1开始计时
//	ET1 = 1;		//使能定时器1中断
	if(command == TIME1FRE)
	{
//		AUXR |= 0x40;		//定时器时钟1T模式
//		TMOD &= 0x0F;
		TMOD |= 0x50;	//设置定时器模式
		IT1 = 1;		//下降沿触发
		TL1 = 0x00;		//设置定时初值
		TH1 = 0x00;		//设置定时初值
		TR1 = 1;		//定时器1开始计时
		Time1Flag.done = TIME1FRE;
		led_control(2, ON);
		led_control(3, OFF);
	}
	else if(command == TIME1LEN)
	{
//		AUXR |= 0x40;		//定时器时钟1T模式
//		TMOD &= 0x0F;
		TMOD |= 0x10;	//设置定时器模式
		TL1 = 0x00;		//设置定时初值
		TH1 = 0x00;		//设置定时初值
		Time1Flag.done = TIME1LEN;
		led_control(2, OFF);
		led_control(3, ON);
	}
	else
	{
		Time1Flag.done = 0;
		led_control(2, OFF);
		led_control(3, OFF);
	}	
}

void Timer2Init(void)		//@11.0592MHz//输入捕获
{
	AUXR |= 0x04;		//定时器时钟1T模式
	AUXR |= 0x08;
	T2L = 0x00;		//设置定时初值
	T2H = 0x00;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
}

void Timer2ReInit(void)//输入捕获
{
	AUXR &= ~0x10;	//关计时
	T2L = 0x00;		//设置定时初值
	T2H = 0x00;		//设置定时初值
	AUXR |= 0x10;	//开计时
}

void UartInit(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE0;		//设定定时初值
	T2H = 0xFE;		//设定定时初值
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x10;		//启动定时器2
}



void display()
{
	P2 |= SEGS;
	P0 = 0xFF;
	P2 &= NONE;
	
	P2 |= BITS;
	P0 = 0x01 << Smg.disbit;
	P2 &= NONE;
	
	P2 |= SEGS;
	P0 = tab[Smg.disbuff[Smg.disbit]];
	P2 &= NONE;
	
	if(++Smg.disbit == 8)
		Smg.disbit = 0;
}

void led_control(uchar ledbit, uchar command)
{
	if(ledbit > 7 || command > 1)
		return;
	
	Led.bit_temp = (~Led.status & (0x01 << ledbit));
	
	if(Led.bit_temp == 0x00 && command == ON)
	{
		Led.status &= ~(0x01 << ledbit);
		EA = 0;
		P2 |= LEDS;
		P0 = Led.status;
		P2 &= NONE;
		EA = 1;
	}
	else if(Led.bit_temp > 0x00 && command == OFF)
	{
		Led.status |= (0x01 << ledbit);
		EA = 0;
		P2 |= LEDS;
		P0 = Led.status;
		P2 &= NONE;
		EA = 1;
	}
}

void relay_or_buzzer(uchar choice, uchar command)
{
	if((choice!=RELAY && choice!=BUZZ) || command > 1)
	{
		return;
	}	

	RelayOrBuzz.bit_temp = (RelayOrBuzz.status & choice);

	if(RelayOrBuzz.bit_temp == 0x00 && command == ON)
	{
		RelayOrBuzz.status |= choice;
		EA = 0;
		P2 |= OTHS;
		P0 = RelayOrBuzz.status;
		P2 &= NONE;
		EA = 1;
	}
	else if(RelayOrBuzz.bit_temp > 0x00 && command == OFF)
	{
		RelayOrBuzz.status &= ~choice;
		EA = 0;
		P2 |= OTHS;
		P0 = RelayOrBuzz.status;
		P2 &= NONE;
		EA = 1;
	}
}

void trans_char(char t)
{
	SBUF = t;
	while(!TI);
	TI = 0;
}
 
void trans_str(char t[])
{
	char *p;
	p = t;
	while(*p != '\0')
	{
		trans_char(*p);
		p++;
	}	
}

