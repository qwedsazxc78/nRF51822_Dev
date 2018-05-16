/****************************************Copyright (c)****************************************************
**
**                                      [艾克姆科技]
**                                        IIKMSIK
**--------------File Info---------------------------------------------------------------------------------
** File name:                main.c
** Last modified Date:
** Last Version:
** Descriptions:           使用的SDK版本-SDK_12.1.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:          FiYu
** Created date:        2016-10-1
** Version:             1.0
** Descriptions:        UART控制指示灯实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"

#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"

#include "boards.h"
#include "bsp.h"
#include "nrf.h"

#include "mb.h"
#include "mbutils.h"

//保持寄存器起始地址
#define REG_HOLDING_START 0x0000
//保持寄存器数量
#define REG_HOLDING_NREGS 8
//保持寄存器内容
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS]
    = {0x147b, 0x3f8e, 0x147b, 0x400e, 0x1eb8, 0x4055, 0x147b, 0x408e};


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


//#define UART_TX_BUF_SIZE 256    /**< UART TX 缓存大小 */
//#define UART_RX_BUF_SIZE 1      /**< UART RX 缓存大小 */
//#define RXBUF_LEN  3           //UART接收缓存字节数

//void uart_error_handle(app_uart_evt_t * p_event)
//{
//    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
//    {
//        APP_ERROR_HANDLER(p_event->data.error_communication);
//    }
//    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
//    {
//        APP_ERROR_HANDLER(p_event->data.error_code);
//    }
//}
///*******************************************************************************
// * 描  述 : 串口初始化。波特率115200bps，流控关闭
// * 参  数 : 无
// * 返回值 : 无
// ******************************************************************************/
//void uart_config(void)
//{
//    uint32_t err_code;

//    const app_uart_comm_params_t comm_params =
//    {
//        RX_PIN_NUMBER,
//        TX_PIN_NUMBER,
//        RTS_PIN_NUMBER,
//        CTS_PIN_NUMBER,
//        APP_UART_FLOW_CONTROL_DISABLED,    //关闭流控
//        false,
//        UART_BAUDRATE_BAUDRATE_Baud115200  //波特率设置为115200bps
//    };

//    APP_UART_FIFO_INIT(&comm_params,
//                       UART_RX_BUF_SIZE,
//                       UART_TX_BUF_SIZE,
//                       uart_error_handle,
//                       APP_IRQ_PRIORITY_LOW,
//                       err_code);

//    APP_ERROR_CHECK(err_code);
//}
/*******************************************************************************
 * 描  述 : main函数
 * 参  数 : 无
 * 返回值 : 无
 ******************************************************************************/

int main(void)
{
    uint8_t RxCnt = 0;             //UART接收字节数
    eMBErrorCode    eStatus;

    LEDS_CONFIGURE(LEDS_MASK);     //配置驱动LED指示灯的管脚
    LEDS_OFF(LEDS_MASK);           //关闭所有指示灯

    //uart_config(); //串口初始化
    //timer_config();

    // inti RTU slave address = 0x01, USART1, 9600, None Parity
    eStatus = eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE);
    // Enable FreeModbus
    eStatus = eMBEnable();

    while (true)
    {
        eStatus = eMBPoll();
				nrf_gpio_pin_toggle(LED_1);
				nrf_delay_ms(200);
				
    }

}
/********************************************END FILE*******************************************/
