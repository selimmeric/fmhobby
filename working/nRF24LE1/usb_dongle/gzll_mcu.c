/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision$
 */ 

#include "gzll.h"
#include "cklf.h"

void mcu_init()
{
  RF = 1;                                       // Radio IRQ enable
  CKCON = 0x02;                                 // nRF24LU1p PAN 011 #2
  TICKDV = 0;

  CE_LOW();
  RFCTL = 0x10;                                 // RF SPI Enable 
   
  T2CON = 0x10;                                 // Reload mode 0, osc / 12 
  T2I0 = 1;                                     // Start Timer2
}
/*
void gzll_set_timer_period(uint16_t period)
{
  // Note: The dongle uses timer 2 for Gazell timing during active state
  // and RTC2 during USB suspend.

  T2 = CRC = (uint32_t)0x10000 - (((float)period * 4 / 3 + 0.5)); // Set up period for timer 2
  period = ((float)period * 32000 / 1000000 + 0.5);
  cklf_rtc_init(0x00, period - 1);                                // Set up period for RTC2
}
*/