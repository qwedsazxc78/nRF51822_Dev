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
** Created date:		2016-5-19
** Version:			    1.0
** Descriptions:		MPU6050ԭʼ���ݶ�ȡʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "mpu6050.h"
#include "twi_master.h"


/* ��������MPU6050ģ��ʹ���ռ�õ�nRF52832�ܽ���Դ
P0.06��UART_TXD   �����ڷ���
P0.08��UART_RXD   �����ڽ���
P0.07��UART_CTS   : δʹ������
P0.05��UART_RTS   : δʹ������

������Ҫ�̽Ӷ�Ӧ������ñ

P0.22��IICʱ��
P0.23��IIC����
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
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
    int16_t AccValue[3],GyroValue[3];
	  uint8_t id;
	
	  nrf_gpio_cfg_output(LED_1);//���ùܽ�P0.17Ϊ���������ָʾ��D1
    nrf_gpio_pin_set(LED_1);   //����ָʾ��D1��ʼ״̬ΪϨ��
	
    uart_config();  //���ô��ڣ���ֹ���أ������ʣ�115200	
		
		twi_master_init();
	
	  nrf_delay_ms(2000);
	
	  while(mpu6050_init(0x68) == false)
		{
				printf("mpu6050 init fail\r\n");
				nrf_delay_ms(500);
		}

		printf("mpu6050 init ok\r\n");
    mpu6050_register_read(0x75U, &id, 1);
		printf("mpu6050 id is %d \r\n",id);

    while (true)
    {
			  MPU6050_ReadAcc( &AccValue[0], &AccValue[1] , &AccValue[2] );
		    MPU6050_ReadGyro(&GyroValue[0] , &GyroValue[1] , &GyroValue[2] );
			
			  printf("ACC:  %d	%d	%d	",AccValue[0],AccValue[1],AccValue[2]);
		    printf("GYRO: %d	%d	%d	\r\n",GyroValue[0],GyroValue[1],GyroValue[2]);
			  nrf_delay_ms(500);
    }
}
/********************************************END FILE*******************************************/
