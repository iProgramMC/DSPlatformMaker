/*---------------------------------------------------------------------------------
							GAME PROJECT
						  [ July 29, 2020 ]
					 Programmed by iProgramInCpp
					 
							Utils module
---------------------------------------------------------------------------------*/
#include "Utils.hpp"

void LogMsg(const char* s, ...) {
	
	char c[10240];
	va_list v;
	va_start(v, s);
	memset(c,0,sizeof(c));
	vsprintf(c, s, v);
	va_end(v);
	puts(c);
	
}