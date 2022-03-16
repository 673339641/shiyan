#include  "lua_operation.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include  "led.h"
#include  "delay.h"
#include  "usart.h"


lua_State *L;

 char lua_test[500] = { 
    "print(\"Hello,I am Jack lu!\\n--this is newline printf\")\n"
};

 char lua_test1[] = { 
		"  led0_switch(0)"
		"  led1_switch(0)"
	  "  delay(500)"
		"  led0_switch(1)"
		"  led1_switch(1)"
	  "  delay(500)"
  
	
//	
//	"while(io_in(3,0)==0.0)"
//	"	do"
//	"	pwm_out(7,10,5,30000,1)"
//	"	end"
 
//				"led0_switch(0)"
//				"led1_switch(0)"
//				"delay(1000)"
//	      "i=i+1"
//			"print(\"i=\n\",i)"
//			"led0_switch(1)"
//			"led1_switch(1)"
    	
};


const char lua_test2[] = { 
  	
		"  led0_switch(0)"
		"  led1_switch(0)"
	  "  delay(2000)"
		"  led0_switch(1)"
		"  led1_switch(1)"
	  "  delay(2000)"

	
//	
//	"while(io_in(3,0)==0.0)"
//	"	do"
//	"	pwm_out(7,10,5,30000,1)"
//	"	end"
 
//				"led0_switch(0)"
//				"led1_switch(0)"
//				"delay(1000)"
//	      "i=i+1"
//			"print(\"i=\n\",i)"
//			"led0_switch(1)"
//			"led1_switch(1)"
    	
};




//LED0 ����
static int  LuaLed0Switch(lua_State *L)
{  
	  u8  State=lua_tonumber(L, 1);
     if(State > 2)   //״̬����
		 {
		    return 0;
		 }
		 LED0_Switch(State);
		 Log_Printf("\r\nLED 0=%u\r\n",State);
		 return 1;
}

//LED1����
static int  LuaLed1Switch(lua_State *L)
{
	   u8  State=lua_tonumber(L, 1);
     if(State > 2)   //״̬����
		 {
		    return 0;
		 }
		 LED1_Switch(State);
		 Log_Printf("\r\nLED 1=%u\r\n",State);
		 return 1;

}


static int  LuaDelay(lua_State *L)
{
    u16  time=lua_tonumber(L, 1);
		Log_Printf("\r\ndelay time =%u\r\n",time);
    delay_ms(time);
		return 1;
}





const char* LED;
//C����ע���
static const struct luaL_Reg mylib[] =
{
	{"led0_switch",LuaLed0Switch},
	{"led1_switch",LuaLed1Switch},
	{"delay",LuaDelay},
  {NULL,NULL}
};



//�Ǽ�C������lua
static u8 luaopen_mylib(lua_State *L)
{ 
#if 1
     luaL_setfuncs(L,mylib, 0);  //ע�ắ��
		 lua_setglobal(L,"LED");
#else
     luaL_newlib(L,mylib);
#endif
     return 1;
}



//����lua����
u8  do_file_script(void)
{
		int ret = 0;
		L =luaL_newstate();/* ����Lua���л��� */
		if(L == NULL)
			return 0;
		 luaopen_base(L);
		 luaopen_mylib(L);//ע��C �⺯��
}


u8 LuaRun(char *Script)
{ 
	int ret = 0;
	ret = luaL_dostring(L, Script);/* ����Lua�ű� */
	if (ret != 0)
	{
		 return 0;
	}
}

//�ر�lua�ű�
void LuaClose(char * Script)
{
		lua_close(L);	 
}







































