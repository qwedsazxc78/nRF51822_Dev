/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                main.c
** Last modified Date:
** Last Version:
** Descriptions:           使用的SDK版本-SDK_11.0.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:          FiYu
** Created date:        2016-10-2
** Version:             1.0
** Descriptions:        GPIOTE实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"

/* 开发板中指示灯占用的nRF52832管脚资源
P0.17：输出：驱动指示灯D1
P0.18：输出：驱动指示灯D2
P0.19：输出：驱动指示灯D3
P0.20：输出：驱动指示灯D4


需要短接对应的跳线帽
*/

#define GPIO_OUTPUT_PIN_NUMBER BSP_LED_0  // 定义指示灯驱动管脚

static nrf_drv_timer_t timer = NRF_DRV_TIMER_INSTANCE(0);

void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context) {}


static void led_blinking_setup()
{
    uint32_t compare_evt_addr;
    uint32_t gpiote_task_addr;
    nrf_ppi_channel_t ppi_channel;
    ret_code_t err_code;
    nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

    //初始化GPIOTE输出管脚
    err_code = nrf_drv_gpiote_out_init(GPIO_OUTPUT_PIN_NUMBER, &config);
    APP_ERROR_CHECK(err_code);


    nrf_drv_timer_extended_compare(&timer, (nrf_timer_cc_channel_t)0, 200 * 1000UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    // 配置PPI通道TIMER0 CC[0]匹配事件作为事件端点触发GPIOTE的Task(任务端点)
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);//分配PPI通道，成功分配通道后，ppi_channel指向被分配的通道
    APP_ERROR_CHECK(err_code);

    compare_evt_addr = nrf_drv_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(GPIO_OUTPUT_PIN_NUMBER);
    // 设置通道的事件端点和任务端点
    err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);
    APP_ERROR_CHECK(err_code);

    // 使能已经配置了事件端点和任务端点的PPI通道
    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_task_enable(GPIO_OUTPUT_PIN_NUMBER);
}


/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/
int main(void)
{
    ret_code_t err_code;

    //初始化PPI。检查PPI模块是否已经被初始化，若未初始化，将初始化标志置位
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    //初始化GPIOTE模块。
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    //初始化定时器，注意这里用不到事件处理函数，所以定义的timer_dummy_handler是个空函数
    err_code = nrf_drv_timer_init(&timer, NULL, timer_dummy_handler);
    APP_ERROR_CHECK(err_code);


    // 设置PPI通道，定时器匹配事件作为事件端点EEP，GPIOTE 的TASK(驱动GPIO翻转)作为任务端点TEP.
    led_blinking_setup();

    // 使能定时器
    nrf_drv_timer_enable(&timer);

    while (true)
    {

    }
}
/********************************************END FILE*******************************************/
