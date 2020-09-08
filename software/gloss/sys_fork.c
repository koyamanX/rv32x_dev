#include <errno.h>

#undef errno
extern int errno;

int _fork(void) {
	errno = EAGAIN;

	return -1;
}
