/**
  * @file delay.c
  * @brief Delay functions implementation.
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

#include "RTE_Components.h"
#include CMSIS_device_header

#if defined(RTE_CMSIS_RTOS2)
#include "cmsis_os2.h"
#endif

#include "delay.h"

#ifndef DELAY_LOOP_CYCLES_PER_ITERATION
#define DELAY_LOOP_CYCLES_PER_ITERATION 6U
#endif

#if defined(__DWT_PRESENT) && (__DWT_PRESENT == 1U)
#define DELAY_HAS_DWT_CYCCNT 1
#elif defined(__CORTEX_M) && \
      ((__CORTEX_M == 3U) || (__CORTEX_M == 4U) || (__CORTEX_M == 7U) || \
       (__CORTEX_M == 33U) || (__CORTEX_M == 55U) || (__CORTEX_M == 85U))
#define DELAY_HAS_DWT_CYCCNT 1
#endif


// Private functions
/**
  * @brief Delay for a specified number of microseconds.
  * @param us Number of microseconds to delay.
  */
static void delay_us_loop(uint32_t us)
{
    uint32_t loops_per_us = SystemCoreClock /
        (1000000U * DELAY_LOOP_CYCLES_PER_ITERATION);

    if (loops_per_us == 0U) {
        loops_per_us = 1U;
    }

    for (uint32_t elapsed_us = 0U; elapsed_us < us; ++elapsed_us) {
        for (volatile uint32_t loop = 0U; loop < loops_per_us; ++loop) {
            __NOP();
        }
    }
}


/**
  * @brief Delay for a specified number of milliseconds.
  * @param ms Number of milliseconds to delay.
  * @note This function can be re-implemented by the user to provide a more 
  *       accurate delay mechanism if needed.
  */
__WEAK void delay_ms(uint32_t ms)
{
    if (ms == 0U)
    {
        return;
    }

#if defined(RTE_CMSIS_RTOS2)
    if (osKernelGetState() == osKernelRunning)
    {
        osDelay(ms);
        return;
    }
#endif /* defined(RTE_CMSIS_RTOS2) */

#if defined(USE_HAL_DRIVER)

    HAL_Delay(ms);

#else /* defined(USE_HAL_DRIVER) */
    delay_us_loop(ms * 1000U);
#endif /* defined(USE_HAL_DRIVER) */

}


/**
  * @brief Delay for a specified number of microseconds.
  * @param us Number of microseconds to delay.
  * @note This function can be re-implemented by the user to provide a more 
  *       accurate delay mechanism if needed.
  */
__WEAK void delay_us(uint32_t us)
{
    if (us == 0U)
    {
        return;
    }

#if defined(DELAY_HAS_DWT_CYCCNT) && \
    defined(DWT_CTRL_NOCYCCNT_Msk) && defined(DWT_CTRL_CYCCNTENA_Msk) && \
    defined(CoreDebug_DEMCR_TRCENA_Msk)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    if ((DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) == 0U && SystemCoreClock != 0U)
    {
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

        if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0U)
        {
            uint64_t remaining_cycles =
                (((uint64_t)SystemCoreClock * us) + 999999U) / 1000000U;

            while (remaining_cycles > 0U)
            {
                uint32_t cycle_chunk = (remaining_cycles > UINT32_MAX)
                    ? UINT32_MAX
                    : (uint32_t)remaining_cycles;
                uint32_t start_cycle = DWT->CYCCNT;

                while ((uint32_t)(DWT->CYCCNT - start_cycle) < cycle_chunk) 
                {
                }

                remaining_cycles -= cycle_chunk;
            }
            return;
        }
    }
#endif

    delay_us_loop(us);
}


/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
