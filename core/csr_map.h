#ifndef CSR_MAP_H
#define CSR_MAP_H

/* CSR address mapping */


/* User Mode CSRs */
/* User Trap Setup */
#define USTATUS			0x000
#define UIE				0x004
#define UTVEC			0x005
/* User Trap Handling */
#define USCRATCH		0x040
#define UEPC			0x041
#define UCAUSE			0x042
#define UTVAL			0x043
#define UIP				0x044
/* User Floating-Point CSRs */
#define FFLAGS			0x001
#define FRM				0x002
#define FCSR			0x003
/* User Counter/Timers */
#define CYCLE           0xc00
#define TIME            0xc01
#define INSTRET         0xc02
#define CYCLEH          0xc80
#define TIMEH           0xc81
#define INSTRETH        0xc82
#define HPMCOUNTER3     0xc03
#define HPMCOUNTER3H    0xc83
#define HPMCOUNTER4     0xc04
#define HPMCOUNTER4H    0xc84
#define HPMCOUNTER5     0xc05
#define HPMCOUNTER5H    0xc85
#define HPMCOUNTER6     0xc06
#define HPMCOUNTER6H    0xc86
#define HPMCOUNTER7     0xc07
#define HPMCOUNTER7H    0xc87
#define HPMCOUNTER8     0xc08
#define HPMCOUNTER8H    0xc88
#define HPMCOUNTER9     0xc09
#define HPMCOUNTER9H    0xc89
#define HPMCOUNTER10    0xc0a
#define HPMCOUNTER10H   0xc8a
#define HPMCOUNTER11    0xc0b
#define HPMCOUNTER11H   0xc8b
#define HPMCOUNTER12    0xc0c
#define HPMCOUNTER12H   0xc8c
#define HPMCOUNTER13    0xc0d
#define HPMCOUNTER13H   0xc8d
#define HPMCOUNTER14    0xc0e
#define HPMCOUNTER14H   0xc8e
#define HPMCOUNTER15    0xc0f
#define HPMCOUNTER15H   0xc8f
#define HPMCOUNTER16    0xc10
#define HPMCOUNTER16H   0xc90
#define HPMCOUNTER17    0xc11
#define HPMCOUNTER17H   0xc91
#define HPMCOUNTER18    0xc12
#define HPMCOUNTER18H   0xc92
#define HPMCOUNTER19    0xc13
#define HPMCOUNTER19H   0xc93
#define HPMCOUNTER20    0xc14
#define HPMCOUNTER20H   0xc94
#define HPMCOUNTER21    0xc15
#define HPMCOUNTER21H   0xc95
#define HPMCOUNTER22    0xc16
#define HPMCOUNTER22H   0xc96
#define HPMCOUNTER23    0xc17
#define HPMCOUNTER23H   0xc97
#define HPMCOUNTER24    0xc18
#define HPMCOUNTER24H   0xc98
#define HPMCOUNTER25    0xc19
#define HPMCOUNTER25H   0xc99
#define HPMCOUNTER26    0xc1a
#define HPMCOUNTER26H   0xc9a
#define HPMCOUNTER27    0xc1b
#define HPMCOUNTER27H   0xc9b
#define HPMCOUNTER28    0xc1c
#define HPMCOUNTER28H   0xc9c
#define HPMCOUNTER29    0xc1d
#define HPMCOUNTER29H   0xc9d
#define HPMCOUNTER30    0xc1e
#define HPMCOUNTER30H   0xc9e
#define HPMCOUNTER31    0xc1f
#define HPMCOUNTER31H   0xc9f
/* Supervisor Mode CSRs */
/* Supervisor Trap Setup */
#define SSTATUS			0x100
#define SEDELEG			0x102
#define SIDELEG			0x103
#define SIE				0x104
#define STVEC			0x105
#define SCOUNTEREN		0x106
/* Supervisor Trap Handling */
#define SSCRATCH		0x140
#define SEPC			0x141
#define SCAUSE			0x142
#define STVAL			0x143
#define SIP				0x144
/* Supervisor Protection and Translation */
#define SATP			0x180

/* Hypervisor Mode CSRs */

/* Machine Mode CSRs */
#define MVENDORID       0xf11
#define MARCHID         0xf12
#define MIMPID          0xf13
#define MHARTID         0xf14
/* Machine Trap Setup */
#define MSTATUS         0x300
#define MISA            0x301
#define MEDELEG			0x302
#define MIDELEG			0x303
#define MIE				0x304
#define MTVEC			0x305
#define MCOUNTEREN		0x306
#define MSTATUSH        0x310
/* Machine Trap Handling */
#define MSCRATCH		0x340
#define MEPC			0x341
#define MCAUSE			0x342
#define MTVAL			0x343
#define MIP				0x344
#define MTINST			0x34a
#define MTVAL2			0x34b
/* Machine Memory Protection */

/* Machine Counter/Timers */
#define MCYCLE          0xb00
#define MCYCLEH         0xb80
#define MINSTRET		0xb02
#define MINSTRETH		0xb82
#define MHPMCOUNTER3	0xb03
#define MHPMCOUNTER3H	0xb83
#define MHPMCOUNTER4	0xb04
#define MHPMCOUNTER4H	0xb84
#define MHPMCOUNTER5	0xb05
#define MHPMCOUNTER5H	0xb85
#define MHPMCOUNTER6	0xb06
#define MHPMCOUNTER6H	0xb86
#define MHPMCOUNTER7	0xb07
#define MHPMCOUNTER7H	0xb87
#define MHPMCOUNTER8	0xb08
#define MHPMCOUNTER8H	0xb88
#define MHPMCOUNTER9	0xb09
#define MHPMCOUNTER9H	0xb89
#define MHPMCOUNTER10	0xb0a
#define MHPMCOUNTER10H	0xb8a
#define MHPMCOUNTER11	0xb0b 
#define MHPMCOUNTER11H	0xb8b
#define MHPMCOUNTER12	0xb0c
#define MHPMCOUNTER12H	0xb8c
#define MHPMCOUNTER13	0xb0d
#define MHPMCOUNTER13H	0xb8d
#define MHPMCOUNTER14	0xb0e
#define MHPMCOUNTER14H	0xb8e
#define MHPMCOUNTER15	0xb0f
#define MHPMCOUNTER15H	0xb8f
#define MHPMCOUNTER16	0xb10
#define MHPMCOUNTER16H	0xb90
#define MHPMCOUNTER17	0xb11
#define MHPMCOUNTER17H	0xb91
#define MHPMCOUNTER18	0xb12
#define MHPMCOUNTER18H	0xb92
#define MHPMCOUNTER19	0xb13
#define MHPMCOUNTER19H	0xb93
#define MHPMCOUNTER20	0xb14
#define MHPMCOUNTER20H	0xb94
#define MHPMCOUNTER21	0xb15
#define MHPMCOUNTER21H	0xb95
#define MHPMCOUNTER22	0xb16
#define MHPMCOUNTER22H	0xb96
#define MHPMCOUNTER23	0xb17
#define MHPMCOUNTER23H	0xb97
#define MHPMCOUNTER24	0xb18
#define MHPMCOUNTER24H	0xb98
#define MHPMCOUNTER25	0xb19
#define MHPMCOUNTER25H	0xb99
#define MHPMCOUNTER26	0xb1a
#define MHPMCOUNTER26H	0xb9a
#define MHPMCOUNTER27	0xb1b
#define MHPMCOUNTER27H	0xb9b
#define MHPMCOUNTER28	0xb1c
#define MHPMCOUNTER28H	0xb9c
#define MHPMCOUNTER29	0xb1d
#define MHPMCOUNTER29H	0xb9d
#define MHPMCOUNTER30	0xb1e
#define MHPMCOUNTER30H	0xb9e
#define MHPMCOUNTER31	0xb1f
#define MHPMCOUNTER31H	0xb9f
/* Machine Counter Setup */
#define MCOUNTINHIBIT	0x320
#define MHPMEVENT3		0x323
#define MHPMEVENT4		0x324
#define MHPMEVENT5		0x325
#define MHPMEVENT6		0x326
#define MHPMEVENT7		0x327
#define MHPMEVENT8		0x328
#define MHPMEVENT9		0x329
#define MHPMEVENT10		0x32a
#define MHPMEVENT11		0x32b
#define MHPMEVENT12		0x32c
#define MHPMEVENT13		0x32d
#define MHPMEVENT14		0x32e
#define MHPMEVENT15		0x32f
#define MHPMEVENT16		0x330
#define MHPMEVENT17		0x331
#define MHPMEVENT18		0x332
#define MHPMEVENT19		0x333
#define MHPMEVENT20		0x334
#define MHPMEVENT21		0x335
#define MHPMEVENT22		0x336
#define MHPMEVENT23		0x337
#define MHPMEVENT24		0x338
#define MHPMEVENT25		0x339
#define MHPMEVENT26		0x33a
#define MHPMEVENT27		0x33b
#define MHPMEVENT28		0x33c
#define MHPMEVENT29		0x33d
#define MHPMEVENT30		0x33e
#define MHPMEVENT31		0x33f

/* Debug Mode CSRs */

#endif
