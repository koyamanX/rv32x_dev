## RISC-V (RV32IMAC) Implementation in NSL
- This repository contains implementation of RV32IMAC(currently supporting RV32IM,Zicsr,Zifencei with Machine-Mode except for memory protection).  
- This implementation is fully synthisable and also able to convert to software simulator(by verilator).  
- Written in [NSL](http://www.overtone.co.jp/products/and-the-nsl/) 
### NSL 
- NSLCORE is compiler tool for NSL.
- NSLCORE can convert NSL code into either Verilog HDL, VHDL, SystemC.
- You can download Trial version of Windows binary of NSLCORE from [NSLCORE Overtone(en)](http://www.overtone.co.jp/en/support/downloads/) or [NSLCORE Overtone(ja)](http://www.overtone.co.jp/support/downloads/). (registration is required, limitation of non-comercial use and code size for trial version (2000 lines?) )
- You can also download [LiveCygwin](http://www.ip-arch.jp/#LiveCygwin) which provides minimum? environment of Cygwin without installing. 
- It also includes verilator, gtkwave, iverilog, vim, NSLCORE and some implementation examples in NSL.
- If you need Linux version of NSL compiler, please consult Overtone.
#### Documentations of NSL
- [NSL Reference en](http://www.overtone.co.jp/wp_overtone/wp-content/uploads/2010/06/NSL_Language_Reference_ver1.1E.pdf) and [NSL Reference ja](http://www.overtone.co.jp/release_data/documents/reference/NSL_Language_Reference_ver1.5.pdf)
- [NSL Tutorial](http://www.overtone.co.jp/software_download/ja/) (registation is required)

### Prerequisites
- Installation of NSL core (SET Environment variable NSLCORE(path to nsl2vl) and specifiy in run.sh to pass it to container) 
- Installation of Docker  (You may also use LiveCygwin + riscv-gnu-toolchain on Windows. DIY, consult Dockerfile)
- Installation of X11 (if you want to debug with GTKWave)  
- Quartus or other synthesis tools (if you want to implement this system on FPGA)

### Testing the implementation
- Implementation can be tested with riscv-tests.  
- To test, run following commands.  
- Writting to tohost(0x80001000) will cause simulator to finish its simulation with written value as exit code.  
in 'logs' directory, we can find instruction execution trace. If tests failed, then 'failed' directory will be created and failed test and its vcd will copied into it for debugging.  
- Test will result in passed, failed or timeout. Timeout means simulator has potential bugs which result in infinite loop.  
- Reset vector is 0x00000000. Bootrom is placed on address location of 0x00000000 to 0x00000400.
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
- environment_call_from_m_mode
- instruction_address_misaligned
- load_address_misaligned
- store_amo_address_misalgined
- breakpoint
- reset

#### Not-fully implemented
- load_access_fault
- store_amo_access_fault

### Interrupts support
- Machine mode timer interrupt (via CLINT)
- Machine mode software interrupt (via CLINT)
- Machine mode external interrupt (no PLIC, only one source for now)

### Executing arbitary executable file
- You can simply give executable file to its first arguments.
- If none is given, then a.out in current directory is assumed.
- Memory is created by reading sections in executable.
- If writting or reading locations not allocated explicitly in executable file 
will cause simulator to finish its execution with error message.

``` 
$ cd simulation
$ ./rv32x_simulation $NAME_OF_EXECUTABLE
```

### TODO 
- Implement full feature of Machine mode
- Attach GDB RSP server
- Develop SoC for FPGA
- Implement Compress and Atomic
- Implement Supervisor mode

### Implementing on FPGA (sid)
The core can be implemented on FPGA.
However, not well tested. 
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
|CLINT|0x2000\_0000 - 0x2000\_c000|Non-cacheable|RW|
|MMIO DEVICES|0x4000\_0000 - 0x4001\_0000|Non-cacheable|RW|
|RAM0|0x8000\_0000 - 0x8400\_8000|Cacheable|RWX|

R: means readable through data address space.  
W: means writable through data address space.  
X: means executable through instruction address space.  

- Other region in executable file is also created automatically, as Cacheable region.
- MMIO DEVICES includes UART TXD, RXD.
