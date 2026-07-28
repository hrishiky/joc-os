# os

**information:**
 - simple operating system targeting x86_64 architecture and legacy hardware devices

**requirements:**
 - QEMU (for emulation)
 - GDB (optional; for debugger)

**build/run instructions:**
 - `make clean` to clean the build
 - `make` to build the disk image
 - `make qemu` to emulate the disk image
 - `./auto.sh` to automatically clean, build, and emulate the disk image
 - `make qemu-debug` to emulate the disk image with gdb debugging
<br></br>

**credits:**
 - `ded` to view dedications (run in the operating system shell)
 - bootloader and early build setup from [Operating Systems: From 0 to 1](https://raw.githubusercontent.com/tuhdo/os01/master/Operating_Systems_From_0_to_1.pdf)
 - physical memory management from [BrokenThorn Entertainment](https://brokenthorn.com/Resources/OSDevIndex.html)
