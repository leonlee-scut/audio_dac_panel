/**
  * @file gpio.h
  * @brief GPIO API header file for GPIO operations.
  * @author Leon Lee (leonlee.scut@outlook.com)
  * @version 0.1
  * @date 2026/09/29
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
  * 	2026/09/29	0.1			Leon Lee		Preliminary version.
  */

#ifndef __GPIO_API_H__
#define __GPIO_API_H__

#include "RTE_Components.h"
#include CMSIS_device_header

/**
  * @brief Type definition for GPIO pin ID.
  *
  * @details
  *   The PinID_t type is used to identify a GPIO pin. It is defined as an 8-bit 
  *   unsigned integer, where the upper 4 bits represent the GPIO port number 
  *   (0 for Port A, 1 for Port B, etc.), and the lower 4 bits represent the pin 
  *   number within that port (0-15). This allows for a compact representation 
  *   of GPIO pins across multiple ports.
  * 
  */
typedef uint8_t PinID_t;

#define GPIO_PIN_MASK               0x0F
#define GPIO_PORT_MASK              0xF0
/**
  * @brief Type definition for GPIO pin state.
  * 
  */
typedef enum {
    PIN_RESET = 0,
    LOW = PIN_RESET,
    PIN_SET = 1,
    HIGH = PIN_SET
} PIN_STATE;

/**
  * @brief GPIO mode.
  * 
  */
typedef enum {
    INPUT,                          ///< Input mode, no pull-up or pull-down resistor, default input mode
    INPUT_FLOATING = INPUT,
    INPUT_PULLUP,                   ///< Input mode with pull-up resistor
    INPUT_PULLDOWN,                 ///< Input mode with pull-down resistor
    OUTPUT,                         ///< Output mode, with push-pull output, default output mode
    OUTPUT_PUSHPULL = OUTPUT,
    OUTPUT_OPENDRAIN                ///< Output mode, with open-drain output
} PIN_MODE;


#define PIN_CONFIG_SUCCESS          0
#define PIN_CONFIG_FAILED           -1

#define pinReset(pin_id)            pinWrite(pin_id, PIN_RESET)
#define pinSet(pin_id)              pinWrite(pin_id, PIN_SET)

#define pinRESET(pin_id)            pinWrite(pin_id, PIN_RESET)
#define pinSET(pin_id)              pinWrite(pin_id, PIN_SET)

#define pinLow(pin_id)              pinWrite(pin_id, PIN_RESET)
#define pinHigh(pin_id)             pinWrite(pin_id, PIN_SET)

#define pinLOW(pin_id)              pinWrite(pin_id, PIN_RESET)
#define pinHIGH(pin_id)             pinWrite(pin_id, PIN_SET)

#define GPIO_PORT_NUMBER(pin_id)    ((pin_id & GPIO_PORT_MASK) >> 4)
#define GPIO_PORT_REGS_SIZE         (0x0400)
#define GPIO_PORT_REGS_OFFSET(port_num)  ((port_num) * GPIO_PORT_REGS_SIZE)

#define GPIO_PIN_NUMBER(pin_id)     (pin_id & GPIO_PIN_MASK)

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/**
  * @brief Get GPIO port from PinID
  * 
  * @param pin Pin id of a GPIO pin 
  * @return GPIO_TypeDef* GPIO Port
  */
__STATIC_INLINE GPIO_TypeDef *GPIOx(PinID_t pin)
{
    uint32_t portNmu = GPIO_PORT_NUMBER(pin);

    return (GPIO_TypeDef *)(GPIOA_BASE + GPIO_PORT_REGS_OFFSET(portNmu));
}

/**
  * @brief Get pin number from PinID
  * 
  * @param pin Pin id of a GPIO pin 
  * @return int 
  */
__STATIC_INLINE uint8_t pinNum(PinID_t pin)
{
    return GPIO_PIN_NUMBER(pin);
}


void pinWrite(PinID_t pin, PIN_STATE val);
uint32_t pinRead(PinID_t pin);
void pinToggle(PinID_t pin);
int pinMode(PinID_t pin, PIN_MODE mode);
void pinDelete(PinID_t pin);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // __GPIO_API_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
