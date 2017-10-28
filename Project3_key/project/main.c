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
** Descriptions:		�������ʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"


/* ��������LED�Ͱ���ռ�õ�nRF52832�ܽ���Դ
P0.17�����������ָʾ��D1
P0.18�����������ָʾ��D2
P0.19�����������ָʾ��D3
P0.20�����������ָʾ��D4

P0.13�����룺�������S1
P0.14�����룺�������S2
P0.15�����룺�������S3
P0.16�����룺�������S4

��Ҫ�̽Ӷ�Ӧ������ñ
*/
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
	nrf_gpio_range_cfg_output(LED_START, LED_STOP);//����P0.17~P0.20Ϊ���
  nrf_gpio_pin_set(LED_1);  //LED��ʼ״̬ΪϨ��
	nrf_gpio_pin_set(LED_2);
	nrf_gpio_pin_set(LED_3);
	nrf_gpio_pin_set(LED_4);
	nrf_gpio_range_cfg_input(BUTTON_START,BUTTON_STOP,NRF_GPIO_PIN_PULLUP);//����P0.13~P0.16Ϊ����


  while (true)
  {
    //��ⰴ��S1�Ƿ���
		if(nrf_gpio_pin_read(BUTTON_1) == 0)
		{
			nrf_gpio_pin_clear(LED_1);
			while(nrf_gpio_pin_read(BUTTON_1) == 0);//�ȴ������ͷ�
			nrf_gpio_pin_set(LED_1);
		}
		//��ⰴ��S2�Ƿ���
		if(nrf_gpio_pin_read(BUTTON_2) == 0)
		{
			nrf_gpio_pin_clear(LED_2);
			while(nrf_gpio_pin_read(BUTTON_2) == 0);//�ȴ������ͷ�
			nrf_gpio_pin_set(LED_2);
		}
		//��ⰴ��S3�Ƿ���
		if(nrf_gpio_pin_read(BUTTON_3) == 0)
		{
			nrf_gpio_pin_clear(LED_3);
			while(nrf_gpio_pin_read(BUTTON_3) == 0);//�ȴ������ͷ�
			nrf_gpio_pin_set(LED_3);
		}
		//��ⰴ��S4�Ƿ���
		if(nrf_gpio_pin_read(BUTTON_4) == 0)
		{
			nrf_gpio_pin_clear(LED_4);
			while(nrf_gpio_pin_read(BUTTON_4) == 0);//�ȴ������ͷ�
			nrf_gpio_pin_set(LED_4);
		}
  }
}
/********************************************END FILE*******************************************/
