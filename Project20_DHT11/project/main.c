/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   ʹ�õ�SDK�汾-SDK_11.0.0
**				
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		��ʪ�ȴ�����DHT11ʵ��
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

/* ����������ʪ�ȴ�������⡢����ռ�õ�nRF52832�ܽ���Դ
P0.06��UART_TXD   �����ڷ���
P0.08��UART_RXD   �����ڽ���
P0.07��UART_CTS   : δʹ������
P0.05��UART_RTS   : δʹ������

������Ҫ�̽Ӷ�Ӧ������ñ

P0.11����ʪ�ȴ�����DHT11���

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
/*******************************************************************************************************
 * ��  �� : ��������ַ���
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void PutString(uint8_t *s)
{
  while(*s != 0)
	  while(app_uart_put(*s++) != NRF_SUCCESS);
}
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
    uint8_t temp[6]; 
	
	  nrf_gpio_cfg_output(LED_1);//���ùܽ�P0.21Ϊ���������ָʾ��D1
    nrf_gpio_pin_set(LED_1);   //����ָʾ��D1��ʼ״̬ΪϨ��
	
	  uart_config();  //���ô��ڣ��ر����أ������ʣ�115200	

    while (true)
    {
    	  memset(temp, 0, 6);
		
		    DHT11(); //��ȡ�¶�ֵ
		
		    //���¶�ֵת��Ϊ�ַ���
			  temp[0] = 'H';   
		    temp[1] = DHT11Value.tempH + 0x30;
        temp[2] = DHT11Value.tempL + 0x30;
        temp[3] = DHT11Value.humyH + 0x30;
        temp[4] = DHT11Value.humyL + 0x30;
		
		    //���ڴ�ӡ�¶�ֵ
        PutString(temp);
		    nrf_delay_ms(300);           //��ʱ������۲�ʵ������
        nrf_gpio_pin_toggle(LED_1);  //ָʾ��D1״̬��ת��ָʾ���������
  }	
}
/********************************************END FILE*******************************************/
