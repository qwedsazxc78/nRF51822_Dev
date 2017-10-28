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
** Created date:		2016-10-2
** Version:			    1.0
** Descriptions:		12864OLED��ʾʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "oled.h"
#include "OLEDFONT.h"


/* ��������OLED��ʾ��ռ�õ�nRF52832�ܽ���Դ
P0.22�������OLEDƬѡ
P0.23�������OLED����/����ѡ�� H=���� L=����
P0.24�������OLED��λ
P0.25�������OLED����
P0.26�������OLEDʱ��

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
    nrf_gpio_cfg_output(OLED_SCL);
    nrf_gpio_cfg_output(OLED_SDA);
	  nrf_gpio_cfg_output(OLED_DC);
	  nrf_gpio_cfg_output(OLED_CS);
	  nrf_gpio_cfg_output(OLED_RST);
	
	  nrf_delay_ms(200); 
    OLED_Init();                      //oled ��ʼ��  
    OLED_Fill(0x00);                  //���� 
	
    while (true)
    {
        //��ʾACSII ʹ�õ����ַ���ʾ����
		    OLED_Fill(0x00);//����
        for(i=0; i<16 ;i++)
        {
            OLED_ShowChar(i*8,0,' '+i);
            OLED_ShowChar(i*8,2,' '+i+16);  
            OLED_ShowChar(i*8,4,' '+i+32);
            OLED_ShowChar(i*8,6,' '+i+48);    
        }
        nrf_delay_ms(3000);
        
        //��ʾ���ֺ���ĸ ʹ���ַ�����ʾ����
        OLED_Fill(0x00);
        LCD_P8x16Str(0,0,"0123456789");
        LCD_P8x16Str(0,2,"abcdefghijklmnop");
        LCD_P8x16Str(0,4,"ABCDEFGHIJKLMNOP");
        LCD_P8x16Str(0,6,"0123456789");
        nrf_delay_ms(5000); 
        
        //��ʾ����
        OLED_Fill(0x00); //����
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
		
		    //��ʾͼ��
        OLED_Fill(0x00);     //����
        OLED_DrawBMP(0,0,128,8,BMP1);//��ʾͼ��
        nrf_delay_ms(3000);  //��ʱ������۲�OLED��ʾ
        
        OLED_Fill(0x00); //����
        OLED_DrawBMP(0,0,128,8,BMP2);
        nrf_delay_ms(3000); 
    }
}
/********************************************END FILE*******************************************/
