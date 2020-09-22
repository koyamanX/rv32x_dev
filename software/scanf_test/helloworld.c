#include <stdio.h>

int main(void) {
	int v;

	setvbuf(stdin, NULL, _IONBF, 0);
	scanf("%d", &v);
	printf("num read : %d\n", v);

	return 0;
}

