/**
  * @file critical_section.c
  * @brief CPU core critical routines
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

/* Declare __STDC_LIMIT_MACROS so stdint.h defines UINT32_MAX when using C++ */
#define __STDC_LIMIT_MACROS

// #include "cmsis.h"

#include "assert.h"
#include "critical_section.h"

static uint32_t critical_section_reentrancy_counter = 0;
static uint32_t primask_backup = 0;

void critical_section_enter(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (critical_section_reentrancy_counter > 0) 
    {
        return;
    }

    primask_backup = primask;

    // If the reentrancy counter overflows something has gone badly wrong.
    ASSERT(critical_section_reentrancy_counter < UINT32_MAX);
    
    ++critical_section_reentrancy_counter;
}

void critical_section_exit(void)
{
    // If critical_section_enter has not previously been called, do nothing
    if (critical_section_reentrancy_counter == 0) 
    {
        return;
    }

    --critical_section_reentrancy_counter;

    if (critical_section_reentrancy_counter == 0)
    {
        // Interrupts must be disabled on invoking an exit from a critical section
        ASSERT(!are_interrupts_enabled());

        // Restore the IRQs to their state prior to entering the critical section
        __set_PRIMASK(primask_backup);
    }
}

bool in_critical_section(void)
{
    return critical_section_reentrancy_counter > 0;
}


/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
