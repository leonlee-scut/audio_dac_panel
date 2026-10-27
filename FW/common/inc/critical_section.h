/**
  * @file critical_section.h
  * @brief CPU core critical header file
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

#ifndef __CRITICAL_SECTION_H__
#define __CRITICAL_SECTION_H__

#include <stdbool.h>

#include "cmsis_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

#define CRITICAL_SECTION_BEGIN()        {\
                                            volatile uint32_t primask = __get_PRIMASK();\
                                            __disable_irq();\

#define CRITICAL_SECTION_END()              __set_PRIMASK(primask);\
                                        }    

#define IS_HANDLER_MODE()               (__get_IPSR() != 0)
#define IS_THREAD_MODE()                (__get_IPSR() == 0)

/**
  * @brief Determine the current interrupts enabled state
  * 
  * This function can be called to determine whether or not interrupts are currently enabled.
  * @note
  * NOTE:
  * This function works for both cortex-A and cortex-M, although the underlying implementation
  * differs.
  * @return true if interrupts are enabled, false otherwise
  */
__STATIC_INLINE bool are_interrupts_enabled(void)
{
    return (__get_PRIMASK() == 0);
}


/**
  * @brief Determine if this code is executing from an interrupt
  * 
  * This function can be called to determine if the code is running on interrupt context.
  * @note
  * NOTE:
  * This function works for both cortex-A and cortex-M, although the underlying implementation
  * differs.
  * @return true if in an isr, false otherwise
  */
__STATIC_INLINE bool is_isr_active(void)
{
    return (__get_IPSR() != 0);
}


/**
  * @brief Mark the start of a critical section
  * 
  * This function should be called to mark the start of a critical section of code.
  * @note
  * NOTES:
  * 1) The use of this style of critical section is targetted at C based implementations.
  * 2) These critical sections can be nested.
  * 3) The interrupt enable state on entry to the first critical section (of a nested set, or single
  *    section) will be preserved on exit from the section.
  * 4) This implementation will currently only work on code running in privileged mode.
  */
void critical_section_enter(void);


/**
  * @brief Mark the end of a critical section
  *
  * This function should be called to mark the end of a critical section of code.
  * @note
  * NOTES:
  * 1) The use of this style of critical section is targetted at C based implementations.
  * 2) These critical sections can be nested.
  * 3) The interrupt enable state on entry to the first critical section (of a nested set, or single
  *    section) will be preserved on exit from the section.
  * 4) This implementation will currently only work on code running in privileged mode.
  */ 
void critical_section_exit(void);


/**
  * @brief Determine if we are currently in a critical section
  *
  * @return true if in a critical section, false otherwise.
  */
bool in_critical_section(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // __CORE_CRITICAL_H__
/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/

