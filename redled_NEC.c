/*
38K红外协议NEC
*/

#include<reg52.h>

sbit HONGWAI = P3^2;     //外部中断INT0

bit hw_data_biaozhi = 0;  //接收数据完成标志

unsigned char hw_data_buf[4]; //红外接收数据缓冲区

/*红外接收初始化*/

void IR_chushi(void)
{
	HONGWAI = 1;
	TMOD &= 0x0f;
	TMOD |= 0x10;
	TR1 = 0;
	ET1 = 0;
	IT0 = 1;
	EX0 = 1;
}

/*检测低电平持续时间*/

unsigned int down_time(void)
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	
	while(!HONGWAI)
	{
		if(TH1 > 0x40)		 //当 T1 计数值大于 0x4000，即高电平持续时间超过约 18ms 时，
		{						//强制退出循环，是为了避免信号异常时，程序假死在这里
			break;
		}
	}
	TR1 = 0;

	return (TH1 * 256 + TL1);
}

/*检测高电平持续时间*/

unsigned int high_time(void)
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	
	while(HONGWAI)
	{
		if(TH1 > 0x40)		 //当 T1 计数值大于 0x4000，即高电平持续时间超过约 18ms 时，
		{						//强制退出循环，是为了避免信号异常时，程序假死在这里
			break;
		}
	}
	TR1 = 0;

	return (TH1 * 256 + TL1);
}

void hongwai_zhongduan() interrupt 0
{
	unsigned char try;
	unsigned char i,j;
	unsigned int time;
	
	time = down_time(); 
	if((time < 7833) || (time > 8755))	  //时间判定范围为 8.5～9.5ms
	{
		IE0 = 0;
		return ;
	}

	time = high_time();
	if((time < 3686)||(time > 4608))   //时间判定范围为 4.0～5.0ms
	{
		IE0 = 0;
		return;
	}

	for(i = 0;i < 4;i++)		 //循环接收 4 个字节
	{
		for(j = 0;j < 8;j++)		//循环接收判定每字节的 8 个 bit
		{
			time = down_time();
			if((time < 313)||(time > 718))	 //时间判定范围为 340～780us
			{
				IE0 = 0;
				return;
			}
			time = high_time();
			if((time > 313) && (time < 718)) //时间判定范围为 340～780us
			{
				try >>= 1;
			}
			else if((time > 1345) && (time < 1751))		 //时间判定范围为 1460～1900us
			{
				try >>= 1;
				try |= 0x80;
			}
			else
			{
				IE0 = 0;
				return;
			}
		}
		hw_data_buf[i] = try;
	}
	hw_data_biaozhi = 1;
	IE0 = 0;
}