# os

![image of os running in QEMU](/image.png)

**information:**
 - operating system with monolithic kernel targeting x86_64 architecture and legacy hardware devices
 - built for learning via simple  / source code to follow
 - made with x86_64 assembly and C; build system using Make
 - features:
    - bootloader
    - memory manager
    - file system (in progress)
    - shell
    - text editor (in progress)

**requirements:**
 - GNU/Linux (for build setup)
 - QEMU (for emulation)
 - GDB (for debugger; optional)

**build/run instructions:**
 - clone the repository and switch to a working commit (commit message has "[ working ]")
 - `make clean` to clean the build
 - `make` to build the disk image
 - `make qemu` to emulate the disk image
 - `make qemu-debug` to emulate the disk image with debugger connected
 - `./auto.sh` to clean, build, and emulate the disk image though GUI
 - `./serial.sh` to clean, build, and emulate the disk image through standard output

**credits:**
 - `ded` in os shell to view dedications
 - bootloader and early build setup: [Operating Systems: From 0 to 1](https://raw.githubusercontent.com/tuhdo/os01/master/Operating_Systems_From_0_to_1.pdf)
 - physical memory management: [BrokenThorn Entertainment](https://brokenthorn.com/Resources/OSDevIndex.html)

**contribution:**
 - would appreciate advice via GitHub issues
 - contributions can be made via pull requests

NO AI GENERATED CODE.
