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
** Descriptions:    ADC检测光敏电阻电压采样实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_saadc.h"

/* 开发板中指示灯和串口占用的nRF52832管脚资源
P0.17：输出：驱动指示灯D1
P0.18：输出：驱动指示灯D2
P0.19：输出：驱动指示灯D3
P0.20：输出：驱动指示灯D4

P0.06：UART_TXD   ：串口发送
P0.08：UART_RXD   ：串口接收
P0.07：UART_CTS   : 未使用流控
P0.05：UART_RTS   : 未使用流控

P0.02：光敏电阻电压采样

需要短接对应的跳线帽
*/

#define UART_TX_BUF_SIZE 256                              /**< UART TX 缓存大小(字节数) */
#define UART_RX_BUF_SIZE 1                                /**< UART RX 缓存大小(字节数) */
#define SAMPLES_IN_BUFFER 1                                 /**< ADC采样数据缓存大小(字节数)  */


static nrf_saadc_value_t       m_buffer_pool[2][SAMPLES_IN_BUFFER];



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
 * 描  述 : ADC中断处理函数
 * 参   数: 无
 * 返回值 : 无
 ***********************************************************************************************/
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
  if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
  {
    ret_code_t err_code;

    err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
    printf("V%d", p_event->data.done.p_buffer[0]);
  }
}
/**********************************************************************************************
 * 描  述 : ADC配置。输入通道：AIN0，分辨率：10位
 * 参   数: 无
 * 返回值 : 无
 ***********************************************************************************************/
void saadc_init(void)
{
  ret_code_t err_code;
  nrf_saadc_channel_config_t channel_config =
    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
  err_code = nrf_drv_saadc_init(NULL, saadc_callback);

  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_channel_init(0, &channel_config);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
  APP_ERROR_CHECK(err_code);
}
/**********************************************************************************************
 * 描  述 : main函数
 * 参   数: 无
 * 返回值 : 无
 ***********************************************************************************************/
int main(void)
{
  uart_config();

  nrf_gpio_cfg_output(LED_1);//配置P0.21为输出驱动LED指示灯D1，用于指示程序的运行
  nrf_gpio_pin_set(LED_1);   //LED初始状态为熄灭
  saadc_init();

  while (true)
  {
    nrf_delay_ms(300);         //延时，方便观察SSADC采样值

    APP_ERROR_CHECK(nrf_drv_saadc_sample());
    nrf_gpio_pin_toggle(LED_1);//LED1状态翻转
  }
}
/***********************************END FILE*******************************************/
