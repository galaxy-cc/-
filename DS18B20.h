

extern bit DS18B20_zt(void);  /*存在脉冲检测函数*/

extern bit DS18B20_start(void);		  /*DS18B20启动一次温度转换*/

extern bit DS18B20_get(int *tmp);	   /*读18B20转换的温度值*/