#include <machine/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include "uart.h"

ssize_t sys_write(int fd, const void *buf, size_t count);

/* 
	arguments are set by __internal_syscall (newlib risc-v ports)
	a0~a5	-> arguments to system call	(some may not be used by actual system call) 
	a6		-> unused 
	a7		-> system call id 
*/
long syscall(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long unused, long syscall_id) {
	long ret = -1;

	switch(syscall_id) {
		case SYS_write:
			sys_write((int)arg0, (const void *)arg1, (size_t)arg2);
			break;
	}
	
	return ret;
}

#include <errno.h>
#undef errno
extern int errno;

ssize_t sys_write(int fd, const void *buf, size_t count) {
	unsigned char ch;
	int i;
	int ret = -1;

	if(fd != STDOUT_FILENO && fd != STDERR_FILENO) {
		errno = EBADF;
		return ret;
	}
	for(i = 0; i < count; i++) {
		ch = ((unsigned char *)buf)[i];
		uart_putchar(ch);
	}
	ret = i;

	return ret;
}
