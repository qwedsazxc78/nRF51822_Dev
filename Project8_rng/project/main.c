/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   ʹ�õ�SDK�汾-SDK_12.1.0
**				
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		RNG(�����������)ʵ��
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
#define RANDOM_BUFF_SIZE 16     /**< ����������С */

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
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
    uint32_t err_code;
	
    uart_config(); //���ڳ�ʼ��

    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_rng_init(NULL);
    APP_ERROR_CHECK(err_code);
		
		nrf_gpio_cfg_output(LED_1);
		

    while (true)
    {
        uint8_t p_buff[RANDOM_BUFF_SIZE];
        uint8_t length = random_vector_generate(p_buff,RANDOM_BUFF_SIZE);
        printf("Random Vector:");
        for(uint8_t i = 0; i < length; i++)  //���������ȡ�������
        {
            printf(" %3d",(int)p_buff[i]);
        }
        printf("\r\n");              //�س�����
        nrf_delay_ms(500);           //��ʱ������۲�����
				nrf_gpio_pin_toggle(LED_1);  //ָʾ��D1ָʾ��������
    }
}
/********************************************END FILE*******************************************/
