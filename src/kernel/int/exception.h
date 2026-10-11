#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "vga_text.h"
#include "interrupt_frame.h"

void int_exception_central_handler(Int_Regs* regs, unsigned long vector, unsigned long error);

#endif
