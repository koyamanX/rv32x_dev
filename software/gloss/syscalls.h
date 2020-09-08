#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <unistd.h>

extern ssize_t sys_write(int file, const void *ptr, size_t len);
extern void sys_exit(int status);

#endif
