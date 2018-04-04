
/*���ߵ�ƬDS18B20����*/

#include<reg52.h>
#include<intrins.h>

sbit DS18B20 = 	P2^2;		   //DS18B20��������


/*��ʱ����us*10*/
void dalay_us_10(unsigned int us_10)
{
	us_10 *= 10;
	for(;us_10 > 0;us_10--)
	{
		_nop_();
	}
}

/*���������⺯��*/
bit DS18B20_zt(void)
{
	bit zt;

	EA = 0;							 //�����ж�������� DS18B20ʱ��Ҫ���ϸ�
	DS18B20 = 0;					 //����ͨ�����ų���500us
	dalay_us_10(50);
	DS18B20 = 1;					 //�ͷ�ͨ�����ŵȴ�18B20��������״̬
	dalay_us_10(6);					 

	zt = DS18B20;
	while(!DS18B20);				 //�ȴ������������
	EA = 1;

	return zt;				  //18B20���ڷ��ص͵�ƽ ����ȡ��
}


/*��DS18B20д1�ֽ�����*/

void  DS18B20_write(unsigned char cmd)
{
	unsigned char mon;

	EA = 0;
	for(mon = 0x01; mon != 0;mon <<= 1)
	{
		DS18B20 = 0;					   //����1us��ͨ����������
		_nop_();
		_nop_();
		
		if((mon & cmd) == 0)
			DS18B20 = 0;
		else
			DS18B20 = 1;
		dalay_us_10(6);					  //�ȴ�18B20��ȡͨ������״̬
		DS18B20 = 1;					  //�ͷ�ͨ������
	}
	EA = 1;
}

/*��DS18B20����*/

unsigned char DS18B20_read(void)
{
	unsigned char mon;
	unsigned char dat = 0;

	EA = 0;

	for(mon = 0x01;mon != 0;mon <<= 1)
	{
		
		DS18B20 = 0;				  //����ͨ������>1us
		_nop_();
		_nop_();
		DS18B20 = 1;				  //�ͷ�ͨ�����ŵȴ�18B20����
		_nop_();
		_nop_();
		if((mon & DS18B20) != 0)
			dat |= mon; 
	}

	EA = 1;

	return dat;
}


/*DS18B20����һ���¶�ת��*/

bit DS18B20_start(void)
{
	bit ack;

	ack = DS18B20_zt();

	if(ack == 0)
	{
		DS18B20_write(0xcc);	//����ROM����
		DS18B20_write(0x44);	//�����¶�ת��
	}

	return ~ack;			   //18B20Ӧ���ź�Ϊ�͵�ƽ ����ȡ��
}

/*��18B20ת�����¶�ֵ*/

bit DS18B20_get(int *tmp)
{
	bit ack;
	unsigned char LSB,MSB; 

	ack = DS18B20_zt();
	
	if(ack == 0)
	{
		DS18B20_write(0xcc);		   //����ROM����
		DS18B20_write(0xbe);		   //��18B20�ݴ�Ĵ���
		LSB = DS18B20_read();
		MSB = DS18B20_read();	
	}
	*tmp = ((int) MSB << 8)	+ LSB;

	return ~ack;
}