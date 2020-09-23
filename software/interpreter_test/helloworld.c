#include <stdio.h>

int main(void) {
	int ch;
	int op = 'x';
	int opr1 = 0, opr2 = 0;
	int acc = 0;

	setvbuf(stdin, NULL, _IONBF, 0);

	while(1) {
		printf(">>> ");
		while(((ch = getchar()) != -1) && (ch != ')')) {
			while(ch == ' ')
				ch = getchar();
			if(ch == '(') {
				ch = getchar();
				while(ch == ' ')
					ch = getchar();
				op = ch;
				ch = getchar();
				while(ch == ' ')
					ch = getchar();
				opr1 = ch - '0';
				ch = getchar();
				while(ch == ' ')
					ch = getchar();
				opr2 = ch - '0';
				ch = getchar();
				while(ch == ' ')
					ch = getchar();
				ungetc(ch, stdin);
				switch(op) {
					case '+':
						acc = opr1 + opr2;
						break;
					case '*':
						acc = opr1 * opr2;
						break;
					case '-':
						acc = opr1 - opr2;
						break;
					case '/':
						acc = opr1 / opr2;
						break;
				}
			} else if(isdigit(ch)) {
				opr1 = acc;
				opr2 = ch - '0';
				ch = getchar();
				while(ch == ' ')
					ch = getchar();
				ungetc(ch, stdin);
				switch(op) {
					case '+':
						acc = opr1 + opr2;
						break;
					case '*':
						acc = opr1 * opr2;
						break;
					case '-':
						acc = opr1 - opr2;
						break;
					case '/':
						acc = opr1 / opr2;
						break;
				}
			}
		}
		printf("acc %d\n", acc);
	}

	return 0;
}

