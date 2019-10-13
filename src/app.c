/*
 * app.c
 *
 *  Created on: Oct 13, 2019
 *      Author: Piet
 */

#include "app.h"
#include "display.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"



void app_run()
{

    display_init();


    while (1) {
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_Delay(500);
    }
}


