/**
  * @file assert.h
  * @brief Application assert header file
  * @author Leon Lee (leonlee.scut@gmail.com)
  * @version 0.1
  * @date 2024/07/02
  * 
  * @copyright Copyright (c) 2024 South China University of Technology.
  * All rights reserved.
  * 
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * 
  * @par History:
  * 	Date		Version		Author			Description
  * 	2024/07/02	0.1			Leon Lee		Preliminary version.
  */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

void assert_handler(const char *expr, const char *file, int line);

#ifdef __cplusplus
}
#endif /* extern "C" */

#if defined(NDEBUG) || defined(APP_NO_ASSERT)
#define assert(expr) ((void)0)
#else
#define assert(expr) ((expr) ? (void)0 : assert_handler(#expr, __FILE__, __LINE__))
#endif

#define ASSERT      assert

#endif // __APP_ASSERT_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
