/*
 * Display.h
 *
 *  Created on: Oct 12, 2019
 *      Author: Piet
 */

#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include "stm32f4xx_hal.h"

class Display {
public:
	Display(LTDC_HandleTypeDef *hltdc);
	virtual ~Display();
private:
	LTDC_HandleTypeDef *handle;
};

#endif /* DISPLAY_HPP_ */
