# joc-os

![image of os running in QEMU](/image.png)

<br>

**information:**
 - operating system with monolithic kernel targeting x86_64 architecture and legacy hardware devices
 - built for learning via simple implementations and simple source code
 - made with x86_64 assembly and C; build system using Make
 <br>
 - features:
    - bootloader
    - memory manager
    - file system
    - shell
    - text editor (in progress)

<br>

**requirements:**
 - GNU coreutils (mkdir, dd, etc.)
 - GNU binutils (ld)
 - Make
 - NASM
 - GCC
 - QEMU (qemu-system-x86_64)
 - GDB (optional)

<br>

**build/run instructions:**
 - clone the repository and switch to a working commit (commit message has "[ working ]")
 - change directory to "/src"
 - `make clean` to clean the build
 - `make` to build the disk image
 - `make qemu` to emulate the disk image
 - `make qemu-debug` to emulate the disk image with debugger connected
 - `./auto.sh` to clean, build, and emulate the disk image with GUI output
 - `./stdout.sh` to clean, build, and emulate the disk image with standard output

<br>

**credits:**
 - `ded` in os shell to view dedications
 - bootloader and early build setup: [Operating Systems: From 0 to 1](https://raw.githubusercontent.com/tuhdo/os01/master/Operating_Systems_From_0_to_1.pdf)
 - physical memory management: [BrokenThorn Entertainment](https://brokenthorn.com/Resources/OSDevIndex.html)

<br>

**contribution:**
 - would appreciate advice via GitHub issues
 - contributions can be made via pull requests

<br>

NO AI GENERATED CODE
