#include <unistd.h>
#include <errno.h>

#undef errno
extern int errno;

int _isatty(int fd) {
	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)
		|| (fd == STDIN_FILENO)) {
		return 1;
	}
	
	errno = EBADF;
	return -1;
}
