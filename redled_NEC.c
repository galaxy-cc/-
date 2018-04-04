/*
38K����Э��NEC
*/

#include<reg52.h>

sbit HONGWAI = P3^2;     //�ⲿ�ж�INT0

bit hw_data_biaozhi = 0;  //����������ɱ�־

unsigned char hw_data_buf[4]; //����������ݻ�����

/*������ճ�ʼ��*/

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

/*���͵�ƽ����ʱ��*/

unsigned int down_time(void)
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	
	while(!HONGWAI)
	{
		if(TH1 > 0x40)		 //�� T1 ����ֵ���� 0x4000�����ߵ�ƽ����ʱ�䳬��Լ 18ms ʱ��
		{						//ǿ���˳�ѭ������Ϊ�˱����ź��쳣ʱ���������������
			break;
		}
	}
	TR1 = 0;

	return (TH1 * 256 + TL1);
}

/*���ߵ�ƽ����ʱ��*/

unsigned int high_time(void)
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	
	while(HONGWAI)
	{
		if(TH1 > 0x40)		 //�� T1 ����ֵ���� 0x4000�����ߵ�ƽ����ʱ�䳬��Լ 18ms ʱ��
		{						//ǿ���˳�ѭ������Ϊ�˱����ź��쳣ʱ���������������
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
	if((time < 7833) || (time > 8755))	  //ʱ���ж���ΧΪ 8.5��9.5ms
	{
		IE0 = 0;
		return ;
	}

	time = high_time();
	if((time < 3686)||(time > 4608))   //ʱ���ж���ΧΪ 4.0��5.0ms
	{
		IE0 = 0;
		return;
	}

	for(i = 0;i < 4;i++)		 //ѭ������ 4 ���ֽ�
	{
		for(j = 0;j < 8;j++)		//ѭ�������ж�ÿ�ֽڵ� 8 �� bit
		{
			time = down_time();
			if((time < 313)||(time > 718))	 //ʱ���ж���ΧΪ 340��780us
			{
				IE0 = 0;
				return;
			}
			time = high_time();
			if((time > 313) && (time < 718)) //ʱ���ж���ΧΪ 340��780us
			{
				try >>= 1;
			}
			else if((time > 1345) && (time < 1751))		 //ʱ���ж���ΧΪ 1460��1900us
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