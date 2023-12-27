#ifndef __CONSOLE_INTERFACE_H__
#define __CONSOLE_INTERFACE_H__
#include <stdint.h>
#include "usbd_cdc_if.h"
 
typedef enum {
	COMM_OK,
	COMM_BUSY,
	COMM_ERR
}comStatus_t;

void consoleSendUsbDataCmd(uint8_t *buf, uint32_t Length);

#endif
