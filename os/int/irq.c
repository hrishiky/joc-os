#include "interrupt_frame.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "keyboard.h"
#include "ata.h"
#include "asm_wrappers.h"
#include "irq.h"

extern bool ata_irq_flag;

void int_irq_central_handler(Int_Regs* regs, uint32_t vector, uint32_t error) {
	switch (vector) {
		case INT_IRQ_VECTOR_ATA:
			printf("ATA INTERRUPT\n");
			// uint8_t ata_status = inb(ATA_REGISTER_STATUS);
			// ata_irq_flag = true;
			// outb(0x20, 0xA0);
			// outb(0x20, 0x20);
			break;

		case INT_IRQ_VECTOR_KEYBOARD:
			uint8_t keyboard_data = inb(KEYBOARD_PORT_DATA);
			keyboard_central_handler(keyboard_data);
			outb(KEYBOARD_EOI, KEYBOARD_PIC1_PORT_COMMAND);
			break;

		default:
			printf("\nIRQ INTERRUPT: 0x%x\n", vector);
		        printf("ERROR: 0x%x\n", error);
			outb(0x20, 0x20);
			break;
	}
}
