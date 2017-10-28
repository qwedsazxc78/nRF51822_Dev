/****************************************Copyright (c)****************************************************
**                                        
**                                      
**                                        
**--------------File Info---------------------------------------------------------------------------------
** File name:			     main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   使用的SDK版本-SDK_11.0.0
**						
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		12864OLED显示实验
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "oled.h"
#include "OLEDFONT.h"


/* 开发板中OLED显示屏占用的nRF52832管脚资源
P0.22：输出：OLED片选
P0.23：输出：OLED命令/数据选择 H=命令 L=数据
P0.24：输出：OLED复位
P0.25：输出：OLED数据
P0.26：输出：OLED时钟

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
    nrf_gpio_cfg_output(OLED_SCL);
    nrf_gpio_cfg_output(OLED_SDA);
	  nrf_gpio_cfg_output(OLED_DC);
	  nrf_gpio_cfg_output(OLED_CS);
	  nrf_gpio_cfg_output(OLED_RST);
	
	  nrf_delay_ms(200); 
    OLED_Init();                      //oled 初始化  
    OLED_Fill(0x00);                  //清屏 
	
    while (true)
    {
        //显示ACSII 使用单个字符显示函数
		    OLED_Fill(0x00);//清屏
        for(i=0; i<16 ;i++)
        {
            OLED_ShowChar(i*8,0,' '+i);
            OLED_ShowChar(i*8,2,' '+i+16);  
            OLED_ShowChar(i*8,4,' '+i+32);
            OLED_ShowChar(i*8,6,' '+i+48);    
        }
        nrf_delay_ms(3000);
        
        //显示数字和字母 使用字符串显示函数
        OLED_Fill(0x00);
        LCD_P8x16Str(0,0,"0123456789");
        LCD_P8x16Str(0,2,"abcdefghijklmnop");
        LCD_P8x16Str(0,4,"ABCDEFGHIJKLMNOP");
        LCD_P8x16Str(0,6,"0123456789");
        nrf_delay_ms(5000); 
        
        //显示汉字
        OLED_Fill(0x00); //清屏
        for(i=0; i<6; i++)
        {
            LCD_P16x16Ch(i*16+16,2,i);    
        }
        for(i=0; i<4; i++)
        {
            LCD_P16x16Ch(i*16+32,4,i+6); 
            LCD_P16x16Ch(i*16+32,6,i+6+4);
        }
        nrf_delay_ms(3000);      
		
		    //显示图形
        OLED_Fill(0x00);     //清屏
        OLED_DrawBMP(0,0,128,8,BMP1);//显示图形
        nrf_delay_ms(3000);  //延时，方便观察OLED显示
        
        OLED_Fill(0x00); //清屏
        OLED_DrawBMP(0,0,128,8,BMP2);
        nrf_delay_ms(3000); 
    }
}
/********************************************END FILE*******************************************/
