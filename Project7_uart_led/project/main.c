/****************************************Copyright (c)****************************************************
**                                        
**                                      [����ķ�Ƽ�]
**                                        IIKMSIK
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:         
** Last Version:		   
** Descriptions:		   ʹ�õ�SDK�汾-SDK_12.1.0
**						
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-1
** Version:			    1.0
** Descriptions:		UART����ָʾ��ʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"


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

#define UART_TX_BUF_SIZE 256    /**< UART TX �����С */
#define UART_RX_BUF_SIZE 1      /**< UART RX �����С */
#define RXBUF_LEN  3           //UART���ջ����ֽ���

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
/*******************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/ 
int main(void)
{
    uint8_t RxCnt = 0;             //UART�����ֽ���
	  uint8_t UartRxBuf[RXBUF_LEN];  //UART���ջ���
	
	  LEDS_CONFIGURE(LEDS_MASK);     //��������LEDָʾ�ƵĹܽ�
    LEDS_OFF(LEDS_MASK);           //�ر�����ָʾ��
    
	  uart_config(); //���ڳ�ʼ��

    while (true)
    {
        uint8_t cr;
        while(app_uart_get(&cr) != NRF_SUCCESS);

        if((cr != '#')&&(RxCnt < 3))
			  {
				    UartRxBuf[RxCnt++] = cr;
			  }
			  else
			  {
				    if(RxCnt >= 3)
				    {
					      RxCnt = 0;
				    }
				    else
				    {
					      if((UartRxBuf[0] == 'D') || (UartRxBuf[0] == 'd'))
					      {
						        switch(UartRxBuf[1]-48)
						        {
							          case 1:
											  LEDS_OFF(LEDS_MASK);       //�ر�����ָʾ��
												nrf_gpio_pin_clear(LED_1); //����D1ָʾ��
							          RxCnt = 0;
							          break;
							
							          case 2:
								        LEDS_OFF(LEDS_MASK);       //�ر�����ָʾ��
												nrf_gpio_pin_clear(LED_2); //����D2ָʾ��
							          RxCnt = 0;
							          break;
												
												case 3:
											  LEDS_OFF(LEDS_MASK);       //�ر�����ָʾ��
												nrf_gpio_pin_clear(LED_3); //����D3ָʾ��
							          RxCnt = 0;
							          break;
							
							          case 4:
								        LEDS_OFF(LEDS_MASK);       //�ر�����ָʾ��
												nrf_gpio_pin_clear(LED_4); //����D4ָʾ��
							          RxCnt = 0;
							          break;
							
							          default:
								        break;
						       }
					     }
				   }
			  }	
    }
}
/********************************************END FILE*******************************************/
