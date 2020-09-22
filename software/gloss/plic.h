#ifndef PLIC_H
#define PLIC_H

#define PLIC_PRIORITY_REGISTER1 ((volatile unsigned int *) 0x0c000004)
#define PLIC_PRIORITY_REGISTER2 ((volatile unsigned int *) 0x0c000008)
#define PLIC_PRIORITY_REGISTER3	((volatile unsigned int *) 0x0c00000c)
#define PLIC_ENABLE_REGISTER	((volatile unsigned int *) 0x0c002000)
#define PLIC_THRESHOLD_REGISTER ((volatile unsigned int *) 0x0c200000)
#define PLIC_CLAIM_REGISTER		((volatile unsigned int *) 0x0c200004)


#endif
