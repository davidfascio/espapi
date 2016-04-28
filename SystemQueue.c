#include "SystemQueue.h"
#include "SystemLog.h"

QUEUE_LIST_PTR QueueList_AddElement(QUEUE_LIST_PTR queueList, char * info, int infoSize){

    QUEUE_ELEMENT_PTR queueElement_ptr = NULL;
    
    if(queueList == NULL)
        return queueList;
    
    if(infoSize > SYSTEM_QUEUE_MAX_INFO_SIZE){
        print_error("SYSTEM_QUEUE_MAX_INFO_SIZE");
        return queueList;
    }
    
    queueElement_ptr = malloc(sizeof(* queueElement_ptr));
    
    if(queueElement_ptr == NULL){

        print_error("Queue could not allocate pending task");
        return queueList;
    }
    
    memset(queueElement_ptr->info, 0 , SYSTEM_QUEUE_MAX_INFO_SIZE);
    memcpy(queueElement_ptr->info, info, infoSize);
    
    queueElement_ptr->infoSize = infoSize;    
    queueElement_ptr->next = NULL;
    
    if((queueList->head == NULL) && (queueList->tail == NULL)){
        
        queueList->head = queueList->tail = queueElement_ptr;
        return queueList;
    }
    
    if((queueList->head == NULL) || (queueList->tail == NULL)){
        free(queueElement_ptr);
        return queueList;
    }
    
    queueList->tail->next = queueElement_ptr;
    queueList->tail =  queueElement_ptr;

    
    return queueList;
}

QUEUE_LIST_PTR QueueList_RemoveElement(QUEUE_LIST_PTR queueList){
    
    QUEUE_ELEMENT_PTR head = NULL;
    QUEUE_ELEMENT_PTR queueNextElement_ptr = NULL;
    
    if(queueList == NULL)
        return queueList;
    
    if((queueList->head == NULL) && (queueList->tail == NULL))
        return queueList;
    
    if((queueList->head == NULL) || (queueList->tail == NULL))
        return queueList;
    
    head = queueList->head;
    queueNextElement_ptr = head->next;
    
    free(head);
    
    queueList->head = queueNextElement_ptr;
    
    if(queueList->head == NULL)
        queueList->tail = queueList->head;
    
    return queueList;    
}

QUEUE_LIST_PTR QueueList_New(void){
    
    QUEUE_LIST_PTR queueList_ptr = NULL;
    
    queueList_ptr = malloc(sizeof(* queueList_ptr));
    
    if(queueList_ptr == NULL)
        return NULL;
    
    queueList_ptr->head = queueList_ptr->tail = NULL;
    
    return queueList_ptr;    
}

QUEUE_LIST_PTR QueueList_Free(QUEUE_LIST_PTR queueList){
    
    while(queueList->head != NULL){
        
        QueueList_RemoveElement(queueList);
    }   
    
    return queueList;            
}

/*void QueueList_Print(QUEUE_LIST_PTR queueList){

    QUEUE_ELEMENT_PTR queueElement_ptr = NULL;
    
    if(queueList != NULL){
        
        for(    queueElement_ptr = queueList->head ; 
                queueElement_ptr != NULL ; 
                queueElement_ptr = queueElement_ptr->next){
            
            QueueList_PrintElement(queueElement_ptr);
        }
    }
}*/

/*void QueueList_PrintElement(QUEUE_ELEMENT_PTR queueElement){
    
    if(queueElement != NULL)
        QueueList_PrintInfo(queueElement->info,queueElement->infoSize);
    
    else
        printTerminal("Could not  print NULL Queue Element", TRUE);
        
}*/

/*void QueueList_PrintInfo( char * info, int infoSize){
    
    int index = 0;
    
    for(index = 0; index < infoSize; info ++, index++)
        printf("%c", * info);
    
    printf("\n");
}*/
