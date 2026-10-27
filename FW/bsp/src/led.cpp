/**
  * @file led.cpp
  * @brief LED class implementation.
  * @author Leon Lee (leonlee.scut@outlook.com)
  * @version 0.1
  * @date 2026/10/09
  * 
  * @copyright Copyright (c) 2026 South China University of Technology.
  * All rights reserved.
  * 
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * 
  * @par History:
  * 	Date		Version		Author			Description
  * 	2026/10/09	0.1			Leon Lee		Preliminary version.
  */

#include "led.h"


/**
  * @brief Constructor for the LED class.
  * @param pin The pin ID for the LED.
  * @param act_lvl The active level of the LED.
  */
LED::LED(PinID_t pin, ACTIVE_LEVEL act_lvl) : _pin(pin), _act_lvl(act_lvl)
{
    // Set the pin mode to output
    pinMode(_pin, OUTPUT);
}


/**
  * @brief Destroy the LED::LED object
  * 
  */
LED::~LED()
{
    pinDelete(_pin);
}


/**
  * @brief Turn the LED on.
  * 
  */
void LED::on()
{
    pinWrite(_pin, (_act_lvl == ACTIVE_HIGH) ? HIGH : LOW);
}


/**
  * @brief Turn the LED off.
  * 
  */
void LED::off()
{
    pinWrite(_pin, (_act_lvl == ACTIVE_HIGH) ? LOW : HIGH);
}


/**
  * @brief Toggle the state of the LED.
  * 
  */
void LED::toggle()
{
    pinToggle(_pin);
}

/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
