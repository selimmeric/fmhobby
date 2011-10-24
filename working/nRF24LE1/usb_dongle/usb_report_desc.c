/*深圳云佳科技无线应用，应用无限*/
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

#include "usb_hid_desc.h"
#include "usb_desc.h"

//-----------------------------------------------------------------------------
// Report Descriptor(s) Declarations
//-----------------------------------------------------------------------------

code const uint8_t g_usb_hid_report_1[HID_REPORT_DESCRIPTOR_SIZE_RC] = 
{
   0x05, 0x0C,       //usage page(Consumer Devices)
   0x09, 0x01,       // usage (consumer control)
   0xA1, 0x01,       // collection (application)
      0x85, 0x01,       // reportID (0x01)
      
      0x05, 0x0C,       //usage page(Consumer Devices)
      0x09, 0x01,       // usage (consumer control)
      0xA1, 0x01,       //collection (application)
        0x75, 0x10,       //Report Size (16)
        0x95, 0x01,        //Report Count (1)
        0x15, 0x01,       //Logical minimum (1)
        0x26, 0x9C, 0x02, //Logical maximum (668)
        0x19, 0x01,       //Usage Minimum (Consumer Control)
        0x2A, 0x9C, 0x02, //Usage Maximum (AC Distribute Vertically)
        0x81, 0x00,        //Input (Data,Array,Absolute,Bit Field)
      0xC0,          //End collection 
       
      0x05, 0x01,    //Usage Page (Generic Desktop)
      0x09, 0x80,    //Usage (System Control)
      0xA1, 0x01,    //Collection (Application)
         0x09, 0x82,         //Usage (Power Off)
         0x09, 0x82,         //Usage (Power toggle (standby))
         0x09, 0x83,         //Usage (Power On)
         0x15, 0x00,       //logical minimum (0)
         0x25, 0x01,       //logical maximum (1)
         0x75, 0x01,       //REPORT_SIZE (1)
         0x95, 0x03,       //REPORT_COUNT(3)
         0x81, 0x02,         //input (Data, Variable, Absolute)
         //-----Fill the Byte----//
         0x75, 0x01,         //report size (1 bit)
         0x95, 0x05,         //report count (5)
         0x81, 0x01,         //input (Cnst)
      0xC0,          //End collection 
      
      0x06, 0xBC, 0xFF, //Usage Page(Vendor Defined)
      0x09, 0x88,    //Usage(vendor defined)
      0xA1, 0x01,    //Collection (Application)
        0x75, 0x08,       //Report Size (8)
        0x95, 0x01,        //Report Count (1)
        0x15, 0x01,       //Logical minimum (1)
        0x26, 0xFF, 0x00,       //Logical maximum (255)
        0x19, 0x01,       //Usage Minimum (Vendor Defined)
        0x2A, 0xFF, 0x00,       //Usage Maximum (Vendor Definede)
        0x81, 0x00,       //Input (Data,Array,Absolute,Bit Field)
      0xC0,          //End collection
      
   0xC0,           //End Collectionn

   0x05, 0x01,       //usage page(generic desktop)
   0x09, 0x06,       // usage (keyboard)
   0xA1, 0x01,       //collection (application)
      0x85, 0x02,       // reportID (0x02)
      0x05, 0x07,       //Usage page(keyboard)
      0x19, 0xE0,       //usage_min (224)	
      0x29, 0xE7,       //usage_max (231)	
      0x15, 0x00,       //logical minimum (0)
      0x25, 0x01,       //logical maximum (1)
      0x75, 0x01,       //report size (1)
      0x95, 0x08,       //report count(8)
      0x81, 0x02,       //input (Data, Variable, Absolute)
      0x95, 0x01,       //report count (1)
      0x75, 0x08,       //report size (8)
      0x81, 0x07,       //input (Constant)
      0x95, 0x06,       //report count (6)
      0x75, 0x08,       //report size (8)
      0x15, 0x00,       //logical minimum (0)
      0x25, 0x65,       //logical maximum (101)
      0x05, 0x07,       //usage page (keyboard)
      0x19, 0x00,       //usage minimum (0)
      0x29, 0x65,       //usage maximum (101)
      0x81, 0x00,       //input (Data, Array)
   0xC0              //end collection  (31+21+23+47=126 bytes)
};


code hal_usb_hid_t g_usb_hid_hids[] = 
    {
        { &g_usb_conf_desc.hid1, g_usb_hid_report_1, sizeof(g_usb_hid_report_1) }
    };
