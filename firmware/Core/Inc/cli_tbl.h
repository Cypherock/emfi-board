/**************************************************************************/
/*!
    @file     cli_tbl.h
    @author   K. Townsend (microBuilder.eu)

    @brief    Command lookup table for the CLI
    @ingroup  CLI

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, K. Townsend
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#ifndef __CLI_TBL_H__
#define __CLI_TBL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_COUNT (sizeof(cli_tbl)/sizeof(cli_t))

#include <stdio.h>
#include <stdint.h>
#include "console.h"
    /* Function prototypes for the command table */
void cmd_help(uint8_t argc, char **argv);         /* handled by cli/cli.c */
void cmd_reset(uint8_t argc, char **argv);
void cmd_startpwm(uint8_t argc, char **argv);
void cmd_activate_voltage_generation(uint8_t argc, char **argv);
void cmd_get_adc_raw_value(uint8_t argc, char **argv);
void cmd_set_period_and_width(uint8_t argc, char **argv);
void cmd_perform_software_shoot(uint8_t argc, char **argv);
void cmd_get_systick(uint8_t argc, char **argv);
void cmd_get_adc_setpoint(uint8_t argc, char **argv);
void cmd_set_adc_setpoint(uint8_t argc, char **argv);
void cmd_get_pwm_value(uint8_t argc, char **argv);
void cmd_get_pid_values(uint8_t argc, char **argv);
void cmd_set_pid_values(uint8_t argc, char **argv);
void cmd_set_pid_limit_values(uint8_t argc, char **argv);
void cmd_get_pid_limit_values(uint8_t argc, char **argv);

#define CMD_NOPARAMS ( "This command has no parameters" )

    /**************************************************************************/
    /*!
        Command list for the command-line interpreter and the name of the
        corresponding method that handles the command.

        Note that a trailing ',' is required on the last entry, which will
        cause a NULL entry to be appended to the end of the table.
    */
    /**************************************************************************/
    cli_t cli_tbl[] =
    {
        // command name,          min args, max args, hidden, function name, command description, syntax
        { "?",                  0, 0, 0,  cmd_help ,                       "Help", CMD_NOPARAMS },
		{ "startpwm",       1, 1, 1, cmd_startpwm,        "startpwm <val>"},
		{ "activateVolt",       1, 1, 1, cmd_activate_voltage_generation,        "activateVolt <val>"},
		{ "getadcraw",       0, 0, 0, cmd_get_adc_raw_value,        "getadcraw",CMD_NOPARAMS},
		{ "setpwmconfig",       2, 2, 0, cmd_set_period_and_width,        "setpwmconfig <val>"},
		{ "setshoot",       0, 0, 0, cmd_perform_software_shoot,        "setshoot <val>",CMD_NOPARAMS},
		{ "gettick",       0, 0, 0, cmd_get_systick,        "gettick",CMD_NOPARAMS},
		{ "getsetpoint",        0, 0, 0, cmd_get_adc_setpoint,        "getsetpoint",CMD_NOPARAMS},
		{ "setsetpoint",       1, 1, 1, cmd_set_adc_setpoint,        "setsetpoint <val>"},
		{ "getpwmvalue",        0, 0, 0, cmd_get_pwm_value,        "getpwmvalue ",CMD_NOPARAMS},
		{ "getpidvalue",        0, 0, 0, cmd_get_pid_values,        "getpidvalue",CMD_NOPARAMS},
		{ "setpidvalue",       3, 3, 0, cmd_set_pid_values,        "setpidvalue <val>"},
		{ "setpidlimitvalue",       3, 3, 0, cmd_set_pid_limit_values,        "setpidlimitvalue <val>"},
		{ "getpidlim",       0, 0, 0, cmd_get_pid_limit_values,        "getpidlim",CMD_NOPARAMS},
    };

#ifdef __cplusplus
}
#endif

#endif
