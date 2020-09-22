#include <stdlib.h>
#include "plic.h"

#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT        1
#define ILLEGAL_INSTRUCTION             2
#define BREAKPOINT                      3
#define LOAD_ADDRESS_MISALIGNED         4
#define LOAD_ACCESS_FAULT               5
#define STORE_AMO_ADDRESS_MISALIGNED    6
#define STORE_AMO_ACCESS_FAULT          7
#define ENVIRONMENT_CALL_FROM_U_MODE    8
#define ENVIRONMENT_CALL_FROM_S_MODE    9
#define ENVIRONMENT_CALL_FROM_M_MODE    11
#define INSTRUCTION_PAGE_FAULT          12
#define LOAD_PAGE_FAULT                 13
#define STORE_AMO_PAGE_FAULT            15

#define UART_RX_CLAIM			1
#define UART_TX_CLAIM			2


__attribute__ ((interrupt ("machine"))) void machine_software_interrupt_handler(void) {
	return ;
}
__attribute__ ((interrupt ("machine"))) void machine_timer_interrupt_handler(void) {
	return ;
}

extern void uart_rx_interrupt_handler(void);
extern void uart_tx_interrupt_handler(void);
__attribute__ ((interrupt ("machine"))) void machine_external_interrupt_handler(void) {
	unsigned int claim;
	claim = *PLIC_CLAIM_REGISTER;
	
	switch(claim) {
		case UART_RX_CLAIM:
			uart_rx_interrupt_handler();
			break;
		case UART_TX_CLAIM:
			uart_tx_interrupt_handler();
			break;
		default:
			exit(1);
			break;
	}

	*PLIC_CLAIM_REGISTER = claim;
	return ;
}
