#include <stdio.h>

int main(void) {
	char *input = "+ 1 4";
	char *bp;
	int op1, op2;

	bp = input;
	while(*bp != '\0') {
		if(*bp == '+') {
			bp++;
			while(*bp == ' ')
				bp++;
			op1 = *bp - '0';	
			bp++;
			while(*bp == ' ')
				bp++;
			op2 = *bp - '0';
			bp++;
			printf("%s : %d\n", input, op1 + op2);
		}
	}

	return 0;
}

