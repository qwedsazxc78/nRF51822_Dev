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
** Descriptions:    ADC�����������ѹ����ʵ��
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

/* ��������ָʾ�ƺʹ���ռ�õ�nRF52832�ܽ���Դ
P0.17�����������ָʾ��D1
P0.18�����������ָʾ��D2
P0.19�����������ָʾ��D3
P0.20�����������ָʾ��D4

P0.06��UART_TXD   �����ڷ���
P0.08��UART_RXD   �����ڽ���
P0.07��UART_CTS   : δʹ������
P0.05��UART_RTS   : δʹ������

P0.02�����������ѹ����

��Ҫ�̽Ӷ�Ӧ������ñ
*/

#define UART_TX_BUF_SIZE 256                              /**< UART TX �����С(�ֽ���) */
#define UART_RX_BUF_SIZE 1                                /**< UART RX �����С(�ֽ���) */
#define SAMPLES_IN_BUFFER 1                                 /**< ADC�������ݻ����С(�ֽ���)  */


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
 * ��  �� : ADC�жϴ�����
 * ��   ��: ��
 * ����ֵ : ��
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
 * ��  �� : ADC���á�����ͨ����AIN0���ֱ��ʣ�10λ
 * ��   ��: ��
 * ����ֵ : ��
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
 * ��  �� : main����
 * ��   ��: ��
 * ����ֵ : ��
 ***********************************************************************************************/
int main(void)
{
  uart_config();

  nrf_gpio_cfg_output(LED_1);//����P0.21Ϊ�������LEDָʾ��D1������ָʾ���������
  nrf_gpio_pin_set(LED_1);   //LED��ʼ״̬ΪϨ��
  saadc_init();

  while (true)
  {
    nrf_delay_ms(300);         //��ʱ������۲�SSADC����ֵ

    APP_ERROR_CHECK(nrf_drv_saadc_sample());
    nrf_gpio_pin_toggle(LED_1);//LED1״̬��ת
  }
}
/***********************************END FILE*******************************************/
