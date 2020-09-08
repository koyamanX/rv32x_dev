#include <errno.h>
#include <stdint.h>

#undef errno
extern int errno;

extern char _heap_start;
extern char _heap_end;

void * _sbrk(intptr_t increment) {
	static char *heap = &_heap_start;

	if(&_heap_start == &_heap_end) {
		return (void *) -1;
	}

	if((heap + increment) < &_heap_end) {
		heap += increment;
	} else {
		heap = &_heap_end;
		return (void *) -1;
	}

	return (void *)(heap - increment);
}
