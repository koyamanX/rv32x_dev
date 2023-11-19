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
- Installation of Docker 
- Installation of X11 (if you want to debug with GTKWave)  
- Quartus or other synthesis tools (if you want to implement this system on FPGA)

### Prepare Development environment 
- Required tools are built in Dockerfile.

#### Build image
```bash
$ cd image
$ ./build.sh # This will create docker image (Take some time to finish)
```

#### Run container
```bash
$ ./run.sh # This will run bash in docker container
```

#### riscv-tests
- Implementation can be tested with riscv-tests.  
- To test, run following commands.  
- Writting to tohost(0x80001000) will cause simulator to finish its simulation with written value as exit code.  
in 'logs' directory, we can find instruction execution trace. If tests failed, then 'failed' directory will be created and failed test and its vcd will copied into it for debugging.  
- Test will result in passed, failed or timeout. Timeout means simulator has potential bugs which result in infinite loop.  
- Reset vector is 0x00000000. Bootrom is placed on address location of 0x00000000 to 0x00001000.
- Bootrom contains code to jump to 0x80000000 which test codes are placed.
- Passed Tests [STATUS.md](STATUS.md)

```bash
scripts/isa_tests_sim.sh
```
#### Sample apps
#### Hello World(Newlib)
```bash
scripts/hello_sim.sh
```
#### linux
```bash
scripts/linux_sim.sh
```

#### DE10-lite Configuration(The script cannot run in container)
- requirements
- Arduino SD card shiled(Pin assignment: CS -> Arduino_io[4], MOSI -> Arduino_io[11], MISO -> Arduino_io[12], SCLK -> Arduino_io[13])
- Arduino RS232 Shield(Pin assignment: TX -> Arduino_io[0], RX ->Arduino_io[1])
```bash
./linux_DE10_lite.sh
```

### Executing arbitary executable file
- You can simply give executable file to its first arguments.
- If none is given, then a.out in current directory is assumed.
- If writting or reading locations not allocated explicitly in executable file 
will cause simulator to finish its execution with error message.

### rv32x_simulation options
- Simulator executable takes at least one positional argument, RISCV32-little ELF executable.
```
$ cd simulation
$ ./rv32x_simulation [Name_Of_ELF_File] [Options]
```
#### [Options]
Options are prefixed with --.
- print-all (Print execution trace)
- dump-vcd=i,[start]-[end]

### TODO 
- See Issues and Projetcs pages.

### Contribution

### Lisence
