#include <stdio.h>
#include <string.h>   // memset
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>    // isdigit, isspace, etc.

#include "commands.h"
#include "console.h"

/**************************************************************************/
/*!
    @brief  Attempts to convert the supplied decimal or hexadecimal
            string to the matching 32-bit value.  All hexadecimal values
            must be preceded by either '0x' or '0X' to be properly parsed.

    @param[in]  s
                Input string
    @param[out] result
                Signed 32-bit integer to hold the conversion results

    @section Example

    @code
    char *hex = "0xABCD";
    char *dec = "1234";

    // Convert supplied values to integers
    int32_t hexValue, decValue;
    getNumber (hex, &hexValue);
    getNumber (dec, &decValue);

    @endcode
*/
/**************************************************************************/
bool getNumber (char *s, int32_t *result)
{
	int32_t value;
	bool mustBeHex = false;
	int32_t sgn = 1;
	const unsigned char hexToDec [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255, 255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15};

	if (!s)
		return 0;

	// Check if this is a hexadecimal value
	if ((strlen (s) > 2) && (!strncmp (s, "0x", 2) || !strncmp (s, "0X", 2)))
	{
		mustBeHex = true;
		s += 2;
	}

	// Check for negative sign
	if (!mustBeHex && *s && (*s == '-'))
	{
		sgn = -1;
		s++;
	}

	// Try to convert value
	for (value = 0; *s; s++)
	{
		if (mustBeHex && isxdigit ((uint8_t)*s))
			value = (value << 4) | hexToDec [toupper((uint8_t)*s) - '0'];
		else if (isdigit ((uint8_t)*s))
			value = (value * 10) + ((uint8_t)*s - '0');
		else
		{
			// "Malformed number. Must be decimal number or hex value preceeded by '0x'"
			printf ("%s%s", "Malformed number. Must be decimal number or \
						hex value preceeded by '0x'", CFG_PRINTF_NEWLINE);
			return false;
		}
	}
	// Set number to negative value if required
	if (!mustBeHex)
		value *= sgn;
	*result = value;

	return true;
}

bool getNumberU32 (char *s, uint32_t *result)
{
	int32_t r;
	if (getNumber(s, &r))
	{
		*result = (uint32_t)r;
		return true;
	}
	/* An error occured in getNumber */
	else 
		return false;
}

