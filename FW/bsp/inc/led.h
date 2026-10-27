/**
  * @file led.h
  * @brief Discrete LED control interface.
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

#ifndef __LED_H__
#define __LED_H__

#include "gpio.h"

typedef enum {
    ACTIVE_LOW,
    ACTIVE_HIGH
} ACTIVE_LEVEL;


/**
  * @brief A class for controlling discrete LEDs.
  */
class LED
{
  public:
    LED(PinID_t pin,  ACTIVE_LEVEL act_lvl = ACTIVE_HIGH);
    ~LED();

    void on();
    void off();
    void toggle();

  private:
    PinID_t _pin;
    ACTIVE_LEVEL _act_lvl;
}; 
    

#endif // __LED_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
