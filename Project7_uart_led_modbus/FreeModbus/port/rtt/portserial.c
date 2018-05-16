/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "bsp.h"


#define UART_TX_BUF_SIZE 256    /**< UART TX 缓存大小 */
#define UART_RX_BUF_SIZE 256      /**< UART RX 缓存大小 */
#define RXBUF_LEN  8           //UART接收缓存字节数

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);


/* ----------------------- Start implementation -----------------------------*/

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

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
                       eMBParity eParity)
{
    (void)ucPORT;//不改串口
    (void)ucDataBits;// 不改数据位数
    (void)eParity;//不改校验方式
    uint32_t err_code;

    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,    //关闭流控
        false,
        UART_BAUDRATE_BAUDRATE_Baud9600  //波特率设置为115200bps
    };


    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);



    APP_ERROR_CHECK(err_code);
    // enable RX interrupt
//    NVIC_DisableIRQ(UART0_IRQn);
//    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
//    NVIC_SetPriority(UART0_IRQn, APP_IRQ_PRIORITY_LOW);
//    NVIC_EnableIRQ(UART0_IRQn);

    return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    if (xRxEnable)
    {
        // /* enable RX interrupt */
        // serial->ops->control(serial, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
        // /* switch 485 to receive mode */
        // rt_pin_write(MODBUS_SLAVE_RT_CONTROL_PIN_INDEX, PIN_LOW);
        NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
    }
    else
    {
        // /* switch 485 to transmit mode */
        // rt_pin_write(MODBUS_SLAVE_RT_CONTROL_PIN_INDEX, PIN_HIGH);
        // /* disable RX interrupt */
        // serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void *)RT_DEVICE_FLAG_INT_RX);
        NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Disabled << UART_INTENSET_RXDRDY_Pos;

    }
    if (xTxEnable)
    {
        // /* start serial transmit */
        // rt_event_send(&event_serial, EVENT_SERIAL_TRANS_START);
        NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
    }
    else
    {
        // /* stop serial transmit */
        // rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START,
        //               RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0,
        //               &recved_event);
        NRF_UART0->INTENSET = UART_INTENSET_TXDRDY_Disabled << UART_INTENSET_RXDRDY_Pos;
    }
}

void vMBPortClose(void)
{
    app_uart_close();
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    while (app_uart_put(ucByte) != NRF_SUCCESS);
    // serial->parent.write(&(serial->parent), 0, &ucByte, 1);

    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
    uint8_t tmp;
    while (app_uart_get(&tmp) != NRF_SUCCESS);
    *pucByte = tmp;

    return TRUE;


    // serial->parent.read(&(serial->parent), 0, pucByte, 1);
    // return TRUE;
}

/*
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/*
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}


//#pragma vector= UART1_R_RXNE_vector
//__interrupt void UART1_R_RXNE_IRQHandler(void)
//{

//    if (UART1_SR & (1 << 3))
//    {
//        // UART1_SR&=~(1<<3);
//    }
//    else
//    {
//        prvvUARTRxISR();//接受中断

//        //UART1_SR&=~(1<<5);
//    }

//    return;
//}

////将收到的数据再发送出去
//#pragma vector= UART1_T_TC_vector
//__interrupt void UART1_T_TC_IRQHandler(void)
//{

//    prvvUARTTxReadyISR();//发送完成中断
//    //UART1_SR&=~(1<<6);

//    return;
//}
