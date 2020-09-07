#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include "uart.h"

#undef errno
extern int errno;

ssize_t _write(int file, const void *ptr, size_t len) {
	int i;
	unsigned char *p;

	if((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
		errno = EBADF;
		return -1;
	}

	p = (unsigned char *)ptr;
	for(i = 0; i < len; i++) {
		uart_putchar(*p++);
	}

	return len;
}
