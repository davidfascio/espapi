/* 
 * File:   main.c
 * Author: fascio
 *
 * Created on 29 de marzo de 2016, 11:12
 */

#include <stdio.h>
#include <stdlib.h>
#include "ESP_API.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    BYTE error_code;
    BYTE buffer[] = {0x55, 0xcc, 0x09, 0x00, 0x1b, 0x09, 0x00, 0x02, 0x0a, 0xb6, 0xda, 0x24, 0x32, 0x70, 0xef, 0x33, 0xcc};
    error_code = ESPComInterface_ReceivedHandler(buffer, sizeof(buffer));
    
    printf("Error Code: %2X \n", error_code);

    return (EXIT_SUCCESS);
}













