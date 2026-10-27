/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
*        SEGGER RTT * Real Time Transfer for embedded targets        *
*                  https://github.com/SEGGERMicro/RTT                *
*                                                                    *
**********************************************************************

---------------------------END-OF-HEADER------------------------------
Purpose : User configuration file for RTT.
          For available configuration,
          refer to SEGGER_RTT_ConfDefaults.h.

----------------------------------------------------------------------
*/

#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H


/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

/*********************************************************************
*
*      SEGGER_RTT_MAX_NUM_UP_BUFFERS 
*
*  Description
*    Maximum number of RTT up-buffers (Target -> Host).
*
*  Additional information
*    Common use case:
*      Up-buffer channel 0: RTT Terminal I/O.
*      Up-buffer channel 1: SystemView.
*/
#define SEGGER_RTT_MAX_NUM_UP_BUFFERS           (2)

/*********************************************************************
*
*      SEGGER_RTT_MAX_NUM_DOWN_BUFFERS 
*
*  Description
*    Maximum number of RTT down-buffers (Host -> Target).
*
*  Additional information
*    Common use case:
*      Down-buffer channel 0: RTT Terminal I/O.
*      Down-buffer channel 1: SystemView.
*
*    The number of up- and down-buffers may differ.
*/
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS         (2)

/*********************************************************************
*
*      BUFFER_SIZE_UP
*
*  Description
*    Size of (auto-installed) up-buffer channel 0.
*
*  Additional information
*    Channel 0 is commonly used for Terminal I/O.
*    Buffer should be large enough for all terminal (to host) output
*    messages.
*/
#define BUFFER_SIZE_UP                          (1024)

/*********************************************************************
*
*      BUFFER_SIZE_DOWN
*
*  Description
*    Size of (auto-installed) down-buffer channel 0.
*
*  Additional information
*    Channel 0 is commonly used for Terminal I/O.
*    Buffer should be large enough to receive all terminal
*    (from host) input.
*/
#define BUFFER_SIZE_DOWN                        (256)

/*********************************************************************
*
*     SEGGER_RTT_MODE_DEFAULT 
*
*  Description
*    RTT Mode for channel 0.
*/
#define SEGGER_RTT_MODE_DEFAULT                 SEGGER_RTT_MODE_NO_BLOCK_SKIP

/*********************************************************************
*
*     SEGGER_RTT_PRINTF_BUFFER_SIZE 
*
*  Description
*    Size of temporary buffer for RTT printf bulk-send.
*/
#define SEGGER_RTT_PRINTF_BUFFER_SIZE           (256u)

#endif /* SEGGER_RTT_CONF_H */
/*************************** End of file ****************************/
