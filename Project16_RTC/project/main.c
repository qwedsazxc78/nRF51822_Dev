/****************************************Copyright (c)****************************************************
**                                        
**                                      [����ķ�Ƽ�]
**                                        IIKMSIK
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
** Descriptions:		��ʱ��ʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf_gpio.h"
#include "bsp.h"
#include "app_error.h"
#include "nrf.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

/* ��������LEDռ�õ�nRF52832�ܽ���Դ
P0.17�����������ָʾ��D1
P0.18�����������ָʾ��D2
P0.19�����������ָʾ��D3
P0.20�����������ָʾ��D4


��Ҫ�̽Ӷ�Ӧ������ñ
*/

#define COMPARE_COUNTERTIME  (3UL)                                        /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */
const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;


const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

/** RTC0�жϴ�����
 * Triggered on TICK and COMPARE0 match.
 */
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        nrf_gpio_pin_toggle(LED_2);  //3���ָʾ��D2���� 
    }
    else if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        nrf_gpio_pin_toggle(LED_1); //ָʾ��D1��8HZ��Ƶ����˸
    }
}

/** ��Ƶʱ�����á�RTCʹ�õ���LFCLK��Ƶʱ��
 */
static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/** @brief Function initialization and configuration of RTC driver instance.
 */
static void rtc_config(void)
{
    uint32_t err_code;

    //��ʼ��RTCʵ������ʼ����ɺ�RTC����power off ״̬
    err_code = nrf_drv_rtc_init(&rtc, NULL, rtc_handler);
    APP_ERROR_CHECK(err_code);

    //ʹ��tick�¼���ʹ��TICK�¼��ж�
    nrf_drv_rtc_tick_enable(&rtc,true);

    //���ñȽ�ͨ�����Ƚ�ֵ���Ƿ�ʹ���ж�
	  //�Ƚ�ͨ��������Ϊͨ��0
	  //�Ƚ�ֵ������Ϊ3��
	  //COUNTER���������õıȽ�ֵ�󣬲����¼��ж�
    err_code = nrf_drv_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME*RTC0_CONFIG_FREQUENCY,true);
    APP_ERROR_CHECK(err_code);

    //����RTC
    nrf_drv_rtc_enable(&rtc);
}

/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);//���ÿ�����������LED�Ĺܽ�Ϊ���
    LEDS_OFF(LEDS_MASK);      //ָʾ�Ƶĳ�ʼ״̬����ΪϨ��
    
    lfclk_config();//LFCLK��Ƶʱ������
    rtc_config();  //RTC����

    while (true)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}
/********************************************END FILE*******************************************/
