#ifndef INTERRUPT_FRAME_H
#define INTERRUPT_FRAME_H

typedef struct {
	unsigned long rax;
	unsigned long rbx;
	unsigned long rcx;
	unsigned long rdx;
	unsigned long rbp;
	unsigned long rdi;
	unsigned long rsi;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long r13;
	unsigned long r14;
	unsigned long r15;
} __attribute__((packed)) Int_Regs;

#endif
