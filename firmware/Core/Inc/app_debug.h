/*
 * app_debug.h
 *
 *  Created on: Mar 14, 2016
 *      Author: sushain.razdan
 */

#ifndef __APP_DEBUG_H__
#define __APP_DEBUG_H__


#include "usbd_cdc_if.h"

typedef enum {
	CONN_USB
}ConnType;


void setDebugConnType(ConnType ConnType);
void setDebugOutput(uint8_t dbgVal);
void setDebugClose(void);

#endif
