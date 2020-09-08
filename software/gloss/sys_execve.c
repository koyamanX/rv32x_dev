#include <errno.h>

#undef errno
extern int errno;

int _execve(const char *pathname, char * const *argv, char * const *envp) {
	errno = ENOMEM;

	return -1;
}
