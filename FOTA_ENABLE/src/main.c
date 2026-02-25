#include <zephyr/kernel.h> 
#include "fota_ble_ashu.h"

#define delayTime 100 

void main(void) 
{
    fota_ashu_start();
    while (1)                                                    
    {
        k_msleep(delayTime);                                              
    }
}
