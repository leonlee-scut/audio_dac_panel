/**
  * @file bsp.h
  * @brief Board Support Package (BSP) header file.
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

#ifndef __BSP_H__
#define __BSP_H__

#include "led.h"
#include "iwi_SPI_RS0010.h"

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#define BSP_VERSION_MAJOR 0
#define BSP_VERSION_MINOR 1
#define BSP_VERSION_PATCH 0

#define BSP_VERSION  ((BSP_VERSION_MAJOR << 16) + (BSP_VERSION_MINOR << 8) + BSP_VERSION_PATCH)
#define BSP_VERSION_STRING  "0.1.0"

enum {
    DIR9001_FS_32K = 0,
    DIR9001_FS_44K1,
    DIR9001_FS_48K,
    DIR9001_FS_88K2,
    DIR9001_FS_96K,
    DIR9001_FS_176K4,
    DIR9001_FS_192K,
    DIR9001_FS_UNKNOWN = 0xFF
};

enum {
    INPUT_BLE     = 0,
    INPUT_OPTICAL ,
    INPUT_COAXIAL ,
    INPUT_AUX     ,
    INPUT_UNKNOWN = 0xFF
};

extern LED led_blue;
extern SPI_RS0010 oled;

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

void BSP_Init(void);
int BSP_getVersion(void); // return a const char*
const char* BSP_getVersionString(void); // return a const char*
const char* BSP_getBuildDate(void);
const char* BSP_getBuildTime(void);

void BSP_setInputSource(uint8_t input_source);
unsigned int BSP_getInputSource(void);
const char* BSP_getInputSourceString(unsigned int ch);

uint8_t BSP_getEncoderState(void);

int BSP_getMeasFs(void);

void DIR9001_Reset(void);
void DIR9001_Resume(void);

int DIR9001_isPLLLocked(void);
int DIR9001_isMuted(void);
int DIR9001_isDeemphasisEnabled(void);

int DIR9001_getCalcFs(void);

const char* DIR9001_getPLLStatusString(unsigned int pll_status);
const char* DIR9001_getMuteString(unsigned int mute_status);
const char* DIR9001_getDeemphasisString(unsigned int deemph_status);
const char* DIR9001_getFsString(unsigned int fs_code);
const char* DIR9001_getCalcFsString(unsigned int fs_code);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // __BSP_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
