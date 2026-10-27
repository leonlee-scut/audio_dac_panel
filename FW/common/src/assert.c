/**
  * @file assert.c
  * @brief Application assert handler 
  * @author Leon Lee (leonlee.scut@gmail.com)
  * @version 0.1
  * @date 2024/07/04
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
  * 	2024/07/04	0.1			Leon Lee		Preliminary version.
  */

#include "cmsis_compiler.h"
#include "assert.h"

__WEAK void assert_handler(const char *expr, const char *file, int line)
{
    
    while(1);
}



/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
