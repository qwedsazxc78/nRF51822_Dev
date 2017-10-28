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
** Descriptions:		���Ź�ʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "nrf.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_drv_wdt.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

/* ��������ָʾ�ƺͰ���ռ�õ�nRF52832�ܽ���Դ
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
#define APP_TIMER_PRESCALER     0                           /**< RTC1��Ƶϵ�� */
#define APP_TIMER_OP_QUEUE_SIZE 2                           /**< Size of timer operation queues. */
#define FEED_BUTTON_ID          0                           /**< Button for feeding the dog. */

nrf_drv_wdt_channel_id m_channel_id;

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    LEDS_OFF(LEDS_MASK);
    
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}

/**
 * @brief Assert callback.
 *
 * @param[in] id    Fault identifier. See @ref NRF_FAULT_IDS.
 * @param[in] pc    The program counter of the instruction that triggered the fault, or 0 if
 *                  unavailable.
 * @param[in] info  Optional additional information regarding the fault. Refer to each fault
 *                  identifier for details.
 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    LEDS_OFF(LEDS_MASK);
    while(1);
}

/**
 * @brief BSP events callback.
 */
void bsp_event_callback(bsp_event_t event)
{
    switch(event)
    {
        case BSP_EVENT_KEY_0:
            nrf_drv_wdt_channel_feed(m_channel_id);
            break;
        
        default :
            //Do nothing.
            break;
    }
}

/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/ 
int main(void)
{
    uint32_t err_code = NRF_SUCCESS;
    
    //BSP configuration for button support: button pushing will feed the dog.
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_BUTTONS, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), bsp_event_callback);
    APP_ERROR_CHECK(err_code);
    
    //���ÿ������ϵ�4���û�LEDָʾ��
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    
    //4��ָʾ��������˸һ�Σ�ָʾϵͳ����
    for(uint32_t i = 0; i < LEDS_NUMBER; i++)
    {   nrf_delay_ms(200);
        LEDS_ON(BSP_LED_0_MASK << i);
    }
     err_code = bsp_buttons_enable();
     APP_ERROR_CHECK(err_code);
    
    //����WDT.
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;//ʹ��Ĭ�ϲ������ÿ��Ź�����CPU˯��ʱ�����Ź��������У����Ź���ʱ����2��
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);//��鷵��ֵ
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();

    while(1)
    {
        __SEV();  //�����¼�
        __WFE();  //����˯�ߣ��ȴ��¼�����
        __WFE();
    }
}
/********************************************END FILE*******************************************/
