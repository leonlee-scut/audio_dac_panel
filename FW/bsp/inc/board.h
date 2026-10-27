/**
  * @file board.h
  * @brief Board-specific definitions and configurations.
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

#ifndef __BOARD_H__
#define __BOARD_H__

#include "blackpill.h"

/* OLED10016 display pins */
#define OLED_CS             PB12
#define OLED_SCK            PB13
#define OLED_MISO           PB14
#define OLED_MOSI           PB15

/* USART1 pins */
#define USART1_TX           PA9
#define USART1_RX           PA10

/* I2S pins */
#define I2S_WS              PA4
#define I2S_CK              PA5
#define I2S_SD              PA7
#define I2S_MCK             PA2

/* DIR9001 interface pins */
#define DIR_FSEL0           PB5         //!< input, Sampling frequency calculator indicator pin
#define DIR_FSEL1           PB6         //!< input, Sampling frequency calculator indicator pin
#define DIR_DEEMPH          PB2         //!< input, When low, two audio channels without pre-emphasis, when high, two audio channels with 50us/15us pre-emphasis
#define DIR_MUTE0           PB3         //!< input, When high, the audio output is muted.
#define DIR_MUTE1           PB4         //!< input, When low, audio sample word represents linear PCM samples, when high, audio sample word is used for other purposes.
#define DIR_MCLK            I2S_MCK
#define DIR_DATA            I2S_SD
#define DIR_LRCK            I2S_WS
#define DIR_BCLK            I2S_CK
#define DIR_RST             PB0         //!< output with open-drain, active low
#define DIR_PLL_OK          PB8         //!< input, high when PLL is locked

/*
 * Calculated Sampling Frequency Output
 *  
 *     NOMINAL fs    ACTUAL SAMPLING  CALCULATED SAMPLING FREQUENCY OUTPUT
 *                   FREQUENCY RANGE     FSOUT1      FSOUT0
 *     Out of range  Out of range or     (DIR_FSEL1) (DIR_FSEL0)
 *                   PLL unlocked          H           L
 *     32 kHz        31.2 kHz–32.8 kHz     H           H
 *     44.1 kHz      43 kHz–45.2 kHz       L           L
 *     48 kHz        46.8 kHz–49.2 kHz     L           H
 */

/* SIGNAL input select pins */
#define SEL1                PB7         //!< input, active high
#define SEL2                PB9         //!< input, active high
#define SEL3                PB1         //!< input, active high
#define SEL4                PB10        //!< input, active high

/* SIGNAL aliases */
#define SIGNAL_BLE          SEL1
#define SIGNAL_OPT          SEL2
#define SIGNAL_COAX         SEL3
#define SIGNAL_USB          SEL4

/* SAMPLE frequency measurement pins */
#define FS_MEAS_IN          PA8         //!< input, use to measure the actual sampling frequency of 
                                        //   the audio signal, connect to DIR9001's LRCKO pin, i.e. I2S_WS pin

/* ENCODER pins */
#define EC_A                PA1         //!< input, Encoder A pin
#define EC_B                PA0         //!< input, Encoder B pin
#define EC_SW               PA3         //!< input, Encoder switch pin
#endif // __BOARD_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
