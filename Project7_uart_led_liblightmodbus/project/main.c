/****************************************Copyright (c)****************************************************
**
**                                      [°¬¿ËÄ·¿Æ¼¼]
**                                        IIKMSIK
**--------------File Info---------------------------------------------------------------------------------
** File name:                main.c
** Last modified Date:
** Last Version:
** Descriptions:           Ê¹ÓÃµÄSDK°æ±¾-SDK_12.1.0
**
**--------------------------------------------------------------------------------------------------------
** Created by:          FiYu
** Created date:        2016-10-1
** Version:             1.0
** Descriptions:        UART¿ØÖÆÖ¸Ê¾µÆÊµÑé
**--------------------------------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"


#include <stdio.h>
#include <inttypes.h>

#include <lightmodbus/core.h>
#include <lightmodbus/master.h>
#include <lightmodbus/slave.h>

//Configuration structures

////Registers and coils
//uint8_t coils[2] = { 0 };
//uint16_t regs[45] = { 0x10 };

//ModbusSlave sstatus =
//{
//    .address = 1,
//    .registers = regs,
//    .registerCount = sizeof(regs),
//    .coils = coils,
//    .coilCount = sizeof(coils),
//};



//For storing exit codes
uint8_t sec, mec;


void masterdump(void);
void slavedump(void);
void slaveResponse(void);


#define UART_TX_BUF_SIZE 256    /**< UART TX »º´æ´óÐ¡ */
#define UART_RX_BUF_SIZE 64      /**< UART RX »º´æ´óÐ¡ */
#define RXBUF_LEN  16           //UART½ÓÊÕ»º´æ×Ö½ÚÊý

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
/*******************************************************************************
 * Ãè  Êö : ´®¿Ú³õÊ¼»¯¡£²¨ÌØÂÊ115200bps£¬Á÷¿Ø¹Ø±Õ
 * ²Î  Êý : ÎÞ
 * ·µ»ØÖµ : ÎÞ
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
        APP_UART_FLOW_CONTROL_DISABLED,    //¹Ø±ÕÁ÷¿Ø
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200  //²¨ÌØÂÊÉèÖÃÎª115200bps
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}
/*******************************************************************************
 * Ãè  Êö : mainº¯Êý
 * ²Î  Êý : ÎÞ
 * ·µ»ØÖµ : ÎÞ
 ******************************************************************************/
int main(void)
{
    uint8_t RxCnt = 0;             //UART½ÓÊÕ×Ö½ÚÊý
    uint8_t UartRxBuf[RXBUF_LEN] = {0x00};  //UART½ÓÊÕ»º´æ

    LEDS_CONFIGURE(LEDS_MASK);     //ÅäÖÃÇý¶¯LEDÖ¸Ê¾µÆµÄ¹Ü½Å
    LEDS_OFF(LEDS_MASK);           //¹Ø±ÕËùÓÐÖ¸Ê¾µÆ

    uart_config(); //´®¿Ú³õÊ¼»¯

    //Init slave (input registers and discrete inputs work just the same)
    modbusSlaveInit( &sstatus );

    // //Pretend frame is being sent to slave
    // sstatus.request.frame = UartRxBuf;
    // sstatus.request.length = RxCnt;

    //Let slave parse frame
    sec = modbusParseRequest( &sstatus );


    slavedump( );
    uint8_t frame_start = false;

    while (true)
    {
        uint8_t cr;
        int i;
        while (app_uart_get(&cr) != NRF_SUCCESS);

        if (cr == sstatus.address)
        {
            frame_start = true;
            UartRxBuf[RxCnt] = cr;
        }

        if (frame_start == true)
        {
            UartRxBuf[RxCnt++] = cr;


            if (RxCnt >= 8)
            {
//                for ( i = 0; i < RxCnt; i++ )
//                    app_uart_put(UartRxBuf[i]);

                switch (UartRxBuf[1])
                {
                case (0x03):
                    sstatus.request.frame = UartRxBuf;
                    sstatus.request.length = RxCnt;
                    sec = modbusParseRequest( &sstatus );
                    nrf_gpio_pin_clear(LED_1);
                    slaveResponse();
                    break;
                case (0x06):
                    sstatus.request.frame = UartRxBuf;
                    sstatus.request.length = RxCnt;
                    sec = modbusParseRequest( &sstatus );
                    nrf_gpio_pin_clear(LED_1);
                    slaveResponse();
                    break;
                case (0x01):
                    sstatus.request.frame = UartRxBuf;
                    sstatus.request.length = RxCnt;
                    sec = modbusParseRequest( &sstatus );
                    LEDS_OFF(LEDS_MASK);
                    nrf_gpio_pin_clear(LED_2);
                    slaveResponse();
                case (0x05):
                    sstatus.request.frame = UartRxBuf;
                    sstatus.request.length = RxCnt;
                    sec = modbusParseRequest( &sstatus );
                    LEDS_OFF(LEDS_MASK);
                    nrf_gpio_pin_clear(LED_2);
                    slaveResponse();


                    break;
                default:
                    break;
                }

                RxCnt = 0;
                frame_start = false;
            }
        }
        // else
        // {
        //     if (RxCnt >= RXBUF_LEN)
        //     {
        //         RxCnt = 0;
        //     }
        //     if ((UartRxBuf[0] == sstatus.address))
        //     {
        //         switch (UartRxBuf[1])
        //         {
        //         case 3:
        //             sstatus.request.frame = UartRxBuf;
        //             sstatus.request.length = RxCnt;
        //             sec = modbusParseRequest( &sstatus );
        //             LEDS_OFF(LEDS_MASK);
        //             nrf_gpio_pin_clear(LED_1);
        //             slaveResponse();
        //             RxCnt = 0;
        //             break;
        //         case 6:
        //             sstatus.request.frame = UartRxBuf;
        //             sstatus.request.length = RxCnt;
        //             sec = modbusParseRequest( &sstatus );
        //             LEDS_OFF(LEDS_MASK);
        //             nrf_gpio_pin_clear(LED_2);
        //             slaveResponse();
        //             RxCnt = 0;
        //             break;
        //         default:
        //             break;
        //         }
        //     }
        // }



    }


    // /* WRITE VALUE */

    // //Build frame to write single register
    // modbusBuildRequest06( &mstatus, 27, 3, 56 );

    // //Pretend frame is being sent to slave
    // sstatus.request.frame = UartRxBuf;
    // sstatus.request.length = RxCnt;

    // //Let slave parse frame
    // sec = modbusParseRequest( &sstatus );

    // //Pretend frame is being sent to master
    // mstatus.response.frame = sstatus.response.frame;
    // mstatus.response.length = sstatus.response.length;

    // mec = modbusParseResponse( &mstatus );

    // //Dump status again
    // slavedump( );
    // masterdump( );

    // /* READ VALUE */

    // //Build frame to read 4 registers
    // modbusBuildRequest03( &mstatus, 27, 0, 4 );

    // //Pretend frame is being sent to slave
    // sstatus.request.frame = mstatus.request.frame;
    // sstatus.request.length = mstatus.request.length;

    // //Let slave parse frame
    // sec = modbusParseRequest( &sstatus );

    // //Pretend frame is being sent to master
    // mstatus.response.frame = sstatus.response.frame;
    // mstatus.response.length = sstatus.response.length;

    // mec = modbusParseResponse( &mstatus );

    // //Dump status again
    // slavedump( );
    // masterdump( );

}
/********************************************END FILE*******************************************/

void slaveResponse( )
{
    int i;
    for ( i = 0; i < sstatus.response.length; i++ )
        app_uart_put(sstatus.response.frame[i] );
}
//Dump slave information
void slavedump( )
{
    int i;
    printf( "==SLAVE DUMP==\n" );

    printf( "Registers:" );
    for ( i = 0; i < sstatus.registerCount; i++ )
        printf( " %d", sstatus.registers[i] );
    printf( "\r\n" );

    printf( "Coils:" );
    for ( i = 0; i < sstatus.coilCount >> 3; i++ )
        printf( " %d", sstatus.coils[i] );
    printf( "\r\n" );

    printf( "Request:" );
    for ( i = 0; i < sstatus.request.length; i++ )
        printf( " %d", sstatus.request.frame[i] );
    printf( "\r\n" );

    printf( "Response:" );
    for ( i = 0; i < sstatus.response.length; i++ )
        printf( " %d", sstatus.response.frame[i] );
    printf( "\r\n" );

    printf( "Exit code: %d\r\n", sec );
}

//Dump master information
void masterdump( )
{
    int i;
    printf( "==MASTER DUMP==\n" );

    printf( "Received data: slave: %d, addr: %d, count: %d, type: %d\n",
            mstatus.data.address, mstatus.data.index, mstatus.data.count, mstatus.data.type );
    printf( "\t\tvalues:" );
    switch ( mstatus.data.type )
    {
    case MODBUS_HOLDING_REGISTER:
    case MODBUS_INPUT_REGISTER:
        for ( i = 0; i < mstatus.data.count; i++ )
            printf( " %d", mstatus.data.regs[i] );
        break;

    case MODBUS_COIL:
    case MODBUS_DISCRETE_INPUT:
        for ( i = 0; i < mstatus.data.count; i++ )
            printf( " %d", modbusMaskRead( mstatus.data.coils, mstatus.data.length, i ) );
        break;
    }
    printf( "\r\n" );

    printf( "Request:" );
    for ( i = 0; i < mstatus.request.length; i++ )
        printf( " %d", mstatus.request.frame[i] );
    printf( "\r\n" );

    printf( "Response:" );
    for ( i = 0; i < mstatus.response.length; i++ )
        printf( " %d", mstatus.response.frame[i] );
    printf( "\r\n" );

    printf( "Exit code: %d\r\n", mec );
}
