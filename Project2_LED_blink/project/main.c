/****************************************Copyright (c)****************************************************
**                                        
**                                      [艾克姆科技]
**                                        IIKMSIK
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   使用的SDK版本-SDK_12.1.0
**						
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-1
** Version:			    1.0
** Descriptions:		跑马灯实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"



/* 开发板中LED占用的nRF51822管脚资源
P0.21：输出：驱动指示灯D1
P0.22：输出：驱动指示灯D2
P0.23：输出：驱动指示灯D3
P0.24：输出：驱动指示灯D4

需要短接对应的跳线帽
*/

/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
	uint8_t i;
	
	//配置用于控制OLED的管脚
  nrf_gpio_range_cfg_output(LED_START, LED_STOP);//配置P0.17~P0.20为输出
  nrf_gpio_pin_set(LED_1);  //LED初始状态为熄灭
	nrf_gpio_pin_set(LED_2);
	nrf_gpio_pin_set(LED_3);
	nrf_gpio_pin_set(LED_4);


  while (true)
  {
    //所有LED轮流闪烁2次
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
		//所有LED同时闪烁2次
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
		//熄灭所有LED
		nrf_gpio_pin_set(LED_1);
	  nrf_gpio_pin_set(LED_2);
	  nrf_gpio_pin_set(LED_3);
	  nrf_gpio_pin_set(LED_4);
	  nrf_delay_ms(500);
	}
}
/********************************************END FILE*******************************************/
