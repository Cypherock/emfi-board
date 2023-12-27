#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <stdio.h>
#include <stdint.h>
/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE 250

/* DEL acts as a backspace. */
#define cmdASCII_DEL (0x7F)
#define KEY_CODE_ESC (27)    /* Escape key code */
#define BUF_SIZE 2
#define CFG_INTERFACE
#define CFG_INTERFACE_MAXMSGSIZE    (256)
#define CFG_INTERFACE_SILENTMODE    (0)
#define CFG_INTERFACE_DROPCR        (0)
#define CFG_INTERFACE_SHORTERRORS   (0)
#define CFG_INTERFACE_CONFIRMREADY  (0)
#define CFG_INTERFACE_LONGSYSINFO   (1)

//#define KEY_CODE_ESC        (27)    /* Escape key code */
#define KEY_CODE_ENTER      (13)    /* Enter key code  */

#define CFG_PRINTF_LEFT           "^[D"
#define CFG_PRINTF_RIGHT          "^[C"
#define CFG_PRINTF_BACKSPACE      "\a"
#define CFG_PRINTF_NEWLINE        "\r\n"
#define CFG_INTERFACE_PROMPT      "EMFI_CypheRock>"

typedef struct
{
	char *command;
	uint8_t minArgs;
	uint8_t maxArgs;
	uint8_t hidden;
	void (*func)(uint8_t argc, char **argv);
	const char *description;
	const char *parameters;
} cli_t;

//#define TELNET_IDLE_TIME 3000
void Send_startup_massage(void);
void Console_serial(uint8_t index);
void xSerialGetChar(void);
void Console_clearBuffers(void);

void telnet_Initialise(void);
void telnet_polling(void);
void switch_to_console(void);
#endif //_CONSOLE_H_

