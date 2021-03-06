#ifndef __SYS_H__
#define __SYS_H__

#include <IAP15F2K61S2.H>

#define uchar unsigned char
#define uint unsigned int
#define uint16 unsigned short

//IO��
#define NONE 0x1f
#define LEDS 0x80
#define OTHS 0xA0
#define BITS 0xC0
#define SEGS 0xE0

#define RELAY 0x10
#define BUZZ  0x40

//TAB��
#define DIS_POINT			10
#define DIS_NONE 			20
#define DIS_SHORT_HORIZON	21
#define DIS_C				22
#define DIS_F				23
#define DIS_U				24
#define DIS_L				25

#define ON  1
#define OFF 0

#define TIME1FRE 1
#define TIME1LEN 2

typedef struct 
{
	uchar status;
	uchar bit_temp;
	uchar count[3];
	uchar flag[3];
}LED_t;

typedef struct 
{
	uchar disbit;
	uchar disbuff[8];
}SMG_t;

typedef struct 
{
	uchar init;
	uchar done;
}TIME1FLAG_t;

extern LED_t Led;
extern SMG_t Smg;
extern TIME1FLAG_t Time1Flag;

void sysInit(void);
void IOinit(void);
void Timer0Init(void);
void Timer1Init(uchar command);
void Timer2Init(void);
void Timer2ReInit(void);//���벶��
void UartInit(void);
void display(void);
void led_control(uchar ledbit, uchar command);
void relay_or_buzzer(uchar choice, uchar command);
void trans_char(char t);
void trans_str(char t[]);

#endif
