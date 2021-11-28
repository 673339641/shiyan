#include "key.h"
#include "delay.h"


static  KeyFifo_T   S_tKeyFofo;
static  Key_T       S_tBtn[3]; //3个按键

static u8  IsKey0Down(void);
static u8  IsKey1Down(void);
static u8  IsKey2Down(void);

#define KET_FILTER_TIME 5  //按键滤波次数


//按键初始化函数
static void KeyGpioInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟

    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
	  GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4; //PE2,3,4
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
}


static void KeyFifoInit(void)
{
   u8 i=0;
    /* 按键fifo头尾指针归零 */
    S_tKeyFofo.Read=0;
    S_tKeyFofo.Write=0;

        S_tBtn[0].LongTime=0;    //100ms 长按时间判断时间 为0表示不进行长按判断
        S_tBtn[0].Count=5/2;        //2
        S_tBtn[0].LongCount=0;
        S_tBtn[0].State=0;          //没按下为0
        S_tBtn[0].RepeatCount=0;   //连发的速度
        S_tBtn[0].RepeatSpeed=500;   //连发计数器.500ms发送一次按键值

        S_tBtn[1].LongTime=100;    //100ms 长按时间判断时间 为0表示不进行长按判断
        S_tBtn[1].Count=5/2;        //2
        S_tBtn[1].LongCount=0;
        S_tBtn[1].State=0;          //没按下 为0
        S_tBtn[1].RepeatCount=0;   //连发的速度
        S_tBtn[1].RepeatSpeed=0;   //连发计数器

        S_tBtn[2].LongTime=0;    //100ms 长按时间判断时间 为0表示不进行长按判断
        S_tBtn[2].Count=5/2;        //2
        S_tBtn[2].LongCount=0;
        S_tBtn[2].State=0;          //没按下 为0
        S_tBtn[2].RepeatCount=0;   //连发的速度
        S_tBtn[2].RepeatSpeed=0;   //连发计数器
   
             /* 按键按下判断函数指针*/
            S_tBtn[0].IsKeyDownFunc=IsKey0Down;  //函数指针赋值
            S_tBtn[1].IsKeyDownFunc=IsKey1Down;
            S_tBtn[2].IsKeyDownFunc=IsKey2Down;
}


void KeyInit(void)
{
        KeyGpioInit();
        KeyFifoInit();
}

/*
**********************************************************
* 函 数 名: KeyFifoPut
* 功能说明:  将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
* 形    参:  KeyValue : 按键代码
* 返 回 值: 无
**********************************************************
*/
void KeyFifoPut(u8 KeyValue)
{
        S_tKeyFofo.Buff[S_tKeyFofo.Write++]=KeyValue;
        if(S_tKeyFofo.Write>=KEY_FIFO_SIZE)
        {
            S_tKeyFofo.Write=0;
        }
}




/*
***********************************************************
* 函 数 名: KEY_FIFO_Get
* 功能说明: 从按键FIFO缓冲区读取一个键值。
* 形    参: 无
* 返 回 值: 按键代码
************************************************************
*/
u8  KeyFifoGet(void)
{
     u8  ret;
     if(S_tKeyFofo.Read==S_tKeyFofo.Write)  //头和尾相同
     {
         return  KEY_NONE;
     }
     else
     {
        ret = S_tKeyFofo.Buff[S_tKeyFofo.Read++];
         if(KEY_FIFO_SIZE <= S_tKeyFofo.Read)  //读地址到头
         {
                S_tKeyFofo.Read=0;
         }
            return ret;       
     }

}


static u8  IsKey0Down(void)
{
	   if(KEY0==0)  //按键上拉==0 表示按键按下
		 {
		   return 1;
		 }
		 
		return 0;	       
}

static u8  IsKey1Down(void)
{
    	if(KEY1==0)  //按键上拉==0 表示按键按下
		 {
		   return 1;
		 }
		 
		return 0;	 
}

static u8  IsKey2Down(void)
{
    if(KEY2==0)  //按键上拉==0 表示按键按下
		 {
		   return 1;
		 }
		 
		return 0;	 
}


static void KeyDetect(u8 i)
{

        Key_T *pBtn;  //按键结构体指针
        pBtn=&S_tBtn[i];
       if(pBtn->IsKeyDownFunc())  //调用函数指针
       {
            if( pBtn->Count < KET_FILTER_TIME)
            {
                    pBtn->Count=KET_FILTER_TIME;  //给count赋值
            }
            else if( pBtn->Count < 2*KET_FILTER_TIME)
            {
                    pBtn->Count++;
            }
            else                        //大于100ms
            {
                    if(pBtn->State == 0)
                    {
                        pBtn->State=1;   //按键状态用1 标记用1标记已经按下  
                        KeyFifoPut(3*i+1);  //fifo中压入对应的按键值    短按        
                    }                   
                                            

                if(pBtn->LongTime > 0)   // 如果长按计时设定大于0   按键或者连发二选一
                {  
                 //   printf("LongCount=%u\r\n",pBtn->LongCount);
                    if(pBtn->LongCount < pBtn->LongTime) //
                    {
                            if(++pBtn->LongCount == pBtn->LongTime) //到达长按计时
                            {
                                //printf("bbbb\r\n");
                                KeyFifoPut(3*i+3);  //fifo中压入对应的按键值 长按
                            }
                    }
                }                                
                else     //连发时需要   
                {
                            if(pBtn->RepeatSpeed > 0)   
                            {
                                    if(++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                                    {
                                        pBtn->RepeatCount=0;
                                        KeyFifoPut(3*i+1);  //fifo中压入对应的按键值 连发短按
                                    }
                            }
                    
                }
            }
        }
        else  //按键松开状态
        {
              if(pBtn->Count >= KET_FILTER_TIME) 
              {
                  pBtn->Count = KET_FILTER_TIME;
              } 
              else if (pBtn->Count!=0)
              {
                  pBtn->Count--;
              } 
              else
              {
                    if(pBtn->State==1)
                    {
                        pBtn->State=0;
                        KeyFifoPut(3*i+2);  //fifo中压入对应的按键值 按压下的按键已经松开
                    }                  
              }

              pBtn->Count=0;
              pBtn->LongCount=0;
        }
 }
      

void  KeyScan(void)
{
   	
     for(u8 i=0;i<3;i++)    
     {
        KeyDetect(i);   //扫描三个按键
     }
}
    






/*
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=1;     //按键松开标志
    if(mode==1)key_up=1;    //支持连按
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0)       return KEY0_PRES;
        else if(KEY1==0)  return KEY1_PRES;
        else if(KEY2==0)  return KEY2_PRES;
        else if(WK_UP==1) return WKUP_PRES;          
    }else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1;
    return 0;   //无按键按下
}


*/

