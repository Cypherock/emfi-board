#include <stdio.h>
#include <string.h>   // memset
#include <stdint.h>
#include <stdbool.h>
#include "commands.h"
#include <string.h>
#include <board.h>

/**
 * Definitions
******************************************************************************/
void cmd_reset(uint8_t argc, char **argv)
{

}

void cmd_startpwm(uint8_t argc, char **argv)
{

}

void cmd_activate_voltage_generation(uint8_t argc, char **argv)
{
	uint32_t number;

	if (getNumberU32(argv[0], &number))
	{
		if (number == 1)
		{
			sysInfo.high_voltage_enabled = true;
		}
		else
		{
			sysInfo.high_voltage_enabled = false;
		}
	}

}

void cmd_get_adc_raw_value(uint8_t argc, char **argv)
{
	printf("%ld\n", sysInfo.adcDataReadBuffer);
}


void cmd_set_period_and_width(uint8_t argc, char **argv)
{
	uint32_t number;

	if (getNumberU32(argv[0], &number))
	{
		if (number && (number < 1000))
		{
			sysInfo.CH1_DC = number;
		}
	}
}

void cmd_perform_software_shoot(uint8_t argc, char **argv)
{

}


void cmd_get_systick(uint8_t argc, char **argv)
{
	printf("%ld\n", HAL_GetTick());
}

void cmd_get_adc_setpoint(uint8_t argc, char **argv)
{

}

void cmd_set_adc_setpoint(uint8_t argc, char **argv)
{

}

void cmd_get_pwm_value(uint8_t argc, char **argv)
{

}

void cmd_get_pid_values(uint8_t argc, char **argv)
{

}

void cmd_set_pid_values(uint8_t argc, char **argv)
{

}

void cmd_set_pid_limit_values(uint8_t argc, char **argv)
{

}


void cmd_get_pid_limit_values(uint8_t argc, char **argv)
{

}


