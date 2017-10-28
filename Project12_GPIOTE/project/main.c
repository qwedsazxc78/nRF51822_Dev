/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                main.c
** Last modified Date:
** Last Version:
** Descriptions:           ʹ�õ�SDK�汾-SDK_11.0.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:          FiYu
** Created date:        2016-10-2
** Version:             1.0
** Descriptions:        GPIOTEʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"

/* ��������ָʾ��ռ�õ�nRF52832�ܽ���Դ
P0.17�����������ָʾ��D1
P0.18�����������ָʾ��D2
P0.19�����������ָʾ��D3
P0.20�����������ָʾ��D4


��Ҫ�̽Ӷ�Ӧ������ñ
*/

#define GPIO_OUTPUT_PIN_NUMBER BSP_LED_0  // ����ָʾ�������ܽ�

static nrf_drv_timer_t timer = NRF_DRV_TIMER_INSTANCE(0);

void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context) {}


static void led_blinking_setup()
{
    uint32_t compare_evt_addr;
    uint32_t gpiote_task_addr;
    nrf_ppi_channel_t ppi_channel;
    ret_code_t err_code;
    nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

    //��ʼ��GPIOTE����ܽ�
    err_code = nrf_drv_gpiote_out_init(GPIO_OUTPUT_PIN_NUMBER, &config);
    APP_ERROR_CHECK(err_code);


    nrf_drv_timer_extended_compare(&timer, (nrf_timer_cc_channel_t)0, 200 * 1000UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    // ����PPIͨ���TTIMER0 CC[0]ƥ���¼���Ϊ�¼��˵㴥��GPIOTE��Task(����˵�)
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);//����PPIͨ�����ɹ�����ͨ����ppi_channelָ�򱻷����ͨ��
    APP_ERROR_CHECK(err_code);

    compare_evt_addr = nrf_drv_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(GPIO_OUTPUT_PIN_NUMBER);
    // ����ͨ�����¼��˵������˵�
    err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);
    APP_ERROR_CHECK(err_code);

    // ʹ���Ѿ��������¼��˵������˵��PPIͨ��
    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_task_enable(GPIO_OUTPUT_PIN_NUMBER);
}


/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/
int main(void)
{
    ret_code_t err_code;

    //��ʼ��PPI�����PPIģ���Ƿ��Ѿ�����ʼ������δ��ʼ��������ʼ����־��λ
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    //��ʼ��GPIOTEģ�顣
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    //��ʼ����ʱ����ע�������ò����¼������������Զ����timer_dummy_handler�Ǹ��պ���
    err_code = nrf_drv_timer_init(&timer, NULL, timer_dummy_handler);
    APP_ERROR_CHECK(err_code);


    // ����PPIͨ������ʱ��ƥ���¼���Ϊ�¼��˵�EEP��GPIOTE ��TASK(����GPIO��ת)��Ϊ����˵�TEP.
    led_blinking_setup();

    // ʹ�ܶ�ʱ��
    nrf_drv_timer_enable(&timer);

    while (true)
    {

    }
}
/********************************************END FILE*******************************************/
