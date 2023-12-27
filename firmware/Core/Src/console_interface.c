/*
 * console_interface.c
 *
 *  Created on: Dec 23, 2016
 *      Author:
 */
#include "console_interface.h"
#include <string.h>
#include "console.h"
void consoleSendUsbDataCmd(uint8_t *buf, uint32_t Length)
{

	//memcpy(charEvt.rxChar, buf, Length);
	for (int i = 0; i < Length; i++)
	{
		Console_serial(buf[i]);
	}


}
