set architecture i386:x86-64
set disassembly-flavor intel
target remote localhost:26000
layout src
b *0x7c00
b main
