#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//KEY驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/5/25
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//下面的方式是通过位带操作方式读取IO
//#define KEY0        PEin(4) 	//KEY0按键PE4
//#define KEY1        PEin(3) 	//KEY1按键PE3
//#define KEY2        PEin(2)	//KEY2按键PE2
//#define WK_UP       PAin(0) 	//WKUP按键PA0


//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)  //KEY0按键PE4
#define KEY1        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)  //KEY1按键PE3
#define KEY2        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2) 	//KEY2按键PE2
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP按键PA0

#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define WKUP_PRES   4

#define  KEY_FIFO_SIZE 10


typedef  enum
{
   KEY_NONE = 0,

   KEY1_DOWM,  //1
   KEY1_UP,    //2
   KEY1_LONG,  //3

   KEY2_DOWM,  //4
   KEY2_UP,    //5
   KEY2_LONG,  //6

   KEY3_DOWM,  //7
   KEY3_UP,    //8
   KEY3_LONG,  //9
}KeyResult_E;



typedef struct
{
   u8 (*IsKeyDownFunc)(void);  //按键按下检测函数指针
   u8  Count;          //滤波计数器
   u16  LongCount;     //长按计数器
   u16  LongTime;      //按下持续时间 0表示不支持长按
   u8  State;         //当前按键状态
   u8  RepeatSpeed;    //连续按键周期   1=10ms  2=20ms  
   u8  RepeatCount;    //连续按键计数器  每过10ms +1
}Key_T;  //表示一个



typedef struct 
{
   u8  Read;
   u8  Write;
   u8  Buff[KEY_FIFO_SIZE];
}KeyFifo_T;     //_T 表示结构体

void KeyInit(void);
u8 KEY_Scan(u8 mode);
void KeyFifoPut(u8 KeyValue);
 u8  KeyFifoGet(void);
 void  KeyScan(void);
#endif
