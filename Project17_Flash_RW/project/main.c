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
** Descriptions:        片内FLASH读写实验
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
P0.06：UART_TXD   ：串口发送
P0.08：UART_RXD   ：串口接收
P0.07：UART_CTS   : 未使用流控
P0.05：UART_RTS   : 未使用流控


需要短接对应的跳线帽
*/

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                           /**< UART RX buffer size. */

//串口中断处理函数
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
/** 擦除flash的页
 * 擦除一个页时只需向ERASEPAGE寄存器写入该页第一个字的地址即可
 */
static void flash_page_erase(uint32_t * page_address)
{
    // 设置寄存器CONFIG.WEN = 2 使能FLASH的擦除操作
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    // 等待NVMC(非易失存储器控制器)准备就绪
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // 擦除页 擦除一个页时只需向ERASEPAGE寄存器写入该页第一个字的地址即可
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;

    // 等待NVMC(非易失存储器控制器)准备就绪
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // 设置寄存器CONFIG.WEN = 0 关闭擦除，即FLASH处于只读状态
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    // 等待NVMC(非易失存储器控制器)准备就绪
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}


/** 向FLASH指定地址写入一个字
 */
static void flash_word_write(uint32_t * address, uint32_t value)
{
    // 设置寄存器CONFIG.WEN = 1 使能FLASH的写操作
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    // 等待NVMC(非易失存储器控制器)准备就绪
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    //向该地址写入一个字value
    *address = value;

    // 等待NVMC(非易失存储器控制器)准备就绪
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // 设置寄存器CONFIG.WEN = 0 ，关闭写功能
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    // 等待NVMC(非易失存储器控制器)准备就绪
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/
int main(void)
{
    uint32_t * addr;
    uint8_t    patwr;
    uint8_t    patrd;
    uint8_t    patold;
    uint32_t   i;
    uint32_t   pg_size;
    uint32_t   pg_num;
    uint32_t err_code;

    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    uart_config();

    printf("Flashwrite example\n\r");
    patold  = 0;
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num  = NRF_FICR->CODESIZE - 1;  // 使用flash的最后一页

    while (true)
    {
        // 待擦除页的起始地址
        addr = (uint32_t *)(pg_size * pg_num);
        // 擦除页
        flash_page_erase(addr);
        i = 0;

        do
        {
            printf("Enter char to write to flash\r\n");

            // 等待接收串口发送来的数据，接收到数据后，写入到FLASH:
            do
            {
                err_code = app_uart_get(&patwr);
            }
            while (err_code == NRF_ERROR_NOT_FOUND);

            //写入数据到FLASH
            if (patold != patwr)
            {
                patold = patwr;
                flash_word_write(addr, (uint32_t)patwr);
                ++addr;
                i += 4;
                printf("'%c' was write to flash\r\n", patwr);
            }
            // 读出数据并通过串口打印出数据
            patrd = (uint8_t) * (addr - 1);
            printf("'%c' was read from flashr\r\n", patrd);
        }
        while (i < pg_size);
    }
}
/********************************************END FILE*******************************************/
