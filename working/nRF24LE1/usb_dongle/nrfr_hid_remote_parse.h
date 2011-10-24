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

#ifndef NRFR_HID_REMOTE_PARSE_H
#define NRFR_HID_REMOTE_PARSE_H

#define NRFR_REMOTE_CONTROL_BUTTONS_REPORT_ID 1
#define NRFR_REMOTE_KEYBOARD_BUTTONS_REPORT_ID 2

// Definition USB HID report 1 packet
#define CONSUMER_CONTROL    1
#define SYSTEM_CONTROL      3
#define VENDOR_DEFINED      4
#define NRFR_LENGTH_REPORT_1 (VENDOR_DEFINED + 1)

// Definition USB HID report 2 packet
#define NRFR_REMOTE_KEYBOARD_BUTTONS_MOD 1
#define NRFR_REMOTE_KEYBOARD_BUTTONS_RESERVED (NRFR_REMOTE_KEYBOARD_BUTTONS_MOD + 1)
#define NRFR_REMOTE_KEYBOARD_BUTTONS_KEYS (NRFR_REMOTE_KEYBOARD_BUTTONS_RESERVED + 1)
#define NRFR_LENGTH_REPORT_2 (NRFR_REMOTE_KEYBOARD_BUTTONS_KEYS + 6)

/**                                                             
  Function for translating the button matrix data received from the nRFready basic remote control into
  HID USB reports.

  @param *indata is a pointer to an array holding the active remote control buttons.
  @param input_number_of_keys is the size of the *indata array.
  @param *report1_buf is the destination for the USB HID report ID 1 output packet.
  @param *report2_buf is the destination for the USB HID report ID 2 output packet.   
*/
void nrfr_hid_remote_parse(uint8_t *indata, uint8_t input_number_of_keys, uint8_t *report1_buf, uint8_t *report2_buf);

#endif