#ifndef __SOFT_TIMER_H
#define  __SOFT_TIMER_H
#include "sys.h"

typedef  struct 
{
    /* data */

   volatile  u8   Mode;         //计数模式      一次性还是多次
   volatile  u8   Flag;         //时间到达标志  时间到达flag=1 
   volatile  u32  Count;        //计数器
   volatile  u32  Preload;      //计数器预装载值
}SoftTimer_T;














#endif







