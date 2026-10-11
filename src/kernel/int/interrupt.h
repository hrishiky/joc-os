#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "interrupt_frame.h"
#include "exception.h"
#include "irq.h"
#include "vga_text.h"

#define ISR_VECTOR_NUMBER_OFFSET 0x78
#define ISR_ERROR_CODE_OFFSET 0x80

void int_central_handler(Int_Regs* regs);

#endif
