## RISC-V (RV32IMAC) Implementation in NSL
This repository contains implementation of RV32IMAC(currently supporting RV32IM with partially Machine-Mode).
This implementation is fully synthisable and also able to convert to software simulator(by verilator).
Written in [NSL](http://www.overtone.co.jp/products/and-the-nsl/)

### Prerequisites
- Installation of NSL core (SET Environment variable NSLCORE(path to nsl2vl) and specifiy in run.sh to pass it to container)
- Installation of Docker
- Installation of X11 (for GTKWave)

### Testing the implementation
Implementation can be tested with riscv-tests.
To test, run following commands.
Writting to tohost(0x80001000) will cause simulator to finish its simulation with written value as exit code.
in 'logs' directory, we can find instruction execution trace.
if tests failed, then 'failed' directory will be created and failed test and its vcd will copied into it for debugging.
Reset vector is 0x00000000.
Bootrom is placed on address location of 0x00000000 to 0x00000400.
Bootrom contains code to jump to 0x80000000 which test codes are placed.
```
$ ./build.sh # Build docker image (need for first time)
$ ./run.sh 'make -C simulation all'
$ ./run.sh 'make -C run_riscv_tests'
```

#### Passed Tests
- rv32ui-p-*
- rv32um-p-*
- rv32mi-p-* (except for breakpoint, ma_fetch, ma_adder)

### Exceptions supported (However priority encoder has known issues)
- illegal_instruction_exception
- environment_call_from_m_mode
- instruction_address_misaligned
- breakpoint
#### stubs
- load_address_misaligned
- load_access_fault
- store_address_misalgined
- store_access_fault

### Interrupts support
- Not-Yet-Implemented


### Executing arbitary executable file
You can simply give executable file to its first arguments.
If none is given, then a.out in current directory is assumed.
Memory is created by reading sections in executable.
If writting or reading locations not allocated explicitly in executable file
	will cause simulator to finish its execution with error message.

``` 
$ cd simulation
$ ./rv32x_simulation $NAME_OF_EXECUTABLE
```

### TODO 
- Implement full feature of Machine mode
- Refactor rv32x\_simulation.cpp
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



