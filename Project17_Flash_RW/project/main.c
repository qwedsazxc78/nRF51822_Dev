/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                main.c
** Last modified Date:
** Last Version:
** Descriptions:           ʹ�õ�SDK�汾-SDK_11.0.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:          FiYu
** Created date:        2016-10-2
** Version:             1.0
** Descriptions:        Ƭ��FLASH��дʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"


/* �������д���ռ�õ�nRF52832�ܽ���Դ
P0.06��UART_TXD   �����ڷ���
P0.08��UART_RXD   �����ڽ���
P0.07��UART_CTS   : δʹ������
P0.05��UART_RTS   : δʹ������


��Ҫ�̽Ӷ�Ӧ������ñ
*/

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                           /**< UART RX buffer size. */

//�����жϴ�����
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
 * ��  �� : ���ڳ�ʼ����������115200bps�����عر�
 * ��  �� : ��
 * ����ֵ : ��
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
        APP_UART_FLOW_CONTROL_DISABLED,    //�ر�����
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200  //����������Ϊ115200bps
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}
/** ����flash��ҳ
 * ����һ��ҳʱֻ����ERASEPAGE�Ĵ���д���ҳ��һ���ֵĵ�ַ����
 */
static void flash_page_erase(uint32_t * page_address)
{
    // ���üĴ���CONFIG.WEN = 2 ʹ��FLASH�Ĳ�������
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    // �ȴ�NVMC(����ʧ�洢��������)׼������
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // ����ҳ ����һ��ҳʱֻ����ERASEPAGE�Ĵ���д���ҳ��һ���ֵĵ�ַ����
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;

    // �ȴ�NVMC(����ʧ�洢��������)׼������
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // ���üĴ���CONFIG.WEN = 0 �رղ�������FLASH����ֻ��״̬
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    // �ȴ�NVMC(����ʧ�洢��������)׼������
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}


/** ��FLASHָ����ַд��һ����
 */
static void flash_word_write(uint32_t * address, uint32_t value)
{
    // ���üĴ���CONFIG.WEN = 1 ʹ��FLASH��д����
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    // �ȴ�NVMC(����ʧ�洢��������)׼������
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    //��õ�ַд��һ����value
    *address = value;

    // �ȴ�NVMC(����ʧ�洢��������)׼������
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // ���üĴ���CONFIG.WEN = 0 ���ر�д����
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    // �ȴ�NVMC(����ʧ�洢��������)׼������
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
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
    pg_num  = NRF_FICR->CODESIZE - 1;  // ʹ��flash�����һҳ

    while (true)
    {
        // ������ҳ����ʼ��ַ
        addr = (uint32_t *)(pg_size * pg_num);
        // ����ҳ
        flash_page_erase(addr);
        i = 0;

        do
        {
            printf("Enter char to write to flash\r\n");

            // �ȴ����մ��ڷ����������ݣ����յ����ݺ�д�뵽FLASH:
            do
            {
                err_code = app_uart_get(&patwr);
            }
            while (err_code == NRF_ERROR_NOT_FOUND);

            //д�����ݵ�FLASH
            if (patold != patwr)
            {
                patold = patwr;
                flash_word_write(addr, (uint32_t)patwr);
                ++addr;
                i += 4;
                printf("'%c' was write to flash\r\n", patwr);
            }
            // �������ݲ�ͨ�����ڴ�ӡ������
            patrd = (uint8_t) * (addr - 1);
            printf("'%c' was read from flashr\r\n", patrd);
        }
        while (i < pg_size);
    }
}
/********************************************END FILE*******************************************/
