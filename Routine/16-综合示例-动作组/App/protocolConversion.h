#ifndef _PROTOCOL_CONVERSION_H_
#define _PROTOCOL_CONVERSION_H_

#include <stdint.h>

#define BUS_SERVO_MAX_BIN   256
#define BUS_SERVO_MAX_ASC   128

#define BUS_SERVO_ERR_FRAME     -1
#define BUS_SERVO_ERR_LONG      -2
#define BUS_SERVO_ERR_PARSE     -3
#define BUS_SERVO_ERR_RANGE     -4
#define BUS_SERVO_ERR_UNSUPPORT -99

int  busServo_asciiToBin(const char *asc, uint8_t *bin);
void user_parse_ascii_command(const char *cmd);

#endif
