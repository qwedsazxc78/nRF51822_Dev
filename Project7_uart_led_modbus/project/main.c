/****************************************Copyright (c)****************************************************
**
**                                      [����ķ�Ƽ�]
**                                        IIKMSIK
**--------------File Info---------------------------------------------------------------------------------
** File name:                main.c
** Last modified Date:
** Last Version:
** Descriptions:           ʹ�õ�SDK�汾-SDK_12.1.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:          FiYu
** Created date:        2016-10-1
** Version:             1.0
** Descriptions:        UART����ָʾ��ʵ��
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

//���ּĴ�����ʼ��ַ
#define REG_HOLDING_START 0x0000
//���ּĴ�������
#define REG_HOLDING_NREGS 8
//���ּĴ�������
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS]
    = {0x147b, 0x3f8e, 0x147b, 0x400e, 0x1eb8, 0x4055, 0x147b, 0x408e};


/* ��������ָʾ�ƺʹ���ռ�õ�nRF52832�ܽ���Դ
P0.17�����������ָʾ��D1
P0.18�����������ָʾ��D2
P0.19�����������ָʾ��D3
P0.20�����������ָʾ��D4

P0.06��UART_TXD   �����ڷ���
P0.08��UART_RXD   �����ڽ���
P0.07��UART_CTS   : δʹ������
P0.05��UART_RTS   : δʹ������

��Ҫ�̽Ӷ�Ӧ������ñ
*/


//#define UART_TX_BUF_SIZE 256    /**< UART TX �����С */
//#define UART_RX_BUF_SIZE 1      /**< UART RX �����С */
//#define RXBUF_LEN  3           //UART���ջ����ֽ���

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
// * ��  �� : ���ڳ�ʼ����������115200bps�����عر�
// * ��  �� : ��
// * ����ֵ : ��
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
//        APP_UART_FLOW_CONTROL_DISABLED,    //�ر�����
//        false,
//        UART_BAUDRATE_BAUDRATE_Baud115200  //����������Ϊ115200bps
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
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/

int main(void)
{
    uint8_t RxCnt = 0;             //UART�����ֽ���
    eMBErrorCode    eStatus;

    LEDS_CONFIGURE(LEDS_MASK);     //��������LEDָʾ�ƵĹܽ�
    LEDS_OFF(LEDS_MASK);           //�ر�����ָʾ��

    //uart_config(); //���ڳ�ʼ��
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
