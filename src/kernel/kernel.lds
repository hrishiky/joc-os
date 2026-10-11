ENTRY(main);

SECTIONS
{
	. = 0x35000;

	kernel_start = .;

	.text ALIGN(0x1000) : { *(.text) *(.rodata) }
	.data ALIGN(0x1000) : { *(.data) }
	.bss ALIGN(0x1000) : { *(COMMON) *(.bss) }
	/DISCARD/ : { *(.eh_frame) *(.comment) }

	kernel_end = .;
}
