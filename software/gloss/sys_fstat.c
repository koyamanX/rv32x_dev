#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#undef errno
extern int errno;

int _fstat(int fd, struct stat *statbuf) {

	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO) 
		|| (fd == STDIN_FILENO)) {
		statbuf->st_mode = S_IFCHR;
		return 0;
	}
	errno = EBADF;
	return -1;
}

