## RISC-V (RV32IMAC) Implementation in NSL
- This repository contains implementation of RV32IMAC(currently supporting RV32IMA,Zicsr,Zifencei,SV32, simple PMAs and some delegation to S-Mode with User-Mode and Supervisor-Mode, Machine-Mode except for PMP, HPM).  
- This implementation is fully synthisable and also able to convert to software simulator(by verilator).  
- Written in [NSL](http://www.overtone.co.jp/products/and-the-nsl/) 
### NSL 
- NSLCORE is compiler tool for NSL.
- NSLCORE can convert NSL code into either Verilog HDL, VHDL, SystemC.
- You can download Trial version of Windows binary of NSLCORE from [NSLCORE Overtone(en)](http://www.overtone.co.jp/en/support/downloads/) or [NSLCORE Overtone(ja)](http://www.overtone.co.jp/support/downloads/). (registration is required, limitation of non-comercial use and code size for trial version (2000 lines?) )
- Currently over 2000 lines(must remove empty lines after preprocessing)
- You can also download [LiveCygwin](http://www.ip-arch.jp/#LiveCygwin) which provides minimum? environment of Cygwin without installing. 
- LiveCygwin includes verilator, gtkwave, iverilog, vim, NSLCORE and some implementation examples in NSL.
- If you need Linux version of NSL compiler, please consult Overtone.
#### Documentations of NSL
- [NSL Reference en](http://www.overtone.co.jp/wp_overtone/wp-content/uploads/2010/06/NSL_Language_Reference_ver1.1E.pdf) and [NSL Reference ja](http://www.overtone.co.jp/release_data/documents/reference/NSL_Language_Reference_ver1.5.pdf)
- [NSL Tutorial](http://www.overtone.co.jp/software_download/ja/) (registation is required)

### Prerequisites
- Installation of NSL core (SET Environment variable NSLCORE(path to nsl2vl) and specifiy in run.sh to pass it to container) 
- Installation of Docker  (used for bulid with tools inside docker image, if you have installed gcc, binutils, libbfd, libopcodes, liberty, libz, libdl for riscv32 with IM, ilp32, softfloat then you can invoke make directly in your host system)
- Installation of X11 (if you want to debug with GTKWave)  
- Quartus or other synthesis tools (if you want to implement this system on FPGA)

### Testing the implementation
- Implementation can be tested with riscv-tests.  
- To test, run following commands.  
- Writting to tohost(0x80001000) will cause simulator to finish its simulation with written value as exit code.  
in 'logs' directory, we can find instruction execution trace. If tests failed, then 'failed' directory will be created and failed test and its vcd will copied into it for debugging.  
- Test will result in passed, failed or timeout. Timeout means simulator has potential bugs which result in infinite loop.  
- Reset vector is 0x00000000. Bootrom is placed on address location of 0x00000000 to 0x00001000.
- Bootrom contains code to jump to 0x80000000 which test codes are placed.
```
$ ./build.sh # Build docker image (need for first time)
$ ./run.sh 'make -C simulation all'
$ ./run.sh 'make -C run_riscv_tests'
```

#### Passed Tests
- See [STATUS.md](STATUS.md)

### Exceptions supported
- illegal_instruction_exception
- instruction_address_misaligned
- environment_call_from_m_mode
- environment_call_from_s_mode
- environment_call_from_u_mode
- environment_call
- load_address_misaligned
- store_amo_address_misalgined
- load_access_fault
- store_amo_access_fault
- instruction_access_fault
- load_page_fault
- store_amo_page_fault
- instruction_page_fault
- reset

### Interrupts support
- Machine mode timer interrupt (via CLINT)
- Machine mode software interrupt (via CLINT)
- Machine mode external interrupt (via PLIC (32 sources))
- Supervisor mode timer interrupt (via CLINT) or via {m,s}ip.stip
- Supervisor mode software interrupt (via CLINT) or via {m,s}ip.ssip
- Supervisor mode external interrupt (via PLIC (32 sources)) or via {m,s}ip.seip

### delegation support (delegation to S-Mode)
#### Exceptions delegation
- environment_call_from_s_mode
- environment_call_from_u_mode
- load_access_fault
- store_amo_access_fault
- instruction_access_fault
- load_page_fault
- store_amo_page_fault
- instruction_page_fault
- instruction_address_misaligned
- breakpoint
#### Interrupts delegation
- supervisor mode external interrupt
- supervisor mode software interrupt
- supervisor mode timer interrupt

### Executing arbitary executable file
- You can simply give executable file to its first arguments.
- If none is given, then a.out in current directory is assumed.
- If writting or reading locations not allocated explicitly in executable file 
will cause simulator to finish its execution with error message.

### rv32x_simulation options

``` 
$ cd simulation
$ ./rv32x_simulation $NAME_OF_EXECUTABLE
```

### TODO 
- Implement PMP, PMA for Instruction address
- Implement HPM
- Implement N-Extension
- Implement SV32
- Implement AMO
- Improve Fmax for FPGA implementation
- Attach GDB RSP server
- Develop SoC for FPGA
- Implement Compress and Atomic
- Implement Supervisor mode

### Implementing on FPGA (sid)
The core can be implemented on FPGA.
However, not well-tested. 
SoC is currently under developing for FPGA.
- UART 				(Implemented)
- VGA Controller	(Implemented)
- SDRAM Controller	(sid)
- SPI SDHC			(sid)
#### Memory Map
see integration/memory\_map.h  
For simulation purpose, address map below is mapped. 
|Name|Region|Is cacheable?|Accessibilty|
|--|--|--|--|
|BOOTROM|0x0000\_0000 - 0x0000\_1000|Cacheable|X|
|PLIC|0x0c00\_0000 - 0x1000\_0000|Non-cacheable|RWA|
|CLINT|0x0200\_0000 - 0x0200\_c000|Non-cacheable|RWA|
|MMIO DEVICES|0x4000\_0000 - 0x4001\_0000|Non-cacheable|RWA|
|RAM0|0x8000\_0000 - 0x8400\_8000|Cacheable|RWXA|



R: means readable through data address space.  
W: means writable through data address space.  
X: means executable through instruction address space.  
A: means atomic operation is allowed through data address space.

- MMIO DEVICES includes UART TXD, RXD.
