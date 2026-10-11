#include "interrupt.h"

#include "interrupt_frame.h"
#include "exception.h"
#include "irq.h"
#include "stdio.h"

void int_central_handler(Int_Regs* regs) {
	unsigned long vector = *(unsigned long*) ((unsigned char*) regs + ISR_VECTOR_NUMBER_OFFSET);
	unsigned long error = *(unsigned long*) ((unsigned char*) regs + ISR_ERROR_CODE_OFFSET);

	if (vector < 32) {
		int_exception_central_handler(regs, vector, error);
		return;
	} else if (vector < 48) {
		int_irq_central_handler(regs, vector, error);
		return;
	} else {
		printf("\nUNHANDLED INTERRUPT\n");
		return;
	}
}
