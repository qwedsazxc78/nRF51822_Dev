/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
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
** Descriptions:		温湿度传感器DHT11实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "DHT11.h"

/* 开发板中温湿度传感器检测、串口占用的nRF52832管脚资源
P0.06：UART_TXD   ：串口发送
P0.08：UART_RXD   ：串口接收
P0.07：UART_CTS   : 未使用流控
P0.05：UART_RTS   : 未使用流控

串口需要短接对应的跳线帽

P0.11：温湿度传感器DHT11检测

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
/*******************************************************************************************************
 * 描  述 : 串口输出字符串
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void PutString(uint8_t *s)
{
  while(*s != 0)
	  while(app_uart_put(*s++) != NRF_SUCCESS);
}
/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
    uint8_t temp[6]; 
	
	  nrf_gpio_cfg_output(LED_1);//配置管脚P0.21为输出，驱动指示灯D1
    nrf_gpio_pin_set(LED_1);   //设置指示灯D1初始状态为熄灭
	
	  uart_config();  //配置串口，关闭流控，波特率：115200	

    while (true)
    {
    	  memset(temp, 0, 6);
		
		    DHT11(); //获取温度值
		
		    //将温度值转换为字符串
			  temp[0] = 'H';   
		    temp[1] = DHT11Value.tempH + 0x30;
        temp[2] = DHT11Value.tempL + 0x30;
        temp[3] = DHT11Value.humyH + 0x30;
        temp[4] = DHT11Value.humyL + 0x30;
		
		    //串口打印温度值
        PutString(temp);
		    nrf_delay_ms(300);           //延时，方便观察实验数据
        nrf_gpio_pin_toggle(LED_1);  //指示灯D1状态翻转，指示程序的运行
  }	
}
/********************************************END FILE*******************************************/
