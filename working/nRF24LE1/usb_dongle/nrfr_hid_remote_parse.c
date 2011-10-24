/* Copyright (c) 2007 Nordic Semiconductor. All Rights Reserved.
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

#include <stdint.h>
#include <stdbool.h>
#include "nrfr_hid_remote_parse.h"

//-----------------------------------------------------------------------------
// Remote packet definition
//-----------------------------------------------------------------------------

static bool generate_HID_report_from_RC_input(uint8_t *indata,uint8_t input_number_of_keys, uint8_t *report1_buf, uint8_t *report2_buf);
static bool resolve_keys_from_matrix(uint8_t *input_key_matrix, uint8_t *keys, uint8_t *number_of_keys);
static uint8_t read_rows(void);

//This table shows the mapping between the key matrix
//and rc_to_hid_table
static const uint8_t code matrix_lookup[] = {		\
14, 20, 6, 11, 5, \                      
10, 12, 13, 15, 16, \
56, 57, 58, 59, 60, \
61, 62, 63, 64, 65, \
72, 67, 71, 70, 73, \
1, 22, 2, 0xFF, 0,};

void nrfr_hid_remote_parse(uint8_t *indata, uint8_t input_number_of_keys, uint8_t *report1_buf, uint8_t *report2_buf)
{
  extern code uint16_t nrfr_remote_HID_table[];
  uint8_t i; 
  uint8_t key_count = 0;
  bool fn_key_is_set = false;
  //bool retval=false;
   
  for(i = 0; i < input_number_of_keys; i++)
  {
    *(indata + i) = matrix_lookup[*(indata + i)];  
  }
            
  for(i = 0; i < NRFR_LENGTH_REPORT_1; i++) //Reset the report id struct
//	             NRFR_LENGTH_REPORT_1 5
  {
    report1_buf[i] = 0;
  }
  report1_buf[0] = NRFR_REMOTE_CONTROL_BUTTONS_REPORT_ID;     //Report ID 1
  
  for(i = 0; i < NRFR_LENGTH_REPORT_2; i++)
//				 NRFR_LENGTH_REPORT_2 9
  {
    report2_buf[i] = 0;
  }
  report2_buf[0] = NRFR_REMOTE_KEYBOARD_BUTTONS_REPORT_ID;     //Report ID 2
  
  //Iterate through the bytes in *indata
  for(i = 0; i < input_number_of_keys; i++) 
  {
                         //The 55 first entries in the rc_to_HID_table represent mappings
                         //to the first HID report  Each entry consists of two numbers in the rc_to_HID_table. The first number
                         //represent the type and which byte in the HID report to change, and the second number 
                         //is the new usage value.
    if(*indata < 56)
    {
      switch(nrfr_remote_HID_table[((*indata)*2)])
      {
        case CONSUMER_CONTROL:
          report1_buf[CONSUMER_CONTROL]=  (uint8_t)(nrfr_remote_HID_table[((*indata)*2) + 1]);   
          report1_buf[CONSUMER_CONTROL+1]=  (uint8_t)(nrfr_remote_HID_table[((*indata)*2) + 1] >> 8);
          break;
       
        case SYSTEM_CONTROL:
          report1_buf[SYSTEM_CONTROL]=  (uint8_t)(nrfr_remote_HID_table[((*indata)*2) + 1]);
          break;
        
        case VENDOR_DEFINED:
          report1_buf[VENDOR_DEFINED]=  (uint8_t)(nrfr_remote_HID_table[((*indata)*2) + 1]);
          break;

        default:
          break;
      }      
    }
                         //The entries from 56 to 73 in the rc_to_HID_table represent mappings
                         //to the second HID report. Each entry consists of two bytes in the rc_to_HID_table. The first byte 
                         //represent the value of byte 1 in the HID report, the second byte represents
                         //the value of the third byte in the HID report
    else if((*indata > 55) && (*indata < 74))
    {
      report2_buf[NRFR_REMOTE_KEYBOARD_BUTTONS_MOD]=nrfr_remote_HID_table[(*indata)*2];
      report2_buf[NRFR_REMOTE_KEYBOARD_BUTTONS_KEYS]=nrfr_remote_HID_table[((*indata)*2)+1];             
    }
    indata++;
  }
}