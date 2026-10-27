/**
  * @file stm32f411_pinid.h
  * @brief Pin ID definitions for STM32F411 series microcontroller.
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

#ifndef __STM32F411_PINID_H__
#define __STM32F411_PINID_H__

#if !defined(STM32F411xE)
  #error "This header file is only for STM32F411xE series microcontrollers."
#endif // !defined(STM32F411xE)

#if !defined(STM32F411CCUx) && \
    !defined(STM32F411CEUx) && \
    !defined(STM32F411RCTx) && \
    !defined(STM32F411RETx) && \
    !defined(STM32F411VCTx) && \
    !defined(STM32F411VETx)
  #error "Please select the target STM32F411 device used in your application (in stm32f411_pinid.h file)."
#endif // !defined(STM32F411CCUx)

typedef enum {
    /* Pins bonded out on all STM32F411 package sizes. */
    PA0        = 0x00,
    PA1        = 0x01,
    PA2        = 0x02,
    PA3        = 0x03,
    PA4        = 0x04,
    PA5        = 0x05,
    PA6        = 0x06,
    PA7        = 0x07,
    PA8        = 0x08,
    PA9        = 0x09,
    PA10       = 0x0A,
    PA11       = 0x0B,
    PA12       = 0x0C,
    PA13       = 0x0D,
    PA14       = 0x0E,
    PA15       = 0x0F,

    PB0        = 0x10,
    PB1        = 0x11,
    PB2        = 0x12,
    PB3        = 0x13,
    PB4        = 0x14,
    PB5        = 0x15,
    PB6        = 0x16,
    PB7        = 0x17,
    PB8        = 0x18,
    PB9        = 0x19,
    PB10       = 0x1A,

#if defined(STM32F411RCTx) || defined(STM32F411RETx) || \
    defined(STM32F411VCTx) || defined(STM32F411VETx)
    /* Additional pins bonded out on 64-pin and larger packages. */
    PB11       = 0x1B,
#endif

    PB12       = 0x1C,
    PB13       = 0x1D,
    PB14       = 0x1E,
    PB15       = 0x1F,

#if defined(STM32F411RCTx) || defined(STM32F411RETx) || \
    defined(STM32F411VCTx) || defined(STM32F411VETx)
    /* Additional pins bonded out on 64-pin and larger packages. */
    PC0        = 0x20,
    PC1        = 0x21,
    PC2        = 0x22,
    PC3        = 0x23,
    PC4        = 0x24,
    PC5        = 0x25,
    PC6        = 0x26,
    PC7        = 0x27,
    PC8        = 0x28,
    PC9        = 0x29,
    PC10       = 0x2A,
    PC11       = 0x2B,
    PC12       = 0x2C,
#endif

    PC13       = 0x2D,
    PC14       = 0x2E,
    PC15       = 0x2F,

#if defined(STM32F411VCTx) || defined(STM32F411VETx)
    /* Additional pins bonded out only on 100-pin packages. */
    PD0        = 0x30,
    PD1        = 0x31,
    PD2        = 0x32,

    PE0        = 0x40,
    PE1        = 0x41,
    PE2        = 0x42,
    PE3        = 0x43,
    PE4        = 0x44,
    PE5        = 0x45,
    PE6        = 0x46,
    PE7        = 0x47,
    PE8        = 0x48,
    PE9        = 0x49,
    PE10       = 0x4A,
    PE11       = 0x4B,
    PE12       = 0x4C,
    PE13       = 0x4D,
    PE14       = 0x4E,
    PE15       = 0x4F,
#endif

    PH0        = 0x70,
    PH1        = 0x71,

    /**** ADC internal channels ****/

    // ADC_TEMP = 0xF0, // Internal pin virtual value
    // ADC_VREF = 0xF1, // Internal pin virtual value

    // STDIO for console print

    /**** USB pins ****/
    USB_DM = PA11,
    USB_DP = PA12,

    /**** OSCILLATOR pins ****/
    RCC_OSC32_IN = PC14,
    RCC_OSC32_OUT = PC15,
    RCC_OSC_IN = PH0,
    RCC_OSC_OUT = PH1,

    /**** DEBUG pins ****/
    SYS_JTCK_SWCLK = PA14,
    SYS_JTDI = PA15,
    SYS_JTDO_TRACESWO = PB3,
    SYS_JTMS_SWDIO = PA13,
    SYS_NJTRST = PB4,
    SYS_WKUP = PA0,

    // Not connected
    // NC = (int)0xFFFFFFFF
} PinID;

#endif // __STM32F411_PINID_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
