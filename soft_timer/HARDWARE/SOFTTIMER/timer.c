#include "timer.h"
#include "usart.h"
//#include "SEGGER_RTT.h"


/* 这2个全局变量转用于 bsp_DelayMS() 函数 */
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t  s_ucTimeOutFlag = 0;

/* 定于软件定时器结构体变量 */
static SOFT_TMR s_tTmr[TMR_COUNT];

/*
	全局运行时间，单位1ms。全局运行时间每1ms增1。
	最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*/
__IO int32_t g_iRunTime = 0;

static void SoftTimerDec(uint8_t _id);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitTimer
*	功能说明: 配置systick中断，并初始化软件定时器变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void Soft_TimerInit(void)
{
	uint8_t i;

	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
	}

	/*
		配置systic中断周期为1ms，并启动systick中断。

    	SystemCoreClock 是固件中定义的系统内核时钟，对于STM32F4XX,一般为 168MHz

    	SysTick  中断时间的计算
	
		SysTick 定时器的计数器是向下递减计数的，计数一次的时间 T_DEC =1/CLK_AHB ，当重装
		载寄存器中的值 VALUE_LOAD 减到 0 的时候，产生中断，可知中断一次的时间
		T_INT =VALUE_LOAD *  T_DEC 中断= VALUE_LOAD /CLK_AHB ，其中 CLK_AHB  = SystemCoreClock = 168MHZ。
		如果设置为168，那中断一次的时间 T_INT =168/168MHz = 1us。不过 1us 的中断没啥意义，整个程序的重心
		都花在进出中断上了，根本没有时间处理其他的任务。
	    所以我们设置重装载值为SystemCoreClock / 1000 =168 000，那中断一次的时间 T_INT =168 0000/168MHz = 1ms
	
		因此：
	
		SysTick_Config() 函数的形参表示内核时钟多少个周期后触发一次Systick定时中断.
	    	-- SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
	    	-- SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
	    	-- SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us

    	对于常规的应用，我们一般取定时周期1ms。对于低速CPU或者低功耗应用，可以设置定时周期为 10ms

	
    */
			SysTick_Config(SystemCoreClock / 1000);/*SystemCoreClock / 1000是重装载寄存器的值LOAD*/
	
}

/*
*********************************************************************************************************
*	函 数 名: StartTimer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 无
*********************************************************************************************************
*/
void StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		while(1); /* 参数异常，死机等待看门狗复位 */
	}
	/*结构体变量赋值前后做了开关中断操作。因为此结构体变量在滴答定时器中断
	里面也要调用，防止变量赋值出问题。*/
	__set_PRIMASK(1); 				 //关闭中断
	
	s_tTmr[_id].Count = _period;		/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 1次性工作模式 */
	
	__set_PRIMASK(0);   				/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: StartAutoTimer
*	功能说明: 启动一个自动定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 无
*********************************************************************************************************
*/
void StartAutoTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		while(1); /* 参数异常，死机等待看门狗复位 */
	}
	/*结构体变量赋值前后做了开关中断操作。因为此结构体变量在滴答定时器中断
	里面也要调用，防止变量赋值出问题。*/
		__set_PRIMASK(1); 				 //关闭中断

	s_tTmr[_id].Count = _period;	    /* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* 自动工作模式 */

	__set_PRIMASK(0);   				/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StopTimer
*	功能说明: 停止一个定时器
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 无
*********************************************************************************************************
*/
void StopTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		while(1); /* 参数异常，死机等待看门狗复位 */
	}
	/*结构体变量赋值前后做了开关中断操作。因为此结构体变量在滴答定时器中断
	里面也要调用，防止变量赋值出问题。*/
	__set_PRIMASK(1); 				 //关闭中断

	s_tTmr[_id].Count = 0;				/* 把结构体变量中的计数器和时间到标志都置位成 0，从而让软件定时器停止运行 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 自动工作模式 */

 __set_PRIMASK(0);   				/* 开中断 */
}




/*
*********************************************************************************************************
*	函 数 名: bsp_CheckTimer
*	功能说明: 检测定时器是否超时
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
*********************************************************************************************************
*/
uint8_t CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)/*检查 ID 是否有效*/
	{
		return 0;
	}
	/*判断时间到标志值 Flag 是否置位，如果置位表示时间已经到，如果为 0，表示时间还没有到*/
	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetRunTime
*	功能说明: 获取CPU运行时间，单位1ms。最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*	形    参:  无
*	返 回 值: CPU运行时间，单位1ms
*********************************************************************************************************
*/
int32_t GetRunTime(void)
{
	int32_t runtime;

	__set_PRIMASK(1);  	/* 关中断 */

	runtime = g_iRunTime;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

	__set_PRIMASK(0);  		/* 开中断 */

	return runtime;
}

/*
*********************************************************************************************************
*	函 数 名: CheckRunTime
*	功能说明: 计算当前运行时间和给定时刻之间的差值。处理了计数器循环。
*	形    参:  _LastTime 上个时刻
*	返 回 值: 当前时间和过去时间的差值，单位1ms
*********************************************************************************************************
*/
int32_t CheckRunTime(int32_t _LastTime)
{
	int32_t now_time;
	int32_t time_diff;

	__set_PRIMASK(1);  	/* 关中断 */

	now_time = g_iRunTime;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

	__set_PRIMASK(0);  		/* 开中断 */
	
	if (now_time >= _LastTime)
	{
		time_diff = now_time - _LastTime;
	}
	else
	{
		time_diff = 0x7FFFFFFF - _LastTime + now_time ;  /*给定时间大于现在时间, 还没走到现在的时间*/
	}

	return time_diff;
}

/*
*********************************************************************************************************
*	函 数 名: SysTick_ISR
*	功能说明: SysTick中断服务程序，每隔1ms进入1次。被SysTick_Handler调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void RunPer1ms(void);
extern void RunPer10ms(void);
void SysTick_ISR(void)
{
	static uint8_t s_count = 0;
	uint8_t _id;
	
	/* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
	if (s_uiDelayCount > 0)/* s_uiDelayCount为毫秒延时的时间 */
	{
		if (--s_uiDelayCount == 0)
		{
			s_ucTimeOutFlag = 1;/* 延时时间到达，标志位置位为1*/
		}
	}

	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	for (_id = 0; _id < TMR_COUNT; _id++)
	{
		SoftTimerDec(_id);
	}

	/* 全局运行时间每1ms增1 */
	g_iRunTime++;
	if (g_iRunTime == 0x7FFFFFFF)	/* 这个变量是 int32_t 类型，最大数为 0x7FFFFFFF */
	{
		g_iRunTime = 0;
	}

//	RunPer1ms();		/* 每隔1ms调用一次此函数，此函数在 bsp.c */

	if (++s_count >= 10)
	{
		s_count = 0;
			//RunPer10ms();	/* 每隔10ms调用一次此函数，此函数在 bsp.c */
	}
}
/*
*********************************************************************************************************
*	函 数 名: SoftTimerDec
*	功能说明: 每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参:  i : 定时器的ID
*	返 回 值: 无
*********************************************************************************************************
*/
static void SoftTimerDec(uint8_t _id)
{
	SOFT_TMR  * tmr;
	tmr = &s_tTmr[_id];/* 每个定时器都有自己的结构体，将对应定时器的地址赋值给结构体指针变量tmr 
	                      才能访问对应定时器的成员变量，因为是结构体指针变量，所以访问要用 —> */
	if (tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--tmr->Count == 0)
		{
			tmr->Flag = 1; /* Flag = 1 在检查定时器时间中会用到 */

			/* 如果是自动模式，则自动重装计数器 */
			if(tmr->Mode == TMR_AUTO_MODE)
			{
				tmr->Count = tmr->PreLoad;
			}
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: SysTick_Handler
*	功能说明: 系统嘀嗒定时器中断服务程序。启动文件中引用了该函数。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void SysTick_Handler(void)
{
  HAL_IncTick();	
  SysTick_ISR();	/* 滴答定时中断服务程序 */	
}


/*
*********************************************************************************************************
*    函 数 名: Delay_us
*    功能说明: us级延迟。 必须在systick定时器启动后才能调用此函数。
*    形    参:  n : 延迟长度，单位1 us
*    返 回 值: 无
*********************************************************************************************************
*/
void Delay_us(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
	  reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* 刚进入时的计数器值 */

    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICK是一个递减的计数器 */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }
} 




/*
*********************************************************************************************************
*	函 数 名: Delay_ms
*	功能说明: ms级延迟，延迟精度为正负1ms
*	形    参:  n : 延迟长度，单位1 ms。 n 应大于2
*	返 回 值: 无
*********************************************************************************************************
*/
void Delay_ms(uint32_t n)
{
	if (n == 0)
	{
		return;
	}
	else if (n == 1)
	{
		n = 2;
	}
	__set_PRIMASK(1);  			/* 关中断 */

	s_uiDelayCount = n;        /* s_uiDelayCount为延时时间 */
	s_ucTimeOutFlag = 0;

	__set_PRIMASK(0);  				/* 开中断 */
	while (1)
	{
		/*
			等待延迟时间到，待延迟时间到后s_ucTimeOutFlag在SysTick_ISR函数中被置为1
			注意：编译器认为 s_ucTimeOutFlag = 0，所以可能优化错误，因此 s_ucTimeOutFlag 变量必须申明为 volatile
		*/
		if (s_ucTimeOutFlag == 1)
		{
			break;
		}
	}
}

/***************************** 智果芯 www.zhiguoxin.cn (END OF FILE) *********************************/










