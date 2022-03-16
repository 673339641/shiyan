#ifndef _LUA_OPERATION_H
#define _LUA_OPERATION_H
#include "sys.h"



u8   do_file_script(void);
u8   LuaRun(char * Script);
void LuaClose(char * Script);

#endif



