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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR(void);

static nrf_drv_timer_t timer = NRF_DRV_TIMER_INSTANCE(0);



/* ----------------------- Start implementation -----------------------------*/
void timer_timeout_ind(nrf_timer_event_t event_type, void * p_context)
{
  switch (event_type)
  {
  case NRF_TIMER_EVENT_COMPARE0:
    prvvTIMERExpiredISR();
    break;
  default:
    //Do nothing.
    break;
  }
}

BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
  uint32_t err_code = NRF_SUCCESS;
  uint32_t time_us = 1000;
  uint32_t time_ticks;

  //Configure timer for generating simple light effect - leds on board will invert his state one after the other.
  err_code = nrf_drv_timer_init(&timer, NULL, timer_timeout_ind);
  APP_ERROR_CHECK(err_code);
  time_ticks = nrf_drv_timer_us_to_ticks(&timer, usTim1Timerout50us * time_us);

  nrf_drv_timer_extended_compare(
    &timer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

  // rt_timer_init(&timer, "slave timer",
  //                timer_timeout_ind, /* bind timeout callback function */
  //                RT_NULL,
  //                (50 * usTim1Timerout50us) / (1000 * 1000 / RT_TICK_PER_SECOND) + 1,
  //                RT_TIMER_FLAG_ONE_SHOT); /* one shot */
  return TRUE;
}

void vMBPortTimersEnable()
{
  nrf_drv_timer_enable(&timer);
}

void vMBPortTimersDisable()
{
  nrf_drv_timer_disable(&timer);
}

void prvvTIMERExpiredISR(void)
{
  (void) pxMBPortCBTimerExpired();
}
