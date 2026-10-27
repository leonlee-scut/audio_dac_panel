/**
  * @file audio_i2s.h
  * @brief Audio I2S interface
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

#ifndef __AUDIO_I2S_H__
#define __AUDIO_I2S_H__

#include "RTE_Components.h"
#include CMSIS_device_header
#include <stdint.h>
#include <stdbool.h>

// [v]:: add the parameter to select the buffer size and the sample rate
#define AUDIO_BUFFER_FRAMES   960   /* 约 10 ms @96 kHz */
#define AUDIO_BUFFER_SAMPLES  (AUDIO_BUFFER_FRAMES * 2)

extern I2S_HandleTypeDef hi2s1;     /* 请在 CubeMX 中命名为 hi2s1 或修改 */

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

void Audio_Init(void);
void Audio_Start(uint32_t sample_rate);  /* 启动 I2S 接收，sample_rate 用于 VU Meter */
void Audio_Stop(void);
bool Audio_GetBlock(int32_t **buf, uint32_t *frames);  /* 非阻塞获取最新块 */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // __AUDIO_I2S_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
