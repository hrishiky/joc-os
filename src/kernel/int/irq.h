#ifndef IRQ_H
#define IRQ_H

#include "keyboard.h"
#include "interrupt_frame.h"
#include "stdint.h"

#define INT_IRQ_VECTOR_ATA 14
#define INT_IRQ_VECTOR_KEYBOARD 33

void int_irq_central_handler(Int_Regs* regs, uint32_t vector, uint32_t error);

#endif
