/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   使用的SDK版本-SDK_12.1.0
**				
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		RNG(随机数发生器)实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_rng.h"

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

#define UART_TX_BUF_SIZE 256    /**< UART TX 缓存大小 */
#define UART_RX_BUF_SIZE 1      /**< UART RX 缓存大小 */
#define RANDOM_BUFF_SIZE 16     /**< 随机数缓存大小 */

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
/** @brief Function for getting vector of random numbers.
 *
 * @param[out] p_buff                               Pointer to unit8_t buffer for storing the bytes.
 * @param[in]  length                               Number of bytes to take from pool and place in p_buff.
 *
 * @retval     Number of bytes actually placed in p_buff.
 */
uint8_t random_vector_generate(uint8_t * p_buff, uint8_t size)
{
    uint8_t available;
    uint32_t err_code;
    err_code = nrf_drv_rng_bytes_available(&available);
    APP_ERROR_CHECK(err_code);
    uint8_t length = (size<available) ? size : available;
    err_code = nrf_drv_rng_rand(p_buff,length);
    APP_ERROR_CHECK(err_code);
    return length;
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
    uint32_t err_code;
	
    uart_config(); //串口初始化

    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_rng_init(NULL);
    APP_ERROR_CHECK(err_code);
		
		nrf_gpio_cfg_output(LED_1);
		

    while (true)
    {
        uint8_t p_buff[RANDOM_BUFF_SIZE];
        uint8_t length = random_vector_generate(p_buff,RANDOM_BUFF_SIZE);
        printf("Random Vector:");
        for(uint8_t i = 0; i < length; i++)  //串口输出读取的随机数
        {
            printf(" %3d",(int)p_buff[i]);
        }
        printf("\r\n");              //回车换行
        nrf_delay_ms(500);           //延时，方便观察数据
				nrf_gpio_pin_toggle(LED_1);  //指示灯D1指示程序运行
    }
}
/********************************************END FILE*******************************************/
