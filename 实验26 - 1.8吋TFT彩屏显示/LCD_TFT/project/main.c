/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   1.0
** Descriptions:		
**						
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		1.8吋TFT液晶屏显示实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "Lcd_Driver.h"

/* 开发板中1.8吋TFT液晶屏占用的nRF52832管脚资源
CS  P0.22 --- LCD_TFT片选
RST P0.23 --- LCD_TFT复位
AO  P0.24 --- LCD_TFT数据/命令切换
SDA P0.25 --- LCD_TFT数据
SCK P0.26 --- LCD_TFT时钟
*/


void showqq(void)
{ 
	  uint16_t x,y; 
	
	  x=0;
	  y=75;
	  while(y<lcddev.height-39)
	  {
		    x=0;
		    while(x<lcddev.width-39)
		    {
			      showimage(x,y);	
			      x+=40;
		    }
		    y+=40;
	  }	  
}
/**********************************************************************************************
 * 描  述 : main函数
 * 入  参 : 无
 * 返回值 : 无
 ***********************************************************************************************/ 
int main(void)
{
		nrf_gpio_cfg_output(LCD_CS);
		nrf_gpio_cfg_output(LCD_RST);
		nrf_gpio_cfg_output(LCD_RS);
		nrf_gpio_cfg_output(LCD_SDA);
		nrf_gpio_cfg_output(LCD_CLK);
		
		nrf_delay_ms(200);  
		
		LCD_Init();	  			
 	  POINT_COLOR = RED;//设置字体为红色 


    while (true)
    {			  
			  LCD_Clear(BLUE);
			  BACK_COLOR=BLUE;
	      POINT_COLOR=RED;   //设置画笔为红色 
	      //显示32*32汉字
	      showhanzi32(0,0,0);	      //艾
	      showhanzi32(40,0,1);	    //克
	      showhanzi32(80,0,2);      //姆
	      //显示16*16汉字
	      showhanzi16(15,35,0);	    //艾
	      showhanzi16(35,35,1);	    //克
	      showhanzi16(55,35,2);	    //姆
	      showhanzi16(75,35,3);	    //科
	      showhanzi16(95,35,4);	    //技
			  //显示字符
	      LCD_ShowString(25,55,200,16,16,"BLUETOOTH");
			  nrf_delay_ms(3000); 
			  //显示图像
			  LCD_Clear(WHITE);
        showqq();     
        nrf_delay_ms(3000); 			
    }
}
/********************************************END FILE*******************************************/
