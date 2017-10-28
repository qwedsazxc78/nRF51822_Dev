/****************************************Copyright (c)****************************************************
**                                        
**                                      [艾克姆科技]
**                                        IIKMSIK
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   使用的SDK版本-SDK_11.0.0
**						
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		看门狗实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_drv_wdt.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

/* 开发板中指示灯和按键占用的nRF52832管脚资源
P0.17：输出：驱动指示灯D1
P0.18：输出：驱动指示灯D2
P0.19：输出：驱动指示灯D3
P0.20：输出：驱动指示灯D4

P0.13：输入：按键检测S1
P0.14：输入：按键检测S2
P0.15：输入：按键检测S3
P0.16：输入：按键检测S4

需要短接对应的跳线帽
*/
#define APP_TIMER_PRESCALER     0                           /**< RTC1分频系数 */
#define APP_TIMER_OP_QUEUE_SIZE 2                           /**< Size of timer operation queues. */
#define FEED_BUTTON_ID          0                           /**< Button for feeding the dog. */

nrf_drv_wdt_channel_id m_channel_id;

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    LEDS_OFF(LEDS_MASK);
    
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}

/**
 * @brief Assert callback.
 *
 * @param[in] id    Fault identifier. See @ref NRF_FAULT_IDS.
 * @param[in] pc    The program counter of the instruction that triggered the fault, or 0 if
 *                  unavailable.
 * @param[in] info  Optional additional information regarding the fault. Refer to each fault
 *                  identifier for details.
 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    LEDS_OFF(LEDS_MASK);
    while(1);
}

/**
 * @brief BSP events callback.
 */
void bsp_event_callback(bsp_event_t event)
{
    switch(event)
    {
        case BSP_EVENT_KEY_0:
            nrf_drv_wdt_channel_feed(m_channel_id);
            break;
        
        default :
            //Do nothing.
            break;
    }
}

/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
    uint32_t err_code = NRF_SUCCESS;
    
    //BSP configuration for button support: button pushing will feed the dog.
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_BUTTONS, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), bsp_event_callback);
    APP_ERROR_CHECK(err_code);
    
    //配置开发板上的4个用户LED指示灯
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    
    //4个指示灯轮流闪烁一次，指示系统启动
    for(uint32_t i = 0; i < LEDS_NUMBER; i++)
    {   nrf_delay_ms(200);
        LEDS_ON(BSP_LED_0_MASK << i);
    }
     err_code = bsp_buttons_enable();
     APP_ERROR_CHECK(err_code);
    
    //配置WDT.
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;//使用默认参数配置看门狗。即CPU睡眠时，看门狗保持运行；看门狗超时周期2秒
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);//检查返回值
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();

    while(1)
    {
        __SEV();  //设置事件
        __WFE();  //进入睡眠，等待事件唤醒
        __WFE();
    }
}
/********************************************END FILE*******************************************/
