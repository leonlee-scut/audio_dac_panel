/**
  * @file audio_i2s.c
  * @brief 
  * @author Leon Lee (leonlee.scut@outlook.com)
  * @version 0.1
  * @date 2026/10/23
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
  * 	2026/10/23	0.1			Leon Lee		Preliminary version.
  */

#include "audio_i2s.h"
#include "cmsis_os2.h"
#include <string.h>

/* 双缓冲，32-bit 容器放 24-bit 数据 */
static int32_t i2s_buf[2][AUDIO_BUFFER_SAMPLES];
static volatile uint8_t  ready_half = 0;   /* 0 或 1 表示哪个半缓冲就绪 */
static volatile uint8_t  processing = 0;

extern I2S_HandleTypeDef hi2s1;

void Audio_Init(void)
{
    /* CubeMX 已配置：
       - I2S Slave, Philips standard, 32-bit data, 24-bit frame?
       - MCK 外部输入
       - DMA 循环模式，半传输 + 传输完成中断
    */
    memset(i2s_buf, 0, sizeof(i2s_buf));
}


// [v]:: add the parameter to select the buffer size and the sample rate
void Audio_Start(uint32_t sample_rate)
{
    // [v]:: Adjust the buffer size based on the sample rate if needed
    uint32_t buffer_size;
    
    if (sample_rate <= 96000)
    {
        buffer_size = 2 * sample_rate / 100; // 10 ms buffer
    }
    else
    {
        buffer_size = AUDIO_BUFFER_SAMPLES * 2; // Default buffer size
    }

    HAL_I2S_Receive_DMA(&hi2s1, (uint16_t*)i2s_buf[0], buffer_size);
    /* 注意：HAL 对 32-bit 数据时 Size 以 16-bit 为单位，需乘 2 */
}

void Audio_Stop(void)
{
    HAL_I2S_DMAStop(&hi2s1);
    processing = 0;
    ready_half = 0;
}

/* DMA 回调 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == hi2s1.Instance) {
        ready_half = 0;
        processing = 1;
    }
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == hi2s1.Instance) {
        ready_half = 1;
        processing = 1;
    }
}

bool Audio_GetBlock(int32_t **buf, uint32_t *frames)
{
    if (!processing) return false;
    processing = 0;
    *buf = i2s_buf[ready_half];
    *frames = AUDIO_BUFFER_FRAMES;
    return true;
}

/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
