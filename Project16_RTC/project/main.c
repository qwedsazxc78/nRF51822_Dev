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
** Descriptions:		定时器实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_gpio.h"
#include "bsp.h"
#include "app_error.h"
#include "nrf.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

/* 开发板中LED占用的nRF52832管脚资源
P0.17：输出：驱动指示灯D1
P0.18：输出：驱动指示灯D2
P0.19：输出：驱动指示灯D3
P0.20：输出：驱动指示灯D4


需要短接对应的跳线帽
*/

#define COMPARE_COUNTERTIME  (3UL)                                        /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */
const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;


const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

/** RTC0中断处理函数
 * Triggered on TICK and COMPARE0 match.
 */
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        nrf_gpio_pin_toggle(LED_2);  //3秒后，指示灯D2点亮 
    }
    else if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        nrf_gpio_pin_toggle(LED_1); //指示灯D1以8HZ的频率闪烁
    }
}

/** 低频时钟配置。RTC使用的是LFCLK低频时钟
 */
static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/** @brief Function initialization and configuration of RTC driver instance.
 */
static void rtc_config(void)
{
    uint32_t err_code;

    //初始化RTC实例。初始化完成后，RTC处于power off 状态
    err_code = nrf_drv_rtc_init(&rtc, NULL, rtc_handler);
    APP_ERROR_CHECK(err_code);

    //使能tick事件，使能TICK事件中断
    nrf_drv_rtc_tick_enable(&rtc,true);

    //设置比较通道。比较值，是否使能中断
	  //比较通道：设置为通道0
	  //比较值：设置为3秒
	  //COUNTER计数到设置的比较值后，产生事件中断
    err_code = nrf_drv_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME*RTC0_CONFIG_FREQUENCY,true);
    APP_ERROR_CHECK(err_code);

    //启动RTC
    nrf_drv_rtc_enable(&rtc);
}

/**********************************************************************************************
 * 描  述 : main函数
 * 参  数 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);//配置开发板上驱动LED的管脚为输出
    LEDS_OFF(LEDS_MASK);      //指示灯的初始状态设置为熄灭
    
    lfclk_config();//LFCLK低频时钟配置
    rtc_config();  //RTC配置

    while (true)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}
/********************************************END FILE*******************************************/
