/*
 * display.c
 *
 *  Created on: Oct 13, 2019
 *      Author: Piet
 */

#include "display.h"
#include "stm32f429i_discovery_lcd.h"



void display_init()
{
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, 0xD0000000);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_Clear(LCD_COLOR_DARKGRAY);
    BSP_LCD_SetTransparency(0, 255);
    BSP_LCD_SetLayerVisible(0, !0U);
    BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0, 0, "Hallo, Welt", LEFT_MODE);
    BSP_LCD_DisplayOn();
}




