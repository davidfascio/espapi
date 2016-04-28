/* 
 * File:   SystemQueue.h
 * Author: fascio
 *
 * Created on 29 de diciembre de 2015, 15:51
 */

#ifndef __SYSTEM_QUEUE_H__
#define	__SYSTEM_QUEUE_H__

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#define SYSTEM_QUEUE_MAX_INFO_SIZE                                      (100)

typedef struct _queue_element{
    
    char info[SYSTEM_QUEUE_MAX_INFO_SIZE];
    int infoSize;
    struct  _queue_element * next;
    
}QUEUE_ELEMENT, * QUEUE_ELEMENT_PTR;

typedef struct {
    
    QUEUE_ELEMENT_PTR head;
    QUEUE_ELEMENT_PTR tail;
    
} QUEUE_LIST, * QUEUE_LIST_PTR;

QUEUE_LIST_PTR QueueList_AddElement(QUEUE_LIST_PTR queueList, char * info, int infoSize);
QUEUE_LIST_PTR QueueList_RemoveElement(QUEUE_LIST_PTR queueList);

QUEUE_LIST_PTR QueueList_New(void);
QUEUE_LIST_PTR QueueList_Free(QUEUE_LIST_PTR queueList);

/*void QueueList_Print(QUEUE_LIST_PTR queueList);
void QueueList_PrintElement(QUEUE_ELEMENT_PTR queueElement);
void QueueList_PrintInfo( char * info, int infoSize);*/

#endif	/* __SYSTEM_QUEUE_H__ */

