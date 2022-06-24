#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "Key.h"
#include "timer.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验1
 跑马灯实验-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

/*下面主函数是使用HAL库函数实现控制IO口输出*/

int main(void)
{
	  uint8_t  KeyCode = 0;
    
      HAL_Init();                    	 	//初始化HAL库    
      Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	  delay_init(72);               		//初始化延时函数，设置systick频率
	  Soft_TimerInit();									//软件定时器初始化
	  LED_Init();							//初始化LED	
	  
	  uart_init(115200);
	  KeyInit();  	 //按键初始化
    HAL_Delay(1000);
	  printf("sys run\r\n");

	//   StartTimer(0,1000);
	//   StartTimer(1,2000);
	//   StartTimer(2,3000);
	  StartAutoTimer(3,10);
	 
	while(1)
	{				
		    
		if (CheckTimer(0))
		{
			printf("timer 0 arrived\r\n"); 
		}

		if (CheckTimer(1))
		{
		printf("timer 1 arrived\r\n"); 
		}

		if (CheckTimer(2))
		{
			printf("timer 2 arrived\r\n"); 
		}

		if (CheckTimer(3))
		{
		 //  printf("111\r\n");
		    KeyScan();		
		}

   		 KeyCode=KeyFifoGet();
		 if(KeyCode != KEY_NONE)
		 {
			 


		     switch(KeyCode)
			 {
				case  KEY1_DOWM: 	printf("K1  press\r\n");		break;
				case  KEY1_UP:	 	printf("K1  lossen\r\n");		break;
				case  KEY1_LONG:	printf("K1  long press\r\n");	break;

				case  KEY2_DOWM:		printf("K2  press\r\n");				break;
				case  KEY2_UP:	 		printf("K2  lossen\r\n");				break;
				case  KEY2_LONG:		printf("K2  long press\r\n");			break;
	
				case  KEY3_DOWM:		break;
				case  KEY3_UP:		  break;
				case  KEY3_LONG:		break;
			 }

			 printf("KeyCode=%u\r\n",KeyCode);

		 }										
				
  }
}













/*下面主函数使用位带操作实现：*/

/*int main(void)
{ 
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               			//初始化延时函数
	LED_Init();							//初始化LED	

	while(1)
	{
         LED0=0;			     	//LED0亮
	     LED1=1;				 	//LED1灭
		 delay_ms(500);
		 LED0=1;					//LED0灭
		 LED1=0;					//LED1亮
		 delay_ms(500);
	 }
}
*/





/*
下面主函数使用直接操作结存器方式实现跑马灯
*/

/*int main(void)
{ 
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	LED_Init();							//初始化LED	

	while(1)
	{
      GPIOB->BSRR=GPIO_PIN_5;     	//LED0亮
	  GPIOE->BSRR=GPIO_PIN_5<<16; 	//LED1灭
	  delay_ms(500);
      GPIOB->BSRR=GPIO_PIN_5<<16; 	//LED0灭
	  GPIOE->BSRR=GPIO_PIN_5;     	//LED1亮
	  delay_ms(500);
	 }
}*/	
