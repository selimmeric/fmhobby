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
    
/** @file
 * @brief The conversion table between remote control messages and HID reports is located here
 * 
 */

#include <stdint.h>
#include "nrfr_hid_remote_parse.h"

/**
* A table showing the byte number and usage value in the HID report for a given input.
* Indexes higher than 56*2 are reserved for numpad inputs since they
* use a different HID report.
*/

code uint16_t nrfr_remote_HID_table[]={
CONSUMER_CONTROL, 0x0000, //Not used
CONSUMER_CONTROL, 0x0224, //1=Back
CONSUMER_CONTROL, 0x0209, //2=More Info
CONSUMER_CONTROL, 0x00B0, //3=Play
CONSUMER_CONTROL, 0x00B1, //4=Pause
CONSUMER_CONTROL, 0x00CD, //5=Play/Pause
CONSUMER_CONTROL, 0x00B7, //6=Stop
CONSUMER_CONTROL, 0x00B2, //7=Record
CONSUMER_CONTROL, 0x00B3, //8=Fast Forward
CONSUMER_CONTROL, 0x00B4, //9=Rewind
CONSUMER_CONTROL, 0x00B5, //10=Skip Forward
CONSUMER_CONTROL, 0x00B6, //11=Skip Back
CONSUMER_CONTROL, 0x00E9, //12=Volume Up
CONSUMER_CONTROL, 0x00EA, //13=Volume Down
CONSUMER_CONTROL, 0x00E2, //14=Mute
CONSUMER_CONTROL, 0x009C, //15=Chan/Page up
CONSUMER_CONTROL, 0x009D, //16=Chan/Page down
CONSUMER_CONTROL, 0x008D, //17=Guide
CONSUMER_CONTROL, 0x0208, //18=Print
/////////////////
SYSTEM_CONTROL, 0x0001,   //19=Power Off
SYSTEM_CONTROL, 0x0002,   //20=Power Toggle (Standby)
SYSTEM_CONTROL, 0x0004,   //21=Power On
/////////////////
VENDOR_DEFINED, 0x000D,   //22=Green Start Button
VENDOR_DEFINED, 0x002B,   //23=Close Captioning
VENDOR_DEFINED, 0x005A,   //24=Teletext On/Of
VENDOR_DEFINED, 0x005B,   //25=Teletext Red
VENDOR_DEFINED, 0x005C,   //26=Teletext Green
VENDOR_DEFINED, 0x005D,   //27=Teletext Yellow
VENDOR_DEFINED, 0x005E,   //28=Teletext Blue
VENDOR_DEFINED, 0x0025,   //29=Live TV
VENDOR_DEFINED, 0x0047,   //30=Music
VENDOR_DEFINED, 0x0048,   //31=Recorded TV
VENDOR_DEFINED, 0x0049,   //32=Pictures
VENDOR_DEFINED, 0x004A,   //33=Videos
VENDOR_DEFINED, 0x0050,   //34=FM Radio
VENDOR_DEFINED, 0x003C,   //35=OnSpot 
VENDOR_DEFINED, 0x003D,   //36=OnSpot App
VENDOR_DEFINED, 0x0024,   //37=DVD Menu 
VENDOR_DEFINED, 0x004B,   //38=DVD Angle
VENDOR_DEFINED, 0x004C,   //39=DVD Audio
VENDOR_DEFINED, 0x004D,   //40=DVD Subtitle
VENDOR_DEFINED, 0x0028,   //41=Eject
VENDOR_DEFINED, 0x0043,   //42=DVD top menu
VENDOR_DEFINED, 0x0032,   //43=Ext0
VENDOR_DEFINED, 0x0033,   //44=Ext1
VENDOR_DEFINED, 0x0034,   //45=Ext2
VENDOR_DEFINED, 0x0035,   //46=Ext3
VENDOR_DEFINED, 0x0036,   //47=Ext4
VENDOR_DEFINED, 0x0037,   //48=Ext5
VENDOR_DEFINED, 0x0038,   //49=Ext6
VENDOR_DEFINED, 0x0039,   //50=Ext7
VENDOR_DEFINED, 0x003A,   //51=Ext8
VENDOR_DEFINED, 0x0080,   //52=Ext9
VENDOR_DEFINED, 0x0081,   //53=Ext10
VENDOR_DEFINED, 0x006F,   //54=Ext11
VENDOR_DEFINED, 0x0027,   //55=Zoom
//------------------------------//
// Keyboard Scancodes from here //
//------------------------------//
0x00,0x1E, //56="1"
0x00,0x1F, //57="2"
0x00,0x20, //58="3"
0x00,0x21, //59="4"
0x00,0x22, //60="5"
0x00,0x23, //61="6"
0x00,0x24, //62="7"
0x00,0x25, //63="8"
0x00,0x26, //64="9"
0x00,0x27, //65="0"
0x00,0x29, //66=Clear
0x00,0x28, //67=Enter
0x02,0x20, //68=" # "
0x02,0x25, //69=" * "
0x00,0x52, //70=Up
0x00,0x51, //71=Down
0x00,0x50, //72=Left
0x00,0x4F, //73=Right
};

