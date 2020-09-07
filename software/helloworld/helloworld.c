#include <unistd.h>

int main(void) {
	write(STDOUT_FILENO, "hello,world!\n", 13);

	return 0;
}

