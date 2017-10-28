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
** Descriptions:		�����ʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"



/* ��������LEDռ�õ�nRF51822�ܽ���Դ
P0.21�����������ָʾ��D1
P0.22�����������ָʾ��D2
P0.23�����������ָʾ��D3
P0.24�����������ָʾ��D4

��Ҫ�̽Ӷ�Ӧ������ñ
*/

/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
	uint8_t i;
	
	//�������ڿ���OLED�Ĺܽ�
  nrf_gpio_range_cfg_output(LED_START, LED_STOP);//����P0.17~P0.20Ϊ���
  nrf_gpio_pin_set(LED_1);  //LED��ʼ״̬ΪϨ��
	nrf_gpio_pin_set(LED_2);
	nrf_gpio_pin_set(LED_3);
	nrf_gpio_pin_set(LED_4);


  while (true)
  {
    //����LED������˸2��
		for(i=0;i<3;i++) 
		{
		  nrf_gpio_pin_toggle(LED_1);
		  nrf_delay_ms(150);
		  nrf_gpio_pin_toggle(LED_2);
		  nrf_delay_ms(150);
		  nrf_gpio_pin_toggle(LED_3);
		  nrf_delay_ms(150);
		  nrf_gpio_pin_toggle(LED_4);
		  nrf_delay_ms(150);
		}
		//����LEDͬʱ��˸2��
		for(i=0;i<2;i++)
		{
		  nrf_gpio_pin_set(LED_1);
	    nrf_gpio_pin_set(LED_2);
	    nrf_gpio_pin_set(LED_3);
	    nrf_gpio_pin_set(LED_4);
		  nrf_delay_ms(150);
			nrf_gpio_pin_clear(LED_1);
	    nrf_gpio_pin_clear(LED_2);
	    nrf_gpio_pin_clear(LED_3);
	    nrf_gpio_pin_clear(LED_4);
			nrf_delay_ms(150);			
    }
		//Ϩ������LED
		nrf_gpio_pin_set(LED_1);
	  nrf_gpio_pin_set(LED_2);
	  nrf_gpio_pin_set(LED_3);
	  nrf_gpio_pin_set(LED_4);
	  nrf_delay_ms(500);
	}
}
/********************************************END FILE*******************************************/
