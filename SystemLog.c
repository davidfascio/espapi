//******************************************************************************
//* File: SystemLog.c
//

//******************************************************************************
// Includes
//******************************************************************************
#include "SystemLog.h"

//******************************************************************************
// Variables
//******************************************************************************
BYTE systemLogTrace = SYSTEM_LOG_PRINT_ALL;

//******************************************************************************
// SYSTEM LOG Function
//******************************************************************************
BYTE printTerminal (BYTE* Ptr, boolean hasPrompt){

    BYTE buffer[500];
    
    memset(buffer, 0, 500);
    
    if (hasPrompt)
        memcpy(buffer, SHELL_CONTEXT_PROMPT, SHELL_CONTEXT_PROMPT_LEN);

    if (Ptr == NULL)
        return 0;
    
    strcat(buffer, Ptr);
    printf(buffer);
    return 0;
}

void SystemLog_Printf(BOOL hasPrompt, BYTE tagTrace, const char * format, ...){

    char buffer[SYSTEM_LOG_PRINT_MAX_BUFFER_SIZE];
    va_list argptr;

    if(!(tagTrace & systemLogTrace))
        return;
    
    va_start(argptr, format);
    vsprintf(buffer, format, argptr);
    va_end(argptr);

    printTerminal(buffer, hasPrompt);
}

void print_buffer(BYTE * buffer, WORD bufferLen){

    WORD index;
    
    for (index = 0; index < bufferLen; index++)        
        print("%02X ",*buffer++);
}
