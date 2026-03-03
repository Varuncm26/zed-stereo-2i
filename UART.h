#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __cplusplus
	extern "C" {
	#endif

int InitUart(speed_t baud);

#ifdef __cplusplus
}
#endif

#endif
