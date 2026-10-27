/**
  * @file bsp.c
  * @brief Board Support Package (BSP) source file.
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

#include <stdint.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "bsp.h"
#include "board.h"
#include "gpio.h"

#define CAPTURE_FLAG_WAIT_FIRST 0
#define CAPTURE_FLAG_WAIT_SECOND 1
#define CAPTURE_FLAG_DONE 2

#define FS_MEAS_TIME_OUT           SystemCoreClock / (1000ul * 6)   // About 1ms timeout for frequency measurement

/*
 * Global objects for the board peripherals
 */
LED led_blue(LED_BLUE, ACTIVE_LOW);
SPI_RS0010 oled;


/*
 * Private variables for frequency measurement using TIM1
 */
TIM_HandleTypeDef htim1;

// 频率测量相关全局变量
volatile uint16_t _capture_val1 = 0;
volatile uint16_t _capture_val2 = 0;
volatile uint8_t  _capture_flag = 0;  // 0: 等待第一次捕获, 1: 等待第二次捕获, 2: 捕获完成
volatile uint32_t _measured_freq_hz = 0;


const char* FS_MSG[] = {
    "32K",
    "44.1K",
    "48K",
    "88.2K",
    "96K",
    "176.4K",
    "192K",
    "352.8K",
    "384K",
    "UNKNOWN"
};

const char* DEEMPH_MSG[] = {
    "Deemph OFF",
    "Deemph ON"
};

const char* MUTE_MSG[] = {
    "Mute OFF",
    "Mute ON"
};

const char* PLL_MSG[] = {
    "PLL UNLOCKED",
    "PLL LOCKED"
};

const char* INPUT_MSG[] = {
    "  BLE  ",
    "OPTICAL",
    "COAXIAL",
    "  USB  ",
    "UNKNOWN"
};

static void TIM1_PA8_FreqMeasure_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
    TIM_MasterConfigTypeDef sMasterConfig = { 0 };
    TIM_IC_InitTypeDef sConfigIC = { 0 };

    // 1. 使能 GPIOA 与 TIM1 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();

    // 2. 配置 PA8 为 TIM1_CH1 复用模式
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. 配置 TIM1 基础定时器参数 (96MHz 时钟直接计数)
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;                     // 不分频，计数频率 96MHz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0xFFFF;                   // 16位最大装载值 65535
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        // Error_Handler();
    }

    // 4. 配置 TIM1 输入捕获通道 CH1
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;// 下降沿捕获
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;       // 映射到 TI1
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;                 // 每次下降沿都捕获
    sConfigIC.ICFilter = 2;                                 // 数字滤波，滤除高频毛刺噪声
    if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        // Error_Handler();
    }

    // 5. 配置 NVIC 中断
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

    // 6. 启动输入捕获中断
    // HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
}


static void TIM1_PA8_FreqMeasure_Start(void)
{
    _capture_val1 = 0;
    _capture_val2 = 0;
    _measured_freq_hz = 0;
    _capture_flag = CAPTURE_FLAG_WAIT_FIRST; // Reset capture state

    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC1); // Clear any pending capture flags
    __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1); // Clear any pending interrupt flags
    __HAL_TIM_SET_COUNTER(&htim1, 0); // Reset the timer counter
    HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
}


void BSP_Init(void)
{
    // Initialize GPIO pins for the board
    pinMode(DIR_FSEL0, INPUT_PULLUP);
    pinMode(DIR_FSEL1, INPUT_PULLUP);
    pinMode(DIR_DEEMPH, INPUT_PULLDOWN);
    pinMode(DIR_MUTE0, INPUT_PULLUP);
    pinMode(DIR_MUTE1, INPUT_PULLUP);
    pinMode(DIR_RST, OUTPUT_OPENDRAIN);
    pinMode(DIR_PLL_OK, INPUT_PULLDOWN);

    // Set the initial state of the reset pin to low (active low)
    pinWrite(DIR_RST, LOW);

    BSP_setInputSource(INPUT_UNKNOWN); // Set default input source to BLE

    pinMode(SEL1, OUTPUT);
    pinMode(SEL2, OUTPUT);
    pinMode(SEL3, OUTPUT);
    pinMode(SEL4, OUTPUT);


    pinMode(EC_A, INPUT);
    pinMode(EC_B, INPUT);
    pinMode(EC_SW, INPUT);

    led_blue.off(); // Ensure the blue LED is off at startup

    oled.initSPI(OLED_MISO, OLED_MOSI, OLED_SCK, OLED_CS);
    oled.begin(16, 2, MODE_GRAPHIC); // Initialize the OLED display with 100x16 resolution and default mode
    oled.displayBuf(); // Update the display with the cleared buffer

    // Initialize TIM1 for frequency measurement on PA8
    TIM1_PA8_FreqMeasure_Init();
}

int BSP_getVersion(void)
{
    return BSP_VERSION;
}

const char* BSP_getVersionString(void)
{
    return BSP_VERSION_STRING;
}

const char* BSP_getBuildDate(void)
{
    return __DATE__;
}

const char* BSP_getBuildTime(void)
{
    return __TIME__;
}

void BSP_setInputSource(uint8_t input_source)
{
    switch (input_source) 
    {
        case INPUT_BLE:
            pinWrite(SEL1, HIGH);
            pinWrite(SEL2, LOW);
            pinWrite(SEL3, LOW);
            pinWrite(SEL4, LOW);
            break;

        case INPUT_OPTICAL:
            pinWrite(SEL1, LOW);
            pinWrite(SEL2, HIGH);
            pinWrite(SEL3, LOW);
            pinWrite(SEL4, LOW);
            break;

        case INPUT_COAXIAL:
            pinWrite(SEL1, LOW);
            pinWrite(SEL2, LOW);
            pinWrite(SEL3, HIGH);
            pinWrite(SEL4, LOW);
            break;

        case INPUT_AUX:
            pinWrite(SEL1, LOW);
            pinWrite(SEL2, LOW);
            pinWrite(SEL3, LOW);
            pinWrite(SEL4, HIGH);
            break;

        default:
            // Invalid input source; do nothing or handle error
            pinWrite(SEL1, LOW);
            pinWrite(SEL2, LOW);
            pinWrite(SEL3, LOW);
            pinWrite(SEL4, LOW);
            break;
    }
}

unsigned int BSP_getInputSource(void)
{
    uint8_t inputSource = 0;

    inputSource |= (pinRead(SEL1) << 0);
    inputSource |= (pinRead(SEL2) << 1);
    inputSource |= (pinRead(SEL3) << 2);
    inputSource |= (pinRead(SEL4) << 3);

    switch (inputSource) {
        case 0b0001:
            return INPUT_BLE;
        case 0b0010:
            return INPUT_OPTICAL;
        case 0b0100:
            return INPUT_COAXIAL;
        case 0b1000:
            return INPUT_AUX;
        default:
            return INPUT_UNKNOWN;
    }
}

const char* BSP_getInputSourceString(unsigned int ch)
{
    if (ch <= INPUT_AUX)
    {
        return INPUT_MSG[ch];
    }
    else
    {
        return INPUT_MSG[INPUT_AUX + 1]; // Return "Input: UNKNOWN" for invalid input source
    }   
}

uint8_t BSP_getEncoderState(void)
{
    static uint8_t ec_a = 0, ec_b = 0;
    static uint8_t last_ec_a = 0, last_ec_b = 0;

    ec_a <<= 1;
    ec_b <<= 1;

    // Read the state of the encoder pins
    ec_a |= pinRead(EC_A);
    ec_b |= pinRead(EC_B);

    ec_a &= 0x03;
    ec_b &= 0x03;

    // Debounce filtering: if the last two readings are the same, consider it stable
    last_ec_a = (ec_a == 0x03) ? 1 : (ec_a == 0) ? 0 : last_ec_a;
    last_ec_b = (ec_b == 0x03) ? 1 : (ec_b == 0) ? 0 : last_ec_b;

    return (last_ec_a << 1) | (last_ec_b
    );
}

int BSP_getMeasFs(void)
{
    uint32_t time_out = FS_MEAS_TIME_OUT; // Set a timeout value to avoid infinite blocking

    // Start the frequency measurement process
    TIM1_PA8_FreqMeasure_Start();

    // Wait for the measurement to complete
    while (_capture_flag != CAPTURE_FLAG_DONE)
    {
        // Optionally, add a timeout mechanism here to avoid infinite blocking
        if (time_out-- == 0)
        {
            HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_1);
            _capture_flag = CAPTURE_FLAG_WAIT_FIRST;
            return DIR9001_FS_UNKNOWN; // Return unknown frequency on timeout
        }
    };

    // Unsigned 16-bit subtraction handles one timer counter wraparound.
    const uint16_t diff = (_capture_val2 >= _capture_val1) ? (_capture_val2 - _capture_val1)
                                                           : ( UINT16_MAX - _capture_val1 + _capture_val2 + 1);

    if (diff == 0U)
    {
        return DIR9001_FS_UNKNOWN;
    }

    // TIM1 is clocked from APB2; its prescaler is zero and APB2 is undivided.
    const uint32_t timer_clock_hz = HAL_RCC_GetPCLK2Freq();
    _measured_freq_hz = (timer_clock_hz + (diff / 2U)) / diff;

    if (_measured_freq_hz >= 30400U && _measured_freq_hz <= 33600U)
    {
        return DIR9001_FS_32K;
    }
    if (_measured_freq_hz >= 41895U && _measured_freq_hz <= 46049U)
    {
        return DIR9001_FS_44K1;
    }
    if (_measured_freq_hz >= 46050U && _measured_freq_hz <= 50400U)
    {
        return DIR9001_FS_48K;
    }
    if (_measured_freq_hz >= 83790U && _measured_freq_hz <= 92099U)
    {
        return DIR9001_FS_88K2;
    }
    if (_measured_freq_hz >= 92100U && _measured_freq_hz <= 100800U)
    {
        return DIR9001_FS_96K;
    }
    if (_measured_freq_hz >= 167580U && _measured_freq_hz <= 184199U)
    {
        return DIR9001_FS_176K4;
    }
    if (_measured_freq_hz >= 184200U && _measured_freq_hz <= 201600U)
    {
        return DIR9001_FS_192K;
    }

    return DIR9001_FS_UNKNOWN;
}


void DIR9001_Reset(void)
{
    pinWrite(DIR_RST, LOW);
}

void DIR9001_Resume(void)
{
    pinWrite(DIR_RST, HIGH);
}

int DIR9001_isPLLLocked(void)
{
    return pinRead(DIR_MUTE0) == HIGH ? FALSE : TRUE;
}

int DIR9001_isMuted(void)
{
    if (pinRead(DIR_MUTE0) == HIGH || pinRead(DIR_MUTE1) == HIGH) 
    {
        return TRUE;
    }

    return FALSE;
}

int DIR9001_isDeemphasisEnabled(void)
{
    return pinRead(DIR_DEEMPH) == HIGH ? TRUE : FALSE;
}

int DIR9001_getSamplingFrequency(void)
{
    return 0;
}

int DIR9001_getCalcFs(void)
{
    int fs_code = (pinRead(DIR_FSEL1) << 1) | pinRead(DIR_FSEL0);

    switch (fs_code)
    {
        case 0b00:
            return DIR9001_FS_44K1;         // 44.1 kHz
        case 0b01:
            return DIR9001_FS_48K;          // 48 kHz
        case 0b10:
            // return DIR9001_FS_UNKNOWN;      // Unknown frequency 
            return BSP_getMeasFs();         // Use measured frequency if the code is invalid
        case 0b11:
            return DIR9001_FS_32K;          // 32 kHz
        default:
            return DIR9001_FS_UNKNOWN;      // Unknown frequency
    }
}

const char* DIR9001_getPLLStatusString(unsigned int pll_status)
{
    return pll_status > FALSE ? PLL_MSG[1] : PLL_MSG[0];
}

const char* DIR9001_getMuteString(unsigned int mute_status)
{
    return mute_status > FALSE ? MUTE_MSG[1] : MUTE_MSG[0];
}

const char* DIR9001_getDeemphasisString(unsigned int deemph_status)
{
    return deemph_status > FALSE ? DEEMPH_MSG[1] : DEEMPH_MSG[0];
}

const char* DIR9001_getFsString(unsigned int fs_code)
{
    if (fs_code <= DIR9001_FS_192K)
    {
        return FS_MSG[fs_code];
    }
    else
    {
        return FS_MSG[9]; // Return "FS: UNKNOWN" for invalid fs_code
    }
}

const char* DIR9001_getCalcFsString(unsigned int fs_code)
{
    if (fs_code <= DIR9001_FS_96K)
    {
        return FS_MSG[fs_code];
    }
    else
    {
        return FS_MSG[8]; // Return "FS: UNKNOWN" for invalid fs_code
    }
}


// TIM1 捕获中断入口
extern "C" void TIM1_CC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

// HAL 库输入捕获回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        switch (_capture_flag)
        {
            case CAPTURE_FLAG_WAIT_FIRST:
                // Save the first capture value
                _capture_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
                _capture_flag = CAPTURE_FLAG_WAIT_SECOND; // Mark to wait for the second capture
                break;

            case CAPTURE_FLAG_WAIT_SECOND:
                // Save the second capture value
                _capture_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
                _capture_flag = CAPTURE_FLAG_DONE; // Mark capture done

                // Stop the timer to prevent further captures until processed
                HAL_TIM_IC_Stop_IT(htim, TIM_CHANNEL_1);
                break;

            default:
                // Capture already done or in an unexpected state, ignore further captures
                break;
        }
    }
}

/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
