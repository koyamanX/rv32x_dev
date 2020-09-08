#include <machine/syscall.h>
#include "syscalls.h"

int syscall(int syscall_id, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
	int ret = -1;

	switch(syscall_id) {
		case SYS_write:
			ret = sys_write(arg0, (void *)arg1, arg2);
		case SYS_exit:
			sys_exit(arg0);
		default:
			sys_write(1, "not implemented\n", 16);
			sys_exit(syscall_id);
			break;
	}
	return ret;
}
