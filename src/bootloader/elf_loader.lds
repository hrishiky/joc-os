ENTRY(main)

SECTIONS {
	. = 0x30000;
	.start : { *(.start) }
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
