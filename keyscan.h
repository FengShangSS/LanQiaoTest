#ifndef __KEYSCAN_H__
#define __KEYSCAN_H__

#include "sys.h"

#define KEYDEBOUNCE 5

#define LONG		1
#define SHORT		0
#define RELEASE		2

typedef struct
{
	//��ʼ��״̬
	uchar lastRead;
	uchar currRead;
	
	//һ�δ������״̬
	uchar curr;
	uchar last;
	uchar validCount;
	
	//���δ������״̬
	uchar Down;
	uchar Release;
	uchar Continue;
		
	uchar pressTimeCount;
	uchar mode;
	
	uchar sacnCount;
	uchar timeOk;
	uint longPressCount;
	uchar longPressFlag;
}KEY_t;

typedef struct
{
	uchar dat[4];
}OUTPUT_t;

extern KEY_t Key;

void keyScan(void);
void keyProcess(KEY_t *in, OUTPUT_t *out);
#endif