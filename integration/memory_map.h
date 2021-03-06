#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "clint.h"

#define BOOTROM_BASE 			0x0000_0000
#define BOOTROM_LENGTH 			0x0000_1000
#define CLINT_BASE				CLINT_BASE_ADDR
#define CLINT_LENGTH			CLINT_SIZE
#define AHB_LITE_IO_BASE		0x1000_0000
#define AHB_LITE_IO_LENGTH		0x0001_0000
#define AHB_LITE_IO0_BASE		0x4000_0000
#define AHB_LITE_IO0_LENGTH		0x0000_1000
#define AHB_LITE_IO1_BASE		0x4000_1000
#define AHB_LITE_IO1_LENGTH		0x0000_2000
#define TCM_IMEM_BASE 			0x8000_0000
#define TCM_IMEM_LENGTH			0x0000_8000
#define AHB_LITE_SDRAM_BASE		0x8000_8000
#define AHB_LITE_SDRAM_LENGTH	0x0400_0000
#define TCM_DMEM_BASE			0x8400_0000
#define TCM_DMEM_LENGTH			0x0000_8000
#define PLIC_BASE 				0x0c00_0000
#define PLIC_LENGTH				0x1000_0000

#endif
