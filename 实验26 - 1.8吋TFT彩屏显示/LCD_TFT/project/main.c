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
** Descriptions:		1.8��TFTҺ������ʾʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "Lcd_Driver.h"

/* ��������1.8��TFTҺ����ռ�õ�nRF52832�ܽ���Դ
CS  P0.22 --- LCD_TFTƬѡ
RST P0.23 --- LCD_TFT��λ
AO  P0.24 --- LCD_TFT����/�����л�
SDA P0.25 --- LCD_TFT����
SCK P0.26 --- LCD_TFTʱ��
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
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
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
 	  POINT_COLOR = RED;//��������Ϊ��ɫ 


    while (true)
    {			  
			  LCD_Clear(BLUE);
			  BACK_COLOR=BLUE;
	      POINT_COLOR=RED;   //���û���Ϊ��ɫ 
	      //��ʾ32*32����
	      showhanzi32(0,0,0);	      //��
	      showhanzi32(40,0,1);	    //��
	      showhanzi32(80,0,2);      //ķ
	      //��ʾ16*16����
	      showhanzi16(15,35,0);	    //��
	      showhanzi16(35,35,1);	    //��
	      showhanzi16(55,35,2);	    //ķ
	      showhanzi16(75,35,3);	    //��
	      showhanzi16(95,35,4);	    //��
			  //��ʾ�ַ�
	      LCD_ShowString(25,55,200,16,16,"BLUETOOTH");
			  nrf_delay_ms(3000); 
			  //��ʾͼ��
			  LCD_Clear(WHITE);
        showqq();     
        nrf_delay_ms(3000); 			
    }
}
/********************************************END FILE*******************************************/
