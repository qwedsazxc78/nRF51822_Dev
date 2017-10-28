/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:          main.c
** Last modified Date:
** Last Version:
** Descriptions:       ʹ�õ�SDK�汾-SDK_11.0.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:      FiYu
** Created date:    2016-10-2
** Version:         1.0
** Descriptions:    Ƭ���¶ȴ�����ʵ��
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

/* ��������LED������ռ�õ�nRF52832�ܽ���Դ
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
    APP_UART_FLOW_CONTROL_DISABLED,    //��ֹ����
    false,
    UART_BAUDRATE_BAUDRATE_Baud115200//����������Ϊ115200
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
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/
int main(void)
{
  int32_t volatile temp;


  LEDS_CONFIGURE(LEDS_MASK);
  LEDS_OFF(LEDS_MASK);

  uart_config();      //���ڳ�ʼ��
  nrf_temp_init();

  while (true)
  {
    NRF_TEMP->TASKS_START = 1; // ����Ƭ���¶ȴ�����Start���������¶ȼ��

    // �ȴ������ɣ�������õ��ǲ�ѯ�ķ�ʽ����������жϵķ�ʽ���Ϳ��Բ���һֱ�ȴ�������
    while (NRF_TEMP->EVENTS_DATARDY == 0)
    {
      // Do nothing.
    }
    NRF_TEMP->EVENTS_DATARDY = 0;

    //��ȡ�¶�ֵ
    temp = (nrf_temp_read() / 4);

    //�¶ȴ�����ģ���ģ���·�ڼ����ɺ󲻻��Զ��رգ���Ҫ�ֶ��ر��Խ�ʡ����
    NRF_TEMP->TASKS_STOP = 1; // ����Stop����ر�Ƭ���¶ȴ�����

    printf("T%d", (int)temp);    //���ڴ�ӡ�¶�ֵ
    nrf_delay_ms(500);           //��ʱ500ms������۲�ʵ������
    nrf_gpio_pin_toggle(LED_1);  //ָʾ��D1״̬��תָʾ��������
  }
}
/********************************************END FILE*******************************************/
