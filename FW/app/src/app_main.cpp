/**
  * @file app_main.cpp
  * @brief 
  * @author Leon Lee (leonlee.scut@outlook.com)
  * @version 0.1
  * @date 2026/10/12
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
  * 	2026/10/12	0.1			Leon Lee		Preliminary version.
  */

#include <string>

#include "cmsis_os2.h"
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"

#include "bsp.h"
#include "delay.h"
#include "audio_i2s.h"
#include "vu_meter.h"
#include "app_main.h"
#include "splash_bitmap.h"

using namespace std;

#define LINE1       line_str[0]
#define LINE2       line_str[1]

#define TICKS_PER_SECOND            1000
#define APP_MAIN_THREAD_PERIOD      20      // 20ms period for the main application thread

#define PLL_LOCKED_DSP_TIMEOUT      (TICKS_PER_SECOND * 5) // 5 seconds timeout for PLL locked status display
#define MUSIC_STOPPED_TIMEOUT       (TICKS_PER_SECOND * 5) // 5 seconds timeout for music stopped state

#define VU_METER_REFRESH_RATE       50      // Refresh rate in Hz for the VU meter 
#define VU_METER_REFRESH_PERIOD     (TICKS_PER_SECOND / VU_METER_REFRESH_RATE) // Refresh period in ticks

typedef enum {
    STATE_PLL_UNLOCKED = 0,
    STATE_PLL_LOCKED,
    STATE_MUSIC_PLAYING,
    STATE_MUSIC_PAUSED,
    STATE_MUSIC_STOPPED
}FSM_State_t;

typedef enum {
    SUBSTATE_ENTRY = 0,
    SUBSTATE_ON_GOING,
    SUBSTATE_EXIT
}SubState_t;

uint8_t sysview_up_buf[SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
uint8_t sysview_down_buf[BUFFER_SIZE_DOWN];

string line_str[2] = {"TDA1543 NOS DAC", ""};

FSM_State_t fsm_state = STATE_PLL_UNLOCKED;

static SubState_t _sub_state = SUBSTATE_ENTRY;
static uint32_t _timeout_counter = 0;

VuMeter _vu_meter;
float _current_fs = 0.0f;
uint8_t _audio_start_enabled = 0;

const uint32_t sample_rates[] = { 32000, 44100, 48000, 88200, 96000, 176400, 192000 };

static void __splash(void)
{
    oled.clearBuf();
    oled.drawBitmap_P(0, 0, BOOT_SPLASH_BITMAP, RS0010_WIDTH, RS0010_HEIGHT, WHITE);
    oled.displayBuf();
}

static void __screen_update(void)
{
    oled.clearBuf();
    oled.drawStringAligned(0, 0, RS0010_WIDTH, 8, line_str[0].c_str(), WHITE, BLACK, ALIGN_CENTER);
    oled.drawStringAligned(0, 8, RS0010_WIDTH, 16, line_str[1].c_str(), WHITE, BLACK, ALIGN_CENTER);
    oled.displayBuf();
}

static void __vu_screen_construct(void)
{
    line_str[0] = string(BSP_getInputSourceString(BSP_getInputSource()));

    uint8_t fs_code = DIR9001_getCalcFs();
    if (fs_code <= DIR9001_FS_96K)
    {
        line_str[1] = string(DIR9001_getCalcFsString(fs_code));
        _current_fs = (float)sample_rates[fs_code];
    }
    else
    {
        line_str[1] = "----";
        _current_fs = 96000.0f;
    }

    oled.clearBuf();
    oled.drawStringAligned(0, 0, RS0010_WIDTH / 2, 8, 
                           line_str[0].c_str(), 
                           WHITE, BLACK, ALIGN_CENTER);
    oled.drawStringAligned(RS0010_WIDTH / 2, 0, RS0010_WIDTH - 5, 8, 
                           line_str[1].c_str(), 
                           WHITE, BLACK, ALIGN_CENTER);

    // Draw the VU meter
    oled.fillRect(0, 9, 1, 2, WHITE);
    oled.fillRect(0, 12, 1, 2, WHITE);

    // Draw the scale marks for the VU meter
    /* -40 dB = col0, -30=25, -20=50, -10=75, 0=100 */
    const uint8_t marks[] = {0, 24, 49, 74, 99};
    for (int i = 0; i < 5; i++) {
        uint8_t x = marks[i];
        oled.drawFastVLine(x, 15, 1, WHITE);
    }

    oled.updateScreen(UPDATE_ALL);
}

static void __vu_screen_update(void)
{
    // TODO: Update the VU meter based on the audio signal level
    uint8_t left, right, pk_l, pk_r;
    uint8_t width = 2; // Width of the VU meter peak indicators

    VU_GetDisplay(&_vu_meter, &left, &right, &pk_l, &pk_r);

    // Clear the VU meter area
    oled.fillRect(0, 9, RS0010_WIDTH, 5, BLACK); 

    // Draw the left channel VU meter and peak indicator
    oled.fillRect(0, 9, left, 2, WHITE);
    width = (pk_l == 0) ? 1 : 2; // Adjust width based on peak position
    pk_l = (pk_l < RS0010_WIDTH) ? pk_l : (RS0010_WIDTH - 1); // Ensure peak position is within bounds
    oled.fillRect(pk_l, 9, width, 2, WHITE);

    // Draw the right channel VU meter and peak indicator
    oled.fillRect(0, 12, right, 2, WHITE);
    width = (pk_r == 0 || pk_r == 100) ? 1 : 2; // Adjust width based on peak position
    pk_r = (pk_r < RS0010_WIDTH) ? pk_r : (RS0010_WIDTH - 1); // Ensure peak position is within bounds
    oled.fillRect(pk_r, 12, width, 2, WHITE);

    oled.updateScreen(UPDATE_BOTTOM_HALF);
}

static void __state_pll_unlocked_process(void)
{
    switch (_sub_state)
    {
        case SUBSTATE_ENTRY:
            SEGGER_RTT_WriteString(0, "FSM: STATE_PLL_UNLOCKED\n");
            _sub_state = SUBSTATE_ON_GOING;
            break;

        case SUBSTATE_ON_GOING:
            // Do nothing.
            break;

        case SUBSTATE_EXIT:
            // Handle exit state if needed
            break;

        default:
            break;
    }

    _audio_start_enabled = 0; // Disable audio processing when PLL is unlocked

    if (DIR9001_isPLLLocked())
    {
        SEGGER_RTT_WriteString(0, "FSM: STATE_PLL_UNLOCKED -> STATE_PLL_LOCKED\n");
        fsm_state = STATE_PLL_LOCKED;
        _sub_state = SUBSTATE_ENTRY;
    }

    // line_str[0] = string(DIR9001_getPLLStatusString(FALSE));
    line_str[0] = "- STOP -";
    line_str[1] = "Input: " + string(BSP_getInputSourceString(BSP_getInputSource()));
    __screen_update();
    SEGGER_RTT_WriteString(0, "Screen updated.\n");
}

static void __state_pll_locked_process(void)
{
    switch (_sub_state)
    {
        case SUBSTATE_ENTRY:
            SEGGER_RTT_WriteString(0, "FSM: STATE_PLL_LOCKED\n");
            _timeout_counter = osKernelGetTickCount() + PLL_LOCKED_DSP_TIMEOUT;
            // line_str[0] = string(DIR9001_getPLLStatusString(TRUE));
            line_str[0] = "- PLAY -";
            line_str[1] = "Input: " + string(BSP_getInputSourceString(BSP_getInputSource()));
            __screen_update();
            SEGGER_RTT_WriteString(0, "Screen updated.\n");
            _sub_state = SUBSTATE_ON_GOING;
            break;

        case SUBSTATE_ON_GOING:
            // Delay for 5 seconds to display the PLL locked status
            // While time out, transition to the music playing state
            if (osKernelGetTickCount() >= _timeout_counter)
            {
                SEGGER_RTT_WriteString(0, "FSM: STATE_PLL_LOCKED -> STATE_MUSIC_PLAYING\n");
                fsm_state = STATE_MUSIC_PLAYING;
                _sub_state = SUBSTATE_ENTRY;
                break;
            }

            if (!DIR9001_isPLLLocked())
            {
                SEGGER_RTT_WriteString(0, "FSM: STATE_PLL_LOCKED -> STATE_PLL_UNLOCKED\n");
                fsm_state = STATE_PLL_UNLOCKED;
                _sub_state = SUBSTATE_ENTRY;
            }
            break;

        case SUBSTATE_EXIT:
            // Handle exit state if needed
            break;

        default:
            break;
    }
}

static void __state_music_playing_process(void)
{
    switch (_sub_state)
    {
        case SUBSTATE_ENTRY:
            SEGGER_RTT_WriteString(0, "FSM: STATE_MUSIC_PLAYING\n");
            __vu_screen_construct();
            _audio_start_enabled = 1; // Enable audio processing
            _sub_state = SUBSTATE_ON_GOING;
            break;

        case SUBSTATE_ON_GOING:
            // Update the screen with the current input source and sample frequency
            __vu_screen_update();
            SEGGER_RTT_WriteString(0, "VU meter updated.\n");

            if (!DIR9001_isPLLLocked())
            {
                SEGGER_RTT_WriteString(0, "FSM: STATE_MUSIC_PLAYING -> STATE_MUSIC_STOPPED\n");
                fsm_state = STATE_MUSIC_STOPPED;
                _sub_state = SUBSTATE_ENTRY;
            }
            
            break;

        case SUBSTATE_EXIT:
            // Handle exit state if needed
            break;

        default:
            break;
    }
}

static void __state_music_stopped_process(void)
{
    switch (_sub_state)
    {
        case SUBSTATE_ENTRY:
            SEGGER_RTT_WriteString(0, "FSM: STATE_MUSIC_STOPPED\n");
            // DO NOT change the screen content, just wait for the next state transition
            _timeout_counter = osKernelGetTickCount() + MUSIC_STOPPED_TIMEOUT;
            _sub_state = SUBSTATE_ON_GOING;
            break;

        case SUBSTATE_ON_GOING:
            // TODO:: While music is stopped, continue to refresh the VU meter display area 
            //        until the timeout is reached.
            __vu_screen_update();
            SEGGER_RTT_WriteString(0, "VU meter updated.\n");

            if (osKernelGetTickCount() >= _timeout_counter)
            {
                SEGGER_RTT_WriteString(0, "FSM: STATE_MUSIC_STOPPED -> STATE_PLL_UNLOCKED\n");
                fsm_state = STATE_PLL_UNLOCKED;
                _sub_state = SUBSTATE_ENTRY;
            }

            break;

        case SUBSTATE_EXIT:
            // Handle exit state if needed
            break;

        default:
            break;
    }
}
void audioTask(void *argument)
{

}

void appMainThread(void *argument)
{
    uint32_t ticks = 0;

    // Initialize the Board Support Package (BSP)
    BSP_Init();

    __splash();
    delay_ms(5000); // Wait for 5 seconds before starting the main loop

    line_str[1] = "Ver: " + string(BSP_getVersionString());

    // Show a welcome message on the OLED display
    __screen_update();

    delay_ms(5000); // Wait for 5 seconds before starting the main loop

    DIR9001_Resume(); // Resume the DIR9001 device after the splash screen

    // Main application loop
    while (1) 
    {
        // Get the current tick count
        ticks = osKernelGetTickCount();

        // Update the input source based on the encoder state
        BSP_setInputSource(BSP_getEncoderState()); 

        /*
         * 1. Check DIR9001 Sample frequency, and display it on the first line of the OLED screen.
         * 2. Check DIR9001 Single input channel, and display it on the second line of the OLED screen.
         */

        switch (fsm_state)
        {
            case STATE_PLL_UNLOCKED:
                __state_pll_unlocked_process();
                break;

            case STATE_PLL_LOCKED:
                __state_pll_locked_process();
                break;

            case STATE_MUSIC_PLAYING:
                __state_music_playing_process();
                break;

            case STATE_MUSIC_PAUSED:
            case STATE_MUSIC_STOPPED:
                __state_music_stopped_process();
                break;

            default:
                break;
        }

        // Delay for the VU meter refresh period
        osDelayUntil(ticks + VU_METER_REFRESH_PERIOD);

    }
}

extern "C" void app_main(void) 
{
    // Initialize SEGGER RTT and SYSVIEW for debugging and profiling
    SEGGER_RTT_Init();

    SEGGER_RTT_ConfigUpBuffer(SEGGER_SYSVIEW_RTT_CHANNEL, 
                              "SYSVIEW", 
                              sysview_up_buf, 
                              sizeof(sysview_up_buf), 
                              SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_RTT_ConfigDownBuffer(SEGGER_SYSVIEW_RTT_CHANNEL, 
                                "SYSVIEW", 
                                sysview_down_buf, 
                                sizeof(sysview_down_buf), 
                                SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_SYSVIEW_Conf();

    SEGGER_RTT_WriteString(0, "SEGGER RTT and SYSVIEW initialized!\n");

    // Create the main application thread
    const osThreadAttr_t appMainThreadAttr = {
        .name = "APP Main",
        .stack_size = 1024,
        .priority = osPriorityNormal
    };

    osKernelInitialize();
    osThreadNew(appMainThread, NULL, &appMainThreadAttr);
    
    SEGGER_RTT_WriteString(0, "RTOS kernel started!\n");
    SEGGER_RTT_WriteString(0, "Application main thread started!\n");

    osKernelStart();
}


/************* (C) COPYRIGHT South China Univ. of Tech. ****** END OF FILE ****/
