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


#ifndef GZLL_PARAMS_H__
#define GZLL_PARAMS_H__

#include "common_gzll_params.h"
 
#define GZLL_DEFAULT_PARAM_DEVICE_MODE 0    // Not relevant for dongle operation
#define GZLL_MAX_CRYPT_PIPES 6

#ifndef GZLL_HOST_ONLY
#define GZLL_HOST_ONLY
#endif

#ifndef GZLL_CRYPT_ENABLE
#define GZLL_CRYPT_ENABLE
#endif

#endif