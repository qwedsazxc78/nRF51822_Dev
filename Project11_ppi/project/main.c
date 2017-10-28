/****************************************Copyright (c)****************************************************
**
**                                      [����ķ�Ƽ�]
**                                        IIKMSIK
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
** Descriptions:        ppiʵ��
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

/* ��������ָʾ�ƺʹ���ռ�õ�nRF52832�ܽ���Դ
P0.17�����������ָʾ��D1
P0.18�����������ָʾ��D2
P0.19�����������ָʾ��D3
P0.20�����������ָʾ��D4

P0.06��UART_TXD   �����ڷ���
P0.08��UART_RXD   �����ڽ���
P0.07��UART_CTS   : δʹ������
P0.05��UART_RTS   : δʹ������

��Ҫ�̽Ӷ�Ӧ������ñ
*/

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                           /**< UART RX buffer size. */

const nrf_drv_timer_t timer0 = NRF_DRV_TIMER_INSTANCE(0);
const nrf_drv_timer_t timer1 = NRF_DRV_TIMER_INSTANCE(1);
const nrf_drv_timer_t timer2 = NRF_DRV_TIMER_INSTANCE(2);

nrf_ppi_channel_t ppi_channel1, ppi_channel2;

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}
// Timer even handler. Not used since timer is used only for PPI.
void timer_event_handler(nrf_timer_event_t event_type, void * p_context) {}

/** @brief Function for initializing the PPI peripheral.
*/
static void ppi_init(void)
{
    uint32_t err_code = NRF_SUCCESS;

    err_code = nrf_drv_ppi_init();//��ʼ��PPI�����PPIģ���Ƿ��Ѿ�����ʼ������δ��ʼ��������ʼ����־��λ
    APP_ERROR_CHECK(err_code);

    // ���õ�һ��PPIͨ��(��ʵ����ʹ����2��PPIͨ��)�TTIMER1 COMPARE[0]ƥ���¼���Ϊ�¼��˵㴥��TIMER0��������STOP Task(����˵�)
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel1);//����PPIͨ�����ɹ�����ͨ����ppi_channel1ָ�򱻷����ͨ��
    APP_ERROR_CHECK(err_code);
    // ����ͨ�����¼��˵������˵�
    err_code = nrf_drv_ppi_channel_assign(ppi_channel1,
                                          nrf_drv_timer_event_address_get(&timer1, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_timer_task_address_get(&timer0, NRF_TIMER_TASK_STOP));
    APP_ERROR_CHECK(err_code);

    // ���õڶ���PPIͨ��(��ʵ����ʹ����2��PPIͨ��)�TTIMER2 COMPARE[0]ƥ���¼���Ϊ�¼��˵㴥��TIMER0��������START Task(����˵�)
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel2);//����PPIͨ�����ɹ�����ͨ����ppi_channel2ָ�򱻷����ͨ��
    APP_ERROR_CHECK(err_code);
    // ����ͨ�����¼��˵������˵�
    err_code = nrf_drv_ppi_channel_assign(ppi_channel2,
                                          nrf_drv_timer_event_address_get(&timer2, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_timer_task_address_get(&timer0, NRF_TIMER_TASK_START));
    APP_ERROR_CHECK(err_code);

    // ʹ�������Ѿ��������¼��˵������˵��PPIͨ��
    err_code = nrf_drv_ppi_channel_enable(ppi_channel1);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_enable(ppi_channel2);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for Timer 0 initialization, which will be started and stopped by timer1 and timer2 using PPI.
*/
static void timer0_init(void)
{
    ret_code_t err_code = nrf_drv_timer_init(&timer0, NULL, timer_event_handler);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for Timer 1 initialization.
 *  @details Initializes Timer 1 peripheral, creates event and interrupt every 2 seconds,
 *           by configuring CC[0] to timer overflow value, we create events at even number of seconds
 *           for example, events are created at 2,4,6 ... seconds. This event can be used to stop Timer 0
 *           with Timer1->Event_Compare[0] triggering Timer 0 TASK_STOP through PPI.
*/
static void timer1_init(void)
{
    // Configure Timer 1 to overflow every 2 seconds. Check TIMER1 configuration for details
    // The overflow occurs every 0xFFFF/(SysClk/2^PRESCALER).
    // = 65535/31250 = 2.097 sec
    ret_code_t err_code = nrf_drv_timer_init(&timer1, NULL, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(&timer1, NRF_TIMER_CC_CHANNEL0, 0xFFFFUL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
}


/** @brief Function for Timer 2 initialization.
 *  @details Initializes Timer 2 peripheral, creates event and interrupt every 2 seconds
 *           by configuring CC[0] to half of timer overflow value. Events are created at odd number of seconds.
 *           For example, events are created at 1,3,5,... seconds. This event can be used to start Timer 0
 *           with Timer2->Event_Compare[0] triggering Timer 0 TASK_START through PPI.
*/
static void timer2_init(void)
{
    // Generate interrupt/event when half of time before the timer overflows has past, that is at 1,3,5,7... seconds from start.
    // Check TIMER1 configuration for details
    // now the overflow occurs every 0xFFFF/(SysClk/2^PRESCALER)
    // = 65535/31250 = 2.097 sec */
    ret_code_t err_code = nrf_drv_timer_init(&timer2, NULL, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(&timer2, NRF_TIMER_CC_CHANNEL0, 0x7FFFUL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
}

/*******************************************************************************
 * ��  �� : ���ڳ�ʼ����������115200bps�����عر�
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/
void uart_config(void)
{
    uint32_t err_code;

    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,    //�ر�����
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200  //����������Ϊ115200bps
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}
/**********************************************************************************************
 * ��  �� : main����
 * ��  �� : ��
 * ����ֵ : ��
 ***********************************************************************************************/
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);     //��������LEDָʾ�ƵĹܽ�
    LEDS_OFF(LEDS_MASK);           //�ر�����ָʾ��

    timer0_init(); // ��ʼ��Timer0����������
    timer1_init(); // ��ʼ��Timer1������ʱ����ż�������ƥ���¼�
    timer2_init(); // ��ʼ��Timer2������ʱ�������������ƥ���¼�
    ppi_init();    // ��ʼ��PPI


    uart_config(); //��ʼ�����ڡ�������115200�����عر�

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This example does not go to low power mode therefore constant latency is not needed.
    //       However this setting will ensure correct behaviour when routing TIMER events through
    //       PPI (shown in this example) and low power mode simultaneously.
    NRF_POWER->TASKS_CONSTLAT = 1;

    // Start clock.
    nrf_drv_timer_enable(&timer0);//ʹ��Timer0
    nrf_drv_timer_enable(&timer1);//ʹ��Timer1
    nrf_drv_timer_enable(&timer2);//ʹ��Timer2

    while (true)
    {
        printf("Current count: %d\r\n", (int)nrf_drv_timer_capture(&timer0, NRF_TIMER_CC_CHANNEL0)); //��ӡ��Timer0����ֵ

        nrf_drv_timer_increment(&timer0);//������Timer0����ֵ��1

        nrf_delay_ms(100);
        nrf_gpio_pin_toggle(LED_1);//��תָʾ��D1��״̬������ָʾ���������

    }
}
/********************************************END FILE*******************************************/
