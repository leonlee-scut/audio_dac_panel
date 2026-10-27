/**
  * @file delay.h
  * @brief  Delay functions for STM32Fxxx series microcontroller.
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

#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // __DELAY_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
