
/*单线单片DS18B20驱动*/

#include<reg52.h>
#include<intrins.h>

sbit DS18B20 = 	P2^2;		   //DS18B20数据引脚


/*延时函数us*10*/
void dalay_us_10(unsigned int us_10)
{
	us_10 *= 10;
	for(;us_10 > 0;us_10--)
	{
		_nop_();
	}
}

/*存在脉冲检测函数*/
bit DS18B20_zt(void)
{
	bit zt;

	EA = 0;							 //关总中断以免打扰 DS18B20时序要求严格
	DS18B20 = 0;					 //拉低通信引脚持续500us
	dalay_us_10(50);
	DS18B20 = 1;					 //释放通信引脚等待18B20存在脉冲状态
	dalay_us_10(6);					 

	zt = DS18B20;
	while(!DS18B20);				 //等待存在脉冲结束
	EA = 1;

	return zt;				  //18B20存在返回低电平 所以取反
}


/*对DS18B20写1字节命令*/

void  DS18B20_write(unsigned char cmd)
{
	unsigned char mon;

	EA = 0;
	for(mon = 0x01; mon != 0;mon <<= 1)
	{
		DS18B20 = 0;					   //大于1us的通信引脚拉低
		_nop_();
		_nop_();
		
		if((mon & cmd) == 0)
			DS18B20 = 0;
		else
			DS18B20 = 1;
		dalay_us_10(6);					  //等待18B20读取通信引脚状态
		DS18B20 = 1;					  //释放通信引脚
	}
	EA = 1;
}

/*读DS18B20数据*/

unsigned char DS18B20_read(void)
{
	unsigned char mon;
	unsigned char dat = 0;

	EA = 0;

	for(mon = 0x01;mon != 0;mon <<= 1)
	{
		
		DS18B20 = 0;				  //拉低通信引脚>1us
		_nop_();
		_nop_();
		DS18B20 = 1;				  //释放通信引脚等待18B20反馈
		_nop_();
		_nop_();
		if((mon & DS18B20) != 0)
			dat |= mon; 
	}

	EA = 1;

	return dat;
}


/*DS18B20启动一次温度转换*/

bit DS18B20_start(void)
{
	bit ack;

	ack = DS18B20_zt();

	if(ack == 0)
	{
		DS18B20_write(0xcc);	//跳过ROM操作
		DS18B20_write(0x44);	//启动温度转换
	}

	return ~ack;			   //18B20应答信号为低电平 所以取反
}

/*读18B20转换的温度值*/

bit DS18B20_get(int *tmp)
{
	bit ack;
	unsigned char LSB,MSB; 

	ack = DS18B20_zt();
	
	if(ack == 0)
	{
		DS18B20_write(0xcc);		   //跳过ROM操作
		DS18B20_write(0xbe);		   //读18B20暂存寄存器
		LSB = DS18B20_read();
		MSB = DS18B20_read();	
	}
	*tmp = ((int) MSB << 8)	+ LSB;

	return ~ack;
}