/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   
**				
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2017-3-1
** Version:			    1.0
** Descriptions:		读取FICR中的地址类型  地址  ID
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"



/* 开发板中串口占用的nRF52832管脚资源
P0.09：UART_TXD   ：串口发送
P0.11：UART_RXD   ：串口接收
P0.07：UART_CTS   ：未使用流控
P0.05：UART_RTS   ：未使用流控

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
uint32_t GetDeviceType(void)
{
	return NRF_FICR->DEVICEADDRTYPE;
	
}
uint32_t GetDeviceID(uint8_t i)
{
	return NRF_FICR->DEVICEID[i];
	
}
uint32_t GetDeviceAddr(uint8_t i)
{
	return NRF_FICR->DEVICEADDR[i];
	
}
/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
    uint32_t DeviceType,DeviceID[2],DeviceAddr[2];   
	
	  LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    
    uart_config();//串口初始化
    while (true)
    {
        DeviceType = GetDeviceType();      //获取设备地址类型
		    DeviceID[0] = GetDeviceID(0);      //读取唯一ID
		    DeviceID[1] = GetDeviceID(1);
		    DeviceAddr[0] = GetDeviceAddr(0);  //读取设备地址
		    DeviceAddr[1] = GetDeviceAddr(1);
			
			  printf("DeviceAddrType is:%X\r\n",DeviceType);               //串口打印设备地址类型
			  printf("DeviceID is:%X%X\r\n",DeviceID[0],DeviceID[1]);      //串口打印唯一ID			
			  printf("DeviceAddr is:%X\r\n",DeviceAddr[0]);//串口打印设备地址DEVICEADDR[0]
			  printf("DeviceAddr is:%X\r\n",DeviceAddr[1]);//串口打印设备地址DEVICEADDR[1]
			  printf("\r\n");      //回车换行
			  nrf_delay_ms(2000);  //延时2秒，方便观察数据
    }
}
/********************************************END FILE*******************************************/
