/*LCD1602����*/

#include<reg52.h>

#define DB P0
sbit RW = P3^6;
sbit RS = P3^5;
sbit LCDEN = P3^4;

/*æ״̬���*/
void LCD1602_mang(void)
{
	unsigned char sta;

	RW = 1;
	RS = 0;

	do
	{
		LCDEN = 1;
		sta = DB;
		LCDEN = 0;		
	}while(sta & 0x80);
}

/*дһ�ֽ�����*/
void LCD1602_cmd(unsigned char cmd)
{
	LCD1602_mang();
	RW = 0;
	RS = 0;
	DB = cmd;
	LCDEN = 1;
	LCDEN = 0;
}

/*дһ�ֽ�����*/
void LCD1602_data(unsigned char lcd_data)
{
	LCD1602_mang();
	RW = 0;
	RS = 1;
	DB = lcd_data;
	LCDEN = 1;
	LCDEN = 0;
}


/*����RAM��ַ*/
void LCD1602_xy(unsigned char x,unsigned char y)
{
	unsigned char dizhi;
	if(y == 0)
		dizhi = x + 0x00;
	else
		dizhi = x + 0x40;
	LCD1602_cmd(dizhi | 0x80);
	
}

/*д�ַ���*/
void LCD1602_str(unsigned char x,unsigned char y,unsigned char *str)
{
	LCD1602_xy(x,y);

	while(*str != '\0')
	{
		LCD1602_data(*str++);
	}
}

/*1602��ʼ��*/
void LCD1602_chushi(void)
{
	 LCD1602_cmd(0x38);
	 LCD1602_cmd(0x0c);
	 LCD1602_cmd(0x06);
	 LCD1602_cmd(0x01);
}