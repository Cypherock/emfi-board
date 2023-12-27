/*
 * console.c
 *
 *  Created on: 20 march , 2018
 *      Author:
 */

#include "string.h"
#include "stdio.h"
#include "console.h"
#include "cli_tbl.h"
#include "usbd_cdc_if.h"
#include "stdbool.h"
#include "board.h"
/* Const messages output by the command console. */
static const char * const welcomeStr = "Welcome to EMFI console.\r\nType ? to view a list of registered \
	commands.\r\nYou can enter maximum of 250 characters at a time\r\n[Press ~ to execute the previous command \
again]\r\n%s>";
const char * const outputMessageStr = "%s>";
static char pcWelcomeMessage[200];
static char pcEndOfOutputMessage[20];
uint8_t rxCircularBuff[BUF_SIZE];
uint8_t const *rxTailPtr;
int8_t cRxedChar;
char cInputString[cmdMAX_INPUT_SIZE];
char cLastInputString[cmdMAX_INPUT_SIZE];
void cliRx(uint8_t c);

uint8_t ucInputIndex;
char *pcOutputString;
static char cli_buffer[CFG_INTERFACE_MAXMSGSIZE];
static char *cli_buffer_ptr;

static void xSerialPutChar(const uint8_t *txData, uint16_t length);
static void vSerialPutString(const int8_t *message, uint16_t length);

void cliInit(void);


bool netwActive;
bool sockInit;
int sock;

uint8_t consoleBuf[BUF_SIZE];
uint8_t Rxed = 0;

uint8_t connection_type = 0;
#define MODEL_NAME		"EMFI_CypheRock>"
void Send_startup_massage(void)
{
	sprintf(pcWelcomeMessage, welcomeStr, MODEL_NAME);
	sprintf(pcEndOfOutputMessage, outputMessageStr, MODEL_NAME);
	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console interface
	will be used at any one time. */

	/* Send the welcome message. */
	vSerialPutString((int8_t *)pcWelcomeMessage, (uint16_t)strlen(pcWelcomeMessage));

	cliInit();
}

void Console_clearBuffers()
{
	ucInputIndex = 0;
	memset(cInputString, 0, cmdMAX_INPUT_SIZE);
	memset(cLastInputString, 0, cmdMAX_INPUT_SIZE);
}	

uint8_t isCmdEnable = 0;
uint8_t dataBuffer[128];
uint8_t dataCounter = 0;
uint8_t dataReadCounter = 0;
uint8_t dataStarted = 0;
uint8_t dataPacketSize = 0;

void Console_serial(uint8_t  charData)
{
	if (isCmdEnable)
	{
		cliRx(charData);

	}
	else
	{
		if ((charData == 0xAA) && (dataStarted == 0))
		{
			dataStarted = 1;
			dataReadCounter++;
		}
		else if ((dataStarted == 1) && (dataPacketSize == 0))
		{
			dataPacketSize = charData;
			dataReadCounter++;
		}
		else if ((dataStarted == 1) && (dataPacketSize))
		{
			dataBuffer[dataCounter++] = charData;
			dataReadCounter++;
		}


		if ((dataCounter) && (dataStarted) && (dataReadCounter == (dataPacketSize)))
		{
			uint8_t cmd = dataBuffer[0];

			dataStarted = 0;
			dataReadCounter = 0;
			dataPacketSize = 0;
			parse_cmd(cmd, (uint8_t *)&dataBuffer[1],dataCounter);
			dataCounter = 0;
		}

	}

}
/*-----------------------------------------------------------*/

static void xSerialPutChar(const uint8_t *txData, uint16_t length)
{
	CDC_Write_FS((uint8_t *)txData, length);
}
/*-----------------------------------------------------------*/

static void vSerialPutString(const int8_t *message, uint16_t length)
{
	xSerialPutChar((uint8_t *)message, length);
}

static void cliMenu()
{
	vSerialPutString((int8_t *)CFG_INTERFACE_PROMPT,strlen(CFG_INTERFACE_PROMPT));
}

void cliParse(char *cmd)
{
	size_t argc, i = 0;
	char *argv[30];

	argv[i] = strtok(cmd, " ");
	do
	{
		argv[++i] = strtok(NULL, " ");
	} while ((i < 30) && (argv[i] != NULL));

	argc = i;
	for (i=0; i < CMD_COUNT; i++)
	{
		if (!strcmp(argv[0], cli_tbl[i].command))
		{
			if ((argc == 2) && !strcmp (argv [1], "?"))
			{
				xSerialPutChar((uint8_t *)cli_tbl[i].description,strlen(cli_tbl[i].description));
				xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));
				xSerialPutChar((uint8_t *)cli_tbl[i].parameters,strlen(cli_tbl[i].parameters));
				xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));
			}
			else if ((argc - 1) < cli_tbl[i].minArgs)
			{
				// Too few arguments supplied
				xSerialPutChar((uint8_t *)"Too Many Arguments",strlen("Too Many Arguments"));
				xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));
			}
			else if ((argc - 1) > cli_tbl[i].maxArgs)
			{
				// Too many arguments supplied
				xSerialPutChar((uint8_t *)"Too Many Arguments",strlen("Too Many Arguments"));
				xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));

			}
			else
			{
				// Dispatch command to the appropriate function
				cli_tbl[i].func(argc - 1, &argv [1]);
			}

			return;
		}
	}
	// Command not recognized
	xSerialPutChar((uint8_t *)"Command not recognized",strlen("Command not recognized"));
	xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));


	cliMenu();
}


/**************************************************************************/
/*!
    @brief Initialises the command line using the appropriate interface
 */
/**************************************************************************/
void cliInit(void)
{
	// init the cli_buffer ptr
	cli_buffer_ptr = cli_buffer;
}

/**************************************************************************/
/*!
    'help' command handler
 */
/**************************************************************************/
void cmd_help(uint8_t argc, char **argv)
{
	size_t i;

	xSerialPutChar((uint8_t *)"Command ",strlen("Command"));
	xSerialPutChar((uint8_t *)"\t",strlen("\t"));
	xSerialPutChar((uint8_t *)"Description",strlen("Description"));
	xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));
	xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));


	// Display full command list
	for (i=0; i < CMD_COUNT; i++)
	{
		//if (!cli_tbl[i].hidden)
		{
			xSerialPutChar((uint8_t *)cli_tbl[i].command,strlen(cli_tbl[i].command));
			xSerialPutChar((uint8_t *)"\t",strlen("\t"));
			xSerialPutChar((uint8_t *)cli_tbl[i].description,strlen(cli_tbl[i].description));
			xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));
			//printf ("%-10s   %s%s", cli_tbl[i].command, cli_tbl[i].description, CFG_PRINTF_NEWLINE);
		}
	}
	xSerialPutChar((uint8_t *)"Command parameters can be seen by entering: <command-name> ?",strlen("Command parameters can be seen by entering: <command-name> ?"));
	xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));

}

void cli_tab_auto_complete(char* cmd_buffer, uint32_t* cmd_pos, bool print_options) {
	const char* matched_command = NULL;
	uint32_t module_index;
	uint32_t num_of_matched_commands = 0;
	uint32_t common_command_len = 0;

	for (module_index = 0; module_index < CMD_COUNT; module_index++) {
		// for (command_index = 0; command_index < CMD_COUNT; command_index++) {
		const cli_t *command = &cli_tbl[module_index];
		if (strncmp(cmd_buffer, command->command, *cmd_pos) == 0) {
			/* Found a new command which matches the string */
			if (num_of_matched_commands == 0) {
				/*
				 * Save the first match for later tab completion in case
				 * print_option is true (double tab)
				 */
				matched_command = command->command;
				common_command_len = strlen(matched_command);
			} else {
				/*
				 * More then one match
				 * in case of print_option we need to print all options
				 */
				if (print_options) {
					/*
					 * Because we want to print help options only when we
					 * have multiple matches, print also the first one.
					 */
					if (num_of_matched_commands == 1) {
						xSerialPutChar((uint8_t *)matched_command,strlen(matched_command));
					}
					xSerialPutChar((uint8_t *)command->command,strlen(command->command));
				}

				/*
				 * Find the common prefix of all the matched commands for
				 * partial completion
				 */
				int last_common_command_len = common_command_len;
				common_command_len = 0;
				while (matched_command[common_command_len] == command->command[common_command_len] &&
						matched_command[common_command_len] != '\0' &&
						command->command[common_command_len] != '\0' &&
						common_command_len < last_common_command_len) {
					common_command_len++;
				}
			}
			num_of_matched_commands++;
		}
		//}
	}

	/* Do the full/partial tab completion */
	if (matched_command != NULL) {
		strncpy(cmd_buffer, matched_command, common_command_len);
		*cmd_pos = strlen(cmd_buffer);
		cmd_buffer[*cmd_pos] = '\0';
		xSerialPutChar((uint8_t *)cmd_buffer,strlen(cmd_buffer));
	}
}
/*=============================================================*/
#ifndef CLI_CMD_HISTORY
#define CLI_CMD_HISTORY             10
#endif
#ifndef CLI_MAX_CMD_LENGTH
#define CLI_MAX_CMD_LENGTH          256
#endif
static char cmd_history_buffer[CLI_CMD_HISTORY][CLI_MAX_CMD_LENGTH];
static uint32_t cmd_history_pos;
static uint32_t cmd_history_full;
static char cmd_buffer[CLI_MAX_CMD_LENGTH];
static uint32_t cmd_pos;

static void clear_cmd_buffer( void ) {
	memset(cmd_buffer, 0x0, sizeof(cmd_buffer));
	cmd_pos = 0;
}
static void store_command_to_history( void ) {
	uint32_t hist_count;
	if (strcmp(cmd_history_buffer[0], cmd_buffer)) {
		for (hist_count = CLI_CMD_HISTORY-1; hist_count > 0; hist_count--) {
			memcpy(cmd_history_buffer[hist_count], cmd_history_buffer[hist_count-1], CLI_MAX_CMD_LENGTH);
		}
		cmd_history_full++;
		if (cmd_history_full > CLI_CMD_HISTORY) {
			cmd_history_full = CLI_CMD_HISTORY;
		}
		memcpy(cmd_history_buffer[0], cmd_buffer, CLI_MAX_CMD_LENGTH);
		cmd_history_buffer[0][CLI_MAX_CMD_LENGTH-1] = '\0';
	}
}
static bool cli_special_key_check(char ch) {
	static uint32_t key_sequence;
	static char last_ch;
	bool special_key_found = false;

	if (key_sequence == 0 && ch == 27) {
		special_key_found = true;
		key_sequence = 1;
	} else if (key_sequence == 1 && (ch == '[' || ch == 'O')) {
		special_key_found = true;
		key_sequence = 2;
	} else if (key_sequence == 2 && ch >= 'A' && ch <= 'D') {
		special_key_found = true;
		key_sequence = 0;
		switch (ch) {
		case 'A':                           /* Up */
			if (cmd_history_pos < cmd_history_full) {
				/* Clear the line */
				memset(cmd_buffer, ' ', cmd_pos);
				xSerialPutChar((uint8_t *)cmd_buffer,strlen(cmd_buffer));
				strcpy(cmd_buffer, cmd_history_buffer[cmd_history_pos]);
				cmd_pos = strlen(cmd_buffer);
				xSerialPutChar((uint8_t *)cmd_buffer,strlen(cmd_buffer));
				cmd_history_pos++;
			} else {
				xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
			}
			break;
		case 'B':                           /* Down */
			if (cmd_history_pos) {
				/* Clear the line */
				memset(cmd_buffer, ' ', cmd_pos);

				xSerialPutChar((uint8_t *)cmd_buffer,strlen(cmd_buffer));
				if (--cmd_history_pos != 0) {
					strcpy(cmd_buffer, cmd_history_buffer[cmd_history_pos]);
					cmd_pos = strlen(cmd_buffer);
					xSerialPutChar((uint8_t *)cmd_buffer,strlen(cmd_buffer));
				} else {
					clear_cmd_buffer();
				}
			} else {
				xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
			}

			break;
		case 'C':                           /* Right */
			/* TODO not finnished
			 * because "^[C" doesn't move the cursor,
			 * don't know why
			 */
			if (cmd_pos < strlen(cmd_buffer)) {
				cmd_pos++;
				xSerialPutChar((uint8_t *)CFG_PRINTF_RIGHT,strlen(CFG_PRINTF_RIGHT));
			} else {
				xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
			}
			break;
		case 'D':                           /* Left */
			/* TODO not finnished
			 * because "^[D" doesn't move the cursor,
			 * don't know why
			 */
			if (cmd_pos > 0) {
				cmd_pos--;
				xSerialPutChar((uint8_t *)CFG_PRINTF_LEFT,strlen(CFG_PRINTF_LEFT));
				// printf("^[D");
			} else {
				xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
				// printf("\a");
			}
			break;
		}
	} else if (key_sequence == 2 && (ch == 'F')) {
		/* End*/
		/* TODO: for now just return invalid key */
		xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
	} else if (key_sequence == 2 && (ch == '1' || ch == '3')) {
		/* Home or Delete, we need to check one more character */
		special_key_found = true;
		key_sequence = 3;
	} else if (key_sequence == 3) {
		/* TODO Home and Delete: for now just return invalid key */
		xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
		special_key_found = true;
	} else {
		/* Unknown sequence */
		key_sequence = 0;
	}

	/* Store the last character */
	last_ch = ch;

	(void)last_ch;                              /* Prevent compiler warnings */

	return special_key_found;
}

/*-----------------------------------------------------------*/
void cliRx(uint8_t c)
{
	static char last_ch;

	if (!cli_special_key_check(c)) {
		/* Parse the characters only if they are not part of the special key sequence */
		switch (c) {
		/* Backspace */
		case '\b':
		case 127:
			if (cmd_pos != 0) {
				cmd_buffer[--cmd_pos] = '\0';
				xSerialPutChar(&c,1);
			} else {
				xSerialPutChar((uint8_t *)CFG_PRINTF_BACKSPACE,strlen(CFG_PRINTF_BACKSPACE));
			}
			// TODO
			break;
			/* Tab for autocomplete */
		case '\t':
			cli_tab_auto_complete(cmd_buffer, &cmd_pos, (last_ch == '\t'));
			break;
			/* New line -> new command */
		case '\n':
		case '\r':
			cmd_history_pos = 0;
			if (strlen(cmd_buffer) == 0) {
				clear_cmd_buffer();
				xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));
				xSerialPutChar((uint8_t *)CFG_INTERFACE_PROMPT,strlen(CFG_INTERFACE_PROMPT));
				return;
			}
			xSerialPutChar((uint8_t *)CFG_PRINTF_NEWLINE,strlen(CFG_PRINTF_NEWLINE));

			store_command_to_history();
			cliParse(cmd_buffer);
			clear_cmd_buffer();

			break;
			/* All other chars */
		default:
			if (cmd_pos < CLI_MAX_CMD_LENGTH) {
				cmd_buffer[cmd_pos++] = c;
			} else {
				clear_cmd_buffer();
				printf("\r\n\aERR: Command too long\r\n" CFG_INTERFACE_PROMPT);
				return;
			}
			xSerialPutChar(&c,1);
		}
	}

	/* Store last character for double tab detection */
	last_ch = c;
}

/*-----------------------------------------------------------*/
