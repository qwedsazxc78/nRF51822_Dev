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
** Descriptions:		按键检测实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"


/* 开发板中LED和按键占用的nRF52832管脚资源
P0.17：输出：驱动指示灯D1
P0.18：输出：驱动指示灯D2
P0.19：输出：驱动指示灯D3
P0.20：输出：驱动指示灯D4

P0.13：输入：按键检测S1
P0.14：输入：按键检测S2
P0.15：输入：按键检测S3
P0.16：输入：按键检测S4

需要短接对应的跳线帽
*/
/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
	nrf_gpio_range_cfg_output(LED_START, LED_STOP);//配置P0.17~P0.20为输出
  nrf_gpio_pin_set(LED_1);  //LED初始状态为熄灭
	nrf_gpio_pin_set(LED_2);
	nrf_gpio_pin_set(LED_3);
	nrf_gpio_pin_set(LED_4);
	nrf_gpio_range_cfg_input(BUTTON_START,BUTTON_STOP,NRF_GPIO_PIN_PULLUP);//配置P0.13~P0.16为输入


  while (true)
  {
    //检测按键S1是否按下
		if(nrf_gpio_pin_read(BUTTON_1) == 0)
		{
			nrf_gpio_pin_clear(LED_1);
			while(nrf_gpio_pin_read(BUTTON_1) == 0);//等待按键释放
			nrf_gpio_pin_set(LED_1);
		}
		//检测按键S2是否按下
		if(nrf_gpio_pin_read(BUTTON_2) == 0)
		{
			nrf_gpio_pin_clear(LED_2);
			while(nrf_gpio_pin_read(BUTTON_2) == 0);//等待按键释放
			nrf_gpio_pin_set(LED_2);
		}
		//检测按键S3是否按下
		if(nrf_gpio_pin_read(BUTTON_3) == 0)
		{
			nrf_gpio_pin_clear(LED_3);
			while(nrf_gpio_pin_read(BUTTON_3) == 0);//等待按键释放
			nrf_gpio_pin_set(LED_3);
		}
		//检测按键S4是否按下
		if(nrf_gpio_pin_read(BUTTON_4) == 0)
		{
			nrf_gpio_pin_clear(LED_4);
			while(nrf_gpio_pin_read(BUTTON_4) == 0);//等待按键释放
			nrf_gpio_pin_set(LED_4);
		}
  }
}
/********************************************END FILE*******************************************/
