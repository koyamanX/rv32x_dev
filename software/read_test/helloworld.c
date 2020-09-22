#include <unistd.h>

int main(void) {
	char buf[256];
	int i = 0;

	i = read(STDIN_FILENO, buf, 10);	
	buf[i] = '\0';

	printf("read count %d, read strings : %s\n", i, buf);


	return 0;

}

