/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   
**				
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2017-3-1
** Version:			    1.0
** Descriptions:		��ȡFICR�еĵ�ַ����  ��ַ  ID
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
P0.09��UART_TXD   �����ڷ���
P0.11��UART_RXD   �����ڽ���
P0.07��UART_CTS   ��δʹ������
P0.05��UART_RTS   ��δʹ������

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
uint32_t GetDeviceType(void)
{
	return NRF_FICR->DEVICEADDRTYPE;
	
}
uint32_t GetDeviceID(uint8_t i)
{
	return NRF_FICR->DEVICEID[i];
	
}
uint32_t GetDeviceAddr(uint8_t i)
{
	return NRF_FICR->DEVICEADDR[i];
	
}
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
    uint32_t DeviceType,DeviceID[2],DeviceAddr[2];   
	
	  LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    
    uart_config();//���ڳ�ʼ��
    while (true)
    {
        DeviceType = GetDeviceType();      //��ȡ�豸��ַ����
		    DeviceID[0] = GetDeviceID(0);      //��ȡΨһID
		    DeviceID[1] = GetDeviceID(1);
		    DeviceAddr[0] = GetDeviceAddr(0);  //��ȡ�豸��ַ
		    DeviceAddr[1] = GetDeviceAddr(1);
			
			  printf("DeviceAddrType is:%X\r\n",DeviceType);               //���ڴ�ӡ�豸��ַ����
			  printf("DeviceID is:%X%X\r\n",DeviceID[0],DeviceID[1]);      //���ڴ�ӡΨһID			
			  printf("DeviceAddr is:%X\r\n",DeviceAddr[0]);//���ڴ�ӡ�豸��ַDEVICEADDR[0]
			  printf("DeviceAddr is:%X\r\n",DeviceAddr[1]);//���ڴ�ӡ�豸��ַDEVICEADDR[1]
			  printf("\r\n");      //�س�����
			  nrf_delay_ms(2000);  //��ʱ2�룬����۲�����
    }
}
/********************************************END FILE*******************************************/
