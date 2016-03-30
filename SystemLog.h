#ifndef __SYSTEM_LOG_H__
#define __SYSTEM_LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include "GenericTypeDefs.h"
//! #include "Shell.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define SYSTEM_LOG_PRINT_MAX_BUFFER_SIZE                                (300)

#define SYSTEM_LOG_NO_COLOR                                         "\e[0m"
#define SYSTEM_LOG_RED_COLOR                          "\e[1;31m"
#define SYSTEM_LOG_GREEN_COLOR                        "\e[1;32m"
#define SYSTEM_LOG_YELLOW_COLOR                       "\e[1;33m"
#define SYSTEM_LOG_BLUE_COLOR                         "\e[1;34m"
#define SYSTEM_LOG_PURPLE_COLOR                       "\e[1;35m"
#define SYSTEM_LOG_CYAN_COLOR                         "\e[1;36m"
#define SYSTEM_LOG_WHITE_COLOR                        "\e[1;37m"


void SystemLog_Printf(BOOL hasPrompt, const char * format, ...);
BYTE printTerminal (BYTE* Ptr, boolean hasPrompt);

#define SystemLog_Println(M, ...) SystemLog_Printf(TRUE, M, ##__VA_ARGS__)

//! #define NDEBUG

#ifdef NDEBUG
#define println_message(M, ...)
#define print_message(M, ...)
#define print_debug(M, ...)
#define print_log(M, ...)
#define print_error(M, ...)
#define print_warm(M, ...)
#define print_info(M, ...)
#else

#define println_message(M, ...) SystemLog_Println(SYSTEM_LOG_BLUE_COLOR "[MSG] " SYSTEM_LOG_NO_COLOR "(%s) " M,__FILENAME__, ##__VA_ARGS__)
#define print_message(M,...) SystemLog_Printf(FALSE, M, ##__VA_ARGS__)
#define print_debug(M, ...) SystemLog_Println(SYSTEM_LOG_PURPLE_COLOR "[DEBUG] " SYSTEM_LOG_NO_COLOR "(%s:%d: function: %s) " M, __FILENAME__ ,__LINE__, __func__, ##__VA_ARGS__)
#define print_log(M, ...) SystemLog_Println(SYSTEM_LOG_CYAN_COLOR "[LOG] " SYSTEM_LOG_NO_COLOR "(%s) " M, __FILENAME__, ##__VA_ARGS__)
#define print_error(M, ...) SystemLog_Println(SYSTEM_LOG_RED_COLOR "[ERROR] " "(%s:%d: function: %s) " M SYSTEM_LOG_NO_COLOR, __FILENAME__ ,__LINE__, __func__, ##__VA_ARGS__)
#define print_warm(M, ...) SystemLog_Println(SYSTEM_LOG_YELLOW_COLOR "[WARM] "SYSTEM_LOG_NO_COLOR "(%s:%d: function: %s) " M, __FILENAME__ ,__LINE__, __func__, ##__VA_ARGS__)
//#define print_warm(M, ...) SystemLog_Println(SYSTEM_LOG_YELLOW_COLOR "[WARM] "SYSTEM_LOG_NO_COLOR M, ##__VA_ARGS__)
#define print_info(M, ...) SystemLog_Println(SYSTEM_LOG_GREEN_COLOR "[INFO] " SYSTEM_LOG_NO_COLOR "(%s) " M, __FILENAME__,  ##__VA_ARGS__)

#define println(M,...) SystemLog_Println(SYSTEM_LOG_PRINT, M,##__VA_ARGS__)
#define print(M,...) SystemLog_Printf(FALSE, SYSTEM_LOG_PRINT, M,##__VA_ARGS__)
#endif

void print_buffer(BYTE * buffer, WORD bufferLen);

#endif /* __SYSTEM_LOG_H__ */
