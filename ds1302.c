#include "ds1302.h"
/********************************************************************/ 
/*单字节写入一字节数据*/
void Write_Ds1302_Byte(unsigned char dat) 
{
	unsigned char i;
	SCK = 0;
	for (i=0;i<8;i++) 
	{ 
		if (dat & 0x01) 	// 等价于if((addr & 0x01) ==1) 
		{
			SDA_SET;		//#define SDA_SET SDA=1 /*电平置高*/
		}
		else 
		{
			SDA_CLR;		//#define SDA_CLR SDA=0 /*电平置低*/
		}		 
		SCK_SET;
		SCK_CLR;		
		dat = dat >> 1; 
	} 
}
/********************************************************************/ 
/*单字节读出一字节数据*/
unsigned char Read_Ds1302_Byte(void) 
{
	unsigned char i, dat=0;	
	for (i=0;i<8;i++)
	{	
		dat = dat >> 1;
		if (SDA_R) 	  //等价于if(SDA_R==1)    #define SDA_R SDA /*电平读取*/	
		{
			dat |= 0x80;
		}
		else 
		{
			dat &= 0x7F;
		}
		SCK_SET;
		SCK_CLR;
	}
	return dat;
}

/********************************************************************/ 
/*向DS1302 单字节写入一字节数据*/
void Ds1302_Single_Byte_Write(unsigned char addr, unsigned char dat)
{ 

	RST_CLR;			/*RST脚置低，实现DS1302的初始化*/
	SCK_CLR;			/*SCK脚置低，实现DS1302的初始化*/

	RST_SET;			/*启动DS1302总线,RST=1电平置高 */
	addr = addr & 0xFE;	 
	Write_Ds1302_Byte(addr); /*写入目标地址：addr,保证是写操作,写之前将最低位置零*/
	dat = (dat/10 << 4) | (dat%10);
	Write_Ds1302_Byte(dat);	 /*写入数据：dat*/
	RST_CLR;				 /*停止DS1302总线*/
	SDA_CLR;
}

/********************************************************************/ 
/*从DS1302单字节读出一字节数据*/
unsigned char Ds1302_Single_Byte_Read(unsigned char addr) 
{ 
	unsigned char temp;
	RST_CLR;			/*RST脚置低，实现DS1302的初始化*/
	SCK_CLR;			/*SCK脚置低，实现DS1302的初始化*/

	RST_SET;	/*启动DS1302总线,RST=1电平置高 */	
	addr = addr | 0x01;	 
	Write_Ds1302_Byte(addr); /*写入目标地址：addr,保证是读操作,写之前将最低位置高*/
	temp = Read_Ds1302_Byte(); /*从DS1302中读出一个字节的数据*/
	temp = temp/16*10 + temp%16;
	RST_CLR;	/*停止DS1302总线*/
	SDA_CLR;
	return temp;
}

void Ds_Control(TIME_t *time, uchar command)
{
	uchar i, add;
	
	Ds1302_Single_Byte_Write(ds1302_control_addr, 0x00);
	
	if(command == WRITE)
	{
		add = ds1302_sec_addr;
		for(i = 0; i < 7; i++,add += 2)
		{
			Ds1302_Single_Byte_Write(add, time->set[i]);
		}
		time->runFlag = 1;
	}
	else if(command == READ)
	{
		add = ds1302_sec_addr + 1;
		for(i = 0; i < 7; i++,add += 2)
		{
			time->read[i] = Ds1302_Single_Byte_Read(add);
		}
	}
	else if(command == RUN)
	{
		Ds1302_Single_Byte_Write(ds1302_sec_addr, time->read[0]);
		time->runFlag = 1;
	}
	else if(command == STOP)
	{
		time->read[0] = Ds1302_Single_Byte_Read(ds1302_sec_addr+1);
		Ds1302_Single_Byte_Write(ds1302_sec_addr, 0x80);
		time->runFlag = 0;
	}
	
	Ds1302_Single_Byte_Write(ds1302_control_addr, 0x80);
}