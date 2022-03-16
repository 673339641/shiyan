#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 



#define USART_REC_LEN  			1024  		//定义最大接收字节数 200
typedef struct
{
   u8  Buff[USART_REC_LEN];
   u16  Len;
	 u8   State;
}UartRecStruct;
	  	
extern UartRecStruct  Uart1RecStruct;
extern UART_HandleTypeDef UART1_Handler; 	//UART句柄





void uart_init(u32 bound);
u8 Usart1Send(u8 *buff,  u16 len);
u8 GetUart1Data(u8 *buff);


#define  DEBUG  

#ifdef   DEBUG
#define  Log_Printf   printf
#endif





#endif


