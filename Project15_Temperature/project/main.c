/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:          main.c
** Last modified Date:
** Last Version:
** Descriptions:       使用的SDK版本-SDK_11.0.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:      FiYu
** Created date:    2016-10-2
** Version:         1.0
** Descriptions:    片内温度传感器实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_temp.h"

/* 开发板中LED、串口占用的nRF52832管脚资源
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
    APP_UART_FLOW_CONTROL_DISABLED,    //禁止流控
    false,
    UART_BAUDRATE_BAUDRATE_Baud115200//波特率设置为115200
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
  int32_t volatile temp;


  LEDS_CONFIGURE(LEDS_MASK);
  LEDS_OFF(LEDS_MASK);

  uart_config();      //串口初始化
  nrf_temp_init();

  while (true)
  {
    NRF_TEMP->TASKS_START = 1; // 触发片内温度传感器Start任务，启动温度检测

    // 等待检测完成，这里采用的是查询的方式。如果采用中断的方式，就可以不用一直等待检测完成
    while (NRF_TEMP->EVENTS_DATARDY == 0)
    {
      // Do nothing.
    }
    NRF_TEMP->EVENTS_DATARDY = 0;

    //读取温度值
    temp = (nrf_temp_read() / 4);

    //温度传感器模块的模拟电路在检测完成后不会自动关闭，需要手动关闭以节省功耗
    NRF_TEMP->TASKS_STOP = 1; // 触发Stop任务关闭片内温度传感器

    printf("T%d", (int)temp);    //串口打印温度值
    nrf_delay_ms(500);           //延时500ms，方便观察实验现象
    nrf_gpio_pin_toggle(LED_1);  //指示灯D1状态翻转指示程序运行
  }
}
/********************************************END FILE*******************************************/
