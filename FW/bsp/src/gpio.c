/**
  * @file gpio.c
  * @brief GPIO API implementation file for GPIO operations.
  * @author Leon Lee (leonlee.scut@outlook.com)
  * @version 0.1
  * @date 2026/09/30
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
  * 	2026/09/30	0.1			Leon Lee		Preliminary version.
  */

/**
  * @note All functions in this file are declared as __WEAK, allowing users to 
  *       override them with their own implementations if needed.
  *       This provides flexibility for users to customize GPIO operations 
  *       according to their specific requirements.
  */

#include "gpio.h"

/**
  * @brief Write a GPIO pin by PinID
  * 
  * @param pin PinID
  * @param val Value (0 or 1)
  */
__WEAK void pinWrite(PinID_t pin, PIN_STATE val)
{
    GPIO_TypeDef *port = GPIOx(pin);
    uint32_t pinBit = 1UL << GPIO_PIN_NUMBER(pin);
    port->BSRR = pinBit << (val != PIN_RESET ? 0 : 16);
}


/**
  * @brief Read a GPIO pin by PinID
  * 
  * @param pin PinID
  * @return pin value (0 or 1) 
  */
__WEAK uint32_t pinRead(PinID_t pin)
{
    GPIO_TypeDef *port = GPIOx(pin);
    uint32_t pinBit = 1UL << GPIO_PIN_NUMBER(pin);
    
    return port->IDR & pinBit ? PIN_SET : PIN_RESET;
}


/**
  * @brief Toggle a GPIO pin by PinID
  * 
  * @param pin PinID
  */
__WEAK void pinToggle(PinID_t pin)
{
    GPIOx(pin)->ODR ^= 1UL << GPIO_PIN_NUMBER(pin);
}


/**
  * @brief Set the mode of a GPIO pin by PinID
  * 
  * @param pin PinID
  * @param mode Pin mode
  * @return PIN_CONFIG_SUCCESS if successful, otherwise PIN_CONFIG_FAILED
  *
  * @note This function uses the HAL_GPIO_Init function to configure the GPIO pin.
  *       User can override this function to provide a custom implementation if needed.
  */
__WEAK int pinMode(PinID_t pin, PIN_MODE mode)
{
#if defined(USE_HAL_DRIVER)
    GPIO_TypeDef* port = GPIOx(pin);
    GPIO_InitTypeDef gpio_init = { 0 };

    if (port == NULL)
    {
        return PIN_CONFIG_FAILED;
    }

    /* Enable GPIO clock for the selected port before calling HAL_GPIO_Init. */
    if (port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
#if defined(GPIOB)
    else if (port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
#endif
#if defined(GPIOC)
    else if (port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
#endif
#if defined(GPIOD)
    else if (port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
#endif
#if defined(GPIOE)
    else if (port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
#endif
#if defined(GPIOF)
    else if (port == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif
#if defined(GPIOG)
    else if (port == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif
#if defined(GPIOH)
    else if (port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif
#if defined(GPIOI)
    else if (port == GPIOI)
    {
        __HAL_RCC_GPIOI_CLK_ENABLE();
    }
#endif

    gpio_init.Pin = 1UL << GPIO_PIN_NUMBER(pin);

    switch (mode)
    {
        case INPUT: /* INPUT_FLOATING is an alias of INPUT */
            gpio_init.Mode = GPIO_MODE_INPUT;
            gpio_init.Pull = GPIO_NOPULL;
            break;

        case INPUT_PULLUP:
            gpio_init.Mode = GPIO_MODE_INPUT;
            gpio_init.Pull = GPIO_PULLUP;
            break;

        case INPUT_PULLDOWN:
            gpio_init.Mode = GPIO_MODE_INPUT;
            gpio_init.Pull = GPIO_PULLDOWN;
            break;

        case OUTPUT: /* OUTPUT_PUSHPULL is an alias of OUTPUT */
            gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
            gpio_init.Pull = GPIO_NOPULL;
            gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;

        case OUTPUT_OPENDRAIN:
            gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
            gpio_init.Pull = GPIO_NOPULL;
            gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;

        default:
            return PIN_CONFIG_FAILED;
    }

    HAL_GPIO_Init(port, &gpio_init);

    return PIN_CONFIG_SUCCESS;
#endif
    return PIN_CONFIG_FAILED;
}


/**
  * @brief Delete a GPIO pin by PinID
  * 
  * @param pin PinID
  */
__WEAK void pinDelete(PinID_t pin)
{
#if defined(USE_HAL_DRIVER)
    GPIO_TypeDef* port = GPIOx(pin);
    
    /* De-initialize the GPIO pin using HAL_GPIO_DeInit. */
    HAL_GPIO_DeInit(port, (1UL << GPIO_PIN_NUMBER(pin)));
#endif
}

/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/

