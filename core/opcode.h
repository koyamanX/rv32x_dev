#ifndef OPCODE_H
#define OPCODE_H

#include "priv.h"

/* base opcode map (riscv-spec p.129) */
#define LOAD        7'b0000011
#define STORE       7'b0100011
#define MADD        7'b1000011
#define BRANCH      7'b1100011
#define LOAD_FP     7'b0000111
#define STORE_FP    7'b0100111
#define MSUB        7'b1000111
#define JALR        7'b1100111
#define CUSTOM_0    7'b0001011
#define CUSTOM_1    7'b0101011
#define NMSUB       7'b1001011
#define MISC_MEM    7'b0001111
#define AMO         7'b0101111
#define NMADD       7'b1001111
#define JAL         7'b1101111
#define OP_IMM      7'b0010011
#define OP          7'b0110011
#define OP_FP       7'b1010011
#define SYSTEM      7'b1110011
#define AUIPC       7'b0010111
#define LUI         7'b0110111
#define OP_32        7'b0111011
#define OP_IMM_32   7'b0011011

/* funct3 field */
#define ALU_ADD		3'b000
#define ALU_SUB		3'b000
#define ALU_SLL		3'b001
#define ALU_SLT		3'b010
#define ALU_SLTU    3'b011
#define ALU_XOR		3'b100
#define ALU_SRL		3'b101
#define ALU_SRA		3'b101
#define ALU_OR		3'b110
#define ALU_AND		3'b111

#define CSRRW       3'b001
#define CSRRS       3'b010
#define CSRRC       3'b011
#define CSRRWI      3'b101
#define CSRRSI      3'b110
#define CSRRCI      3'b111
#define PRIV		3'b000
#define FENCEI		3'b001
#define FENCE 		3'b000

#define LR_W		5'b00010
#define SC_W		5'b00011
#define AMOSWAP_W	5'b00001
#define AMOADD_W	5'b00000
#define AMOXOR_W	5'b00100
#define AMOAND_W	5'b01100
#define AMOOR_W		5'b01000
#define AMOMIN_W	5'b10000
#define AMOMAX_W	5'b10100
#define AMOMINU_W	5'b11000
#define AMOMAXU_W	5'b11100

/* funct3 M Standard Extension */
#define ALU_MUL		3'b000
#define ALU_MULH	3'b001
#define ALU_MULHSU	3'b010
#define ALU_MULHU	3'b011
#define ALU_DIV		3'b100
#define ALU_DIVU	3'b101
#define ALU_REM		3'b110
#define ALU_REMU	3'b111

/* funct3 condition code */
#define CC_BEQ      3'b000
#define CC_BNE      3'b001
#define CC_BLT      3'b100
#define CC_BGE      3'b101
#define CC_BLTU     3'b110
#define CC_BGEU     3'b111

/* funct3 C Extension */
#define C_ADDI4SPN  3'b000
#define C_LW        3'b010
#define C_SW        3'b110
#define C_ADDI      3'b000
#define C_JAL       3'b001
#define C_LI        3'b010
#define C_LUI       3'b011
#define C_ADDI16SP  3'b011
#define C_J         3'b101
#define C_BEQZ      3'b110
#define C_BNEZ      3'b111
#define C_SLLI      3'b000
#define C_LWSP      3'b010
#define C_SWSP      3'b110

/* funct2 C Extension */
#define C_SRLI      2'b00
#define C_SRAI      2'b01
#define C_ANDI      2'b10
#define C_SUB       2'b00
#define C_XOR       2'b01
#define C_OR        2'b10
#define C_AND       2'b11

/* funct3 A Standard Extension */

/* funct7 field */
#define M_EXT      	7'b0000001 
#define FUNCT7_SFT 	7'b0100000
#define FUNCT7_SUB 	7'b0100000

/* funct12 field */
#define ECALL		12'b000000000000
#define EBREAK		12'b000000000001
#define URET		12'b000000000010
#define SRET		12'b000100000010
#define MRET		12'b001100000010
#define WFI			12'b000100000101

/* funct7 field */
#define SFENCE_VMA	7'b0001001

/* byteena */
#define MEM_WORD        3'b010
#define MEM_HALFWORD    3'b001
#define MEM_BYTE        3'b000
#define MEM_QUADWORD	3'b111

#define LB              3'b000
#define LH              3'b001

/* ALU sel */
#define ALU 1'b0
#define MUNIT 1'b1

#define XLEN 32
#define ILEN 32
#define VALEN 32
#define IALIGN 32
#define ASIDLEN 0
#define ASIDMAX 9
#define LEVELS 2

#endif
