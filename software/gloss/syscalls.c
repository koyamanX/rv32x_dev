#include <machine/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "uart.h"

ssize_t sys_write(int fd, const void *buf, size_t count);
void sys_exit(int status);
int sys_access(const char *pathname, int mode);
int sys_close(int fd);
int sys_faccessat(int dirfd, const char *pathname, int mode, int flags);
int sys_fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);
int sys_fstat(int fd, struct stat *statbuf);
int sys_link(const char *oldpath, const char *newpath);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_lstat(const char *pathname, struct stat *statbuf);

char *__env[1] = {0};
char **environ = __env;

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
			ret = sys_write((int)arg0, (const void *)arg1, (size_t)arg2);
			break;
		case SYS_access:
			ret = sys_access((const char *)arg0, (int)arg1);
			break;
		case SYS_close:
			ret = sys_close((int) arg0);
			break;
		case SYS_faccessat:
			ret = sys_faccessat((int)arg0, (const char *)arg1, (int)arg2, (int)arg3);
			break;
		case SYS_fstatat:
			ret = sys_fstatat((int)arg0, (const char *)arg1, (struct stat *)arg2, (int)arg3);
			break;
		case SYS_fstat:
			ret = sys_fstat((int)arg0, (struct stat *)arg1);
			break;
		case SYS_link:
			ret = sys_link((const char *)arg0, (const char *)arg1);
			break;
		case SYS_lseek:
			ret = sys_lseek((int)arg0, (off_t)arg1, (int)arg2);
			break;
		case SYS_lstat:
			ret = sys_lstat((const char *)arg0, (struct stat *)arg1);
			break;
		case SYS_exit: /* no-break */
		default:
			sys_exit((int)arg0);
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

	if((fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
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
void sys_exit(int status) {
_sys_exit:
	goto _sys_exit;
}
int sys_access(const char *pathname, int mode) {
	int ret = -1;
	errno = ENOENT;

	return ret;
}
int sys_close(int fd) {
	int ret = -1;
	errno = EBADF;

	return ret;
}
int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	int ret = -1;
	errno = ENOENT;

	return ret;
}
int sys_fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) {
	int ret = -1;
	errno = ENOENT;

	return ret;
}
int sys_fstat(int fd, struct stat *statbuf) {
	int ret = -1;

	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
		statbuf->st_mode = S_IFCHR;
		ret = 0;
	} else {
		errno = EBADF;
	}

	return ret;
}
int sys_link(const char *oldpath, const char *newpath) {
	int ret = -1;
	errno = EMLINK;

	return ret;
}
off_t sys_lseek(int fd, off_t offset, int whence) {
	int ret = -1;

	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
		ret = 0;
	} else {
		errno = EBADF;
	}

	return (off_t)ret;
}
int sys_lstat(const char *pathname, struct stat *statbuf) {
	int ret = -1;
	errno = EACCES;

	return ret;
}
