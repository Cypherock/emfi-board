/*
 * app_debug.c
 *
 *  Created on: Mar 14, 2016
 *      Author: sushain.razdan
 */

#include "app_debug.h"

static ConnType debugConn;
void setDebugConnType(ConnType connVal)
{
	debugConn = connVal;
}

int stdout_putchar (int ch)
{
	uint8_t txStr[2];
	uint8_t txSize = 1;
	
	if (ch == '\n') {
		txStr[0] = '\r';
		txStr[1] = '\n';
		txSize = 2;
	} else
		txStr[0] = (uint8_t)ch;
	//if (debugConn == CONN_USB)
	{
		CDC_Write_FS(txStr, txSize);
	}
	return 1;
}
int ttywrch(int ch)
{
	return 1;
}
