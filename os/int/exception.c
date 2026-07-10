#include "interrupt_frame.h"
#include "stdio.h"
#include "exception.h"

void int_exception_central_handler(Int_Regs* regs, unsigned long vector, unsigned long error) {
	printf("\nCPU EXCEPTION: 0x%x\n", vector);
        printf("ERROR: 0x%x\n", error);
	__asm__ volatile ("hlt");
}
