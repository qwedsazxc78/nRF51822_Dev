/****************************************Copyright (c)****************************************************
**
**                                      [艾克姆科技]
**                                        IIKMSIK
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
** Descriptions:        ppi实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

/* 开发板中指示灯和串口占用的nRF52832管脚资源
P0.17：输出：驱动指示灯D1
P0.18：输出：驱动指示灯D2
P0.19：输出：驱动指示灯D3
P0.20：输出：驱动指示灯D4

P0.06：UART_TXD   ：串口发送
P0.08：UART_RXD   ：串口接收
P0.07：UART_CTS   : 未使用流控
P0.05：UART_RTS   : 未使用流控

需要短接对应的跳线帽
*/

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                           /**< UART RX buffer size. */

const nrf_drv_timer_t timer0 = NRF_DRV_TIMER_INSTANCE(0);
const nrf_drv_timer_t timer1 = NRF_DRV_TIMER_INSTANCE(1);
const nrf_drv_timer_t timer2 = NRF_DRV_TIMER_INSTANCE(2);

nrf_ppi_channel_t ppi_channel1, ppi_channel2;

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}
// Timer even handler. Not used since timer is used only for PPI.
void timer_event_handler(nrf_timer_event_t event_type, void * p_context) {}

/** @brief Function for initializing the PPI peripheral.
*/
static void ppi_init(void)
{
    uint32_t err_code = NRF_SUCCESS;

    err_code = nrf_drv_ppi_init();//初始化PPI。检查PPI模块是否已经被初始化，若未初始化，将初始化标志置位
    APP_ERROR_CHECK(err_code);

    // 配置第一个PPI通道(本实验中使用了2个PPI通道)TIMER1 COMPARE[0]匹配事件作为事件端点触发TIMER0计数器的STOP Task(任务端点)
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel1);//分配PPI通道，成功分配通道后，ppi_channel1指向被分配的通道
    APP_ERROR_CHECK(err_code);
    // 设置通道的事件端点和任务端点
    err_code = nrf_drv_ppi_channel_assign(ppi_channel1,
                                          nrf_drv_timer_event_address_get(&timer1, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_timer_task_address_get(&timer0, NRF_TIMER_TASK_STOP));
    APP_ERROR_CHECK(err_code);

    // 配置第二个PPI通道(本实验中使用了2个PPI通道)TIMER2 COMPARE[0]匹配事件作为事件端点触发TIMER0计数器的START Task(任务端点)
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel2);//分配PPI通道，成功分配通道后，ppi_channel2指向被分配的通道
    APP_ERROR_CHECK(err_code);
    // 设置通道的事件端点和任务端点
    err_code = nrf_drv_ppi_channel_assign(ppi_channel2,
                                          nrf_drv_timer_event_address_get(&timer2, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_timer_task_address_get(&timer0, NRF_TIMER_TASK_START));
    APP_ERROR_CHECK(err_code);

    // 使能两个已经配置了事件端点和任务端点的PPI通道
    err_code = nrf_drv_ppi_channel_enable(ppi_channel1);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_enable(ppi_channel2);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for Timer 0 initialization, which will be started and stopped by timer1 and timer2 using PPI.
*/
static void timer0_init(void)
{
    ret_code_t err_code = nrf_drv_timer_init(&timer0, NULL, timer_event_handler);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for Timer 1 initialization.
 *  @details Initializes Timer 1 peripheral, creates event and interrupt every 2 seconds,
 *           by configuring CC[0] to timer overflow value, we create events at even number of seconds
 *           for example, events are created at 2,4,6 ... seconds. This event can be used to stop Timer 0
 *           with Timer1->Event_Compare[0] triggering Timer 0 TASK_STOP through PPI.
*/
static void timer1_init(void)
{
    // Configure Timer 1 to overflow every 2 seconds. Check TIMER1 configuration for details
    // The overflow occurs every 0xFFFF/(SysClk/2^PRESCALER).
    // = 65535/31250 = 2.097 sec
    ret_code_t err_code = nrf_drv_timer_init(&timer1, NULL, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(&timer1, NRF_TIMER_CC_CHANNEL0, 0xFFFFUL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
}


/** @brief Function for Timer 2 initialization.
 *  @details Initializes Timer 2 peripheral, creates event and interrupt every 2 seconds
 *           by configuring CC[0] to half of timer overflow value. Events are created at odd number of seconds.
 *           For example, events are created at 1,3,5,... seconds. This event can be used to start Timer 0
 *           with Timer2->Event_Compare[0] triggering Timer 0 TASK_START through PPI.
*/
static void timer2_init(void)
{
    // Generate interrupt/event when half of time before the timer overflows has past, that is at 1,3,5,7... seconds from start.
    // Check TIMER1 configuration for details
    // now the overflow occurs every 0xFFFF/(SysClk/2^PRESCALER)
    // = 65535/31250 = 2.097 sec */
    ret_code_t err_code = nrf_drv_timer_init(&timer2, NULL, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(&timer2, NRF_TIMER_CC_CHANNEL0, 0x7FFFUL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
}

/*******************************************************************************
 * 描  述 : 串口初始化。波特率115200bps，流控关闭
 * 参  数 : 无
 * 返回值 : 无
 ******************************************************************************/
void uart_config(void)
{
    uint32_t err_code;

    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,    //关闭流控
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200  //波特率设置为115200bps
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}
/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);     //配置驱动LED指示灯的管脚
    LEDS_OFF(LEDS_MASK);           //关闭所有指示灯

    timer0_init(); // 初始化Timer0用作计数器
    timer1_init(); // 初始化Timer1用作定时器，偶数秒产生匹配事件
    timer2_init(); // 初始化Timer2用作定时器，奇数秒产生匹配事件
    ppi_init();    // 初始化PPI


    uart_config(); //初始化串口。波特率115200，流控关闭

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This example does not go to low power mode therefore constant latency is not needed.
    //       However this setting will ensure correct behaviour when routing TIMER events through
    //       PPI (shown in this example) and low power mode simultaneously.
    NRF_POWER->TASKS_CONSTLAT = 1;

    // Start clock.
    nrf_drv_timer_enable(&timer0);//使能Timer0
    nrf_drv_timer_enable(&timer1);//使能Timer1
    nrf_drv_timer_enable(&timer2);//使能Timer2

    while (true)
    {
        printf("Current count: %d\r\n", (int)nrf_drv_timer_capture(&timer0, NRF_TIMER_CC_CHANNEL0)); //打印出Timer0计数值

        nrf_drv_timer_increment(&timer0);//计数器Timer0计数值加1

        nrf_delay_ms(100);
        nrf_gpio_pin_toggle(LED_1);//翻转指示灯D1的状态，用于指示程序的运行

    }
}
/********************************************END FILE*******************************************/
