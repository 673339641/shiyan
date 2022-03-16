#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "time.h"
#include "stdlib.h"
#include "string.h"
#include "lua_operation.h"


extern  char lua_test1[];
extern  char lua_test2[];
extern  char lua_test[];

u8 ScriptRunBuff[500];
	
int main(void)
{
	  u8 TempBuff[1024]={0};
	
		HAL_Init();                    	 			  //初始化HAL库    
		Stm32_Clock_Init(RCC_PLL_MUL9);   			//设置时钟,72M
		delay_init(72);               				  //初始化延时函数
		uart_init(115200); 
		LED_Init();
		Log_Printf("sys run\r\nscript size=%u\r\n",sizeof((u8 *)TempBuff));	
		do_file_script();
	  memcpy(ScriptRunBuff,lua_test,500);
		Log_Printf("ScriptRunBuff=\r\n%s\r\n",ScriptRunBuff);  
    while(1)
    {

			 if(GetUart1Data(TempBuff))
			 {
				   Log_Printf("LEN=%u REC=\r\n%s\r\n",Uart1RecStruct.Len,TempBuff);
				   memset(ScriptRunBuff,0,500);
					 memcpy(ScriptRunBuff,TempBuff,500);			 
			 }
				LuaRun((char *)ScriptRunBuff);	
		 

		/*	
          key=KEY_Scan(0);            //按键扫描
					switch(key)
					{				 
						case WKUP_PRES:			//控制LED0,LED1互斥点亮
							LED1=!LED1;
							LED0=!LED1;
							break;
						case KEY2_PRES:			//控制LED0翻转
							LED0=!LED0;
							break;
						case KEY1_PRES:			//控制LED1翻转	 
							LED1=!LED1;
							break;
						case KEY0_PRES:			//同时控制LED0,LED1翻转 
							LED0=!LED0;
							LED1=!LED1;
							break;
					}
							delay_ms(10);
		 */
	}
}







time_t time(time_t * time)
{
    return 0;
}

void exit(int status)
{
  
}

int system(const char * string)
{
  return 0;
}










