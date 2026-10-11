org 0x8000

[bits 16]
start:
	jmp	real_mode

gdt32_start:
	dd	0x00000000
	dd	0x00000000
	dd	0x0000FFFF
	dd	0x00CF9A00
	dd	0x0000FFFF
	dd	0x00CF9200
gdt32_end:

align 4
gdt32_location:
	dw	gdt32_end - gdt32_start - 1
	dd	gdt32_start

gdt64_start:
	dq	0x0000000000000000
	dq	0x00209A0000000000
	dq	0x0000920000000000
gdt64_end:

align 4
gdt64_location:
	dw	gdt64_end - gdt64_start - 1
	dd	gdt64_start

align 4096
PML4:
	dq	PDPT + 0x03
	times 511 dq 0

align 4096
PDPT:
	dq	PD + 0x03
	times 511 dq 0

align 4096
PD:
	dq	0x00000000 + 0x83
	dq	0x00200000 + 0x83
	dq	0x00400000 + 0x83
	dq	0x00600000 + 0x83
	dq	0x00800000 + 0x83
	dq	0x00A00000 + 0x83
	dq	0x00C00000 + 0x83
	dq	0x00E00000 + 0x83
	dq	0x01000000 + 0x83
	times 503 dq 0

struc memory_map_entry
	.base_address	resq	1
	.length		resq	1
	.entry_type	resd	1
	.acpi_null	resd	1
endstruc

get_memory_map:
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di

	xor	ebx, ebx
	xor	bp, bp
	mov	edx, 'PAMS'
	mov	eax, 0xE820
	mov	ecx, 24
	int	0x15
	jc	.error
	cmp	eax, 'PAMS'
	jne	.error
	test	ebx, ebx
	je	.error
	jmp	.start
.next_entry:
	mov	edx, 'PAMS'
	mov	ecx, 24
	mov	eax, 0xE820
	int	0x15
.start:
	jcxz	.skip_entry
.notext:
	mov	ecx, [es:di + memory_map_entry.length]
	test	ecx, ecx
	jne	short .good_entry
	mov	ecx, [es:di + memory_map_entry.length + 4]
	jecxz	.skip_entry
.good_entry:
	inc	bp
	add	di, 24
.skip_entry:
	cmp	ebx, 0
        jne	.next_entry
	jmp	.done
.error:
	stc
.done:
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax

	ret


real_mode:
	cli

	mov	ax, 0
	mov	es, ax
	mov 	di, 0x1000
	call	get_memory_map

	mov	ax, 0
	mov	ds, ax

	movzx	eax, bp
	mov 	[0x2000 + 0], eax
	mov	dword [0x2000 + 4], 0x00001000
	mov	dword [0x2000 + 8], 0x00000000

	lgdt	[gdt32_location]

	in	al, 0x92
	or	al, 0x02
	out	0x92, al

	mov	eax, cr0
	or	eax, 0x1
	mov	cr0, eax

	jmp	0x08:protected_mode

[bits 32]
protected_mode:
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	mov	esp, 0x90000

	lgdt	[gdt64_location]

	mov	eax, cr4
	or	eax, 1 << 5
	mov	cr4, eax

	mov	eax, PML4
	mov	cr3, eax

	mov	ecx, 0xC0000080
	rdmsr
	or	eax, 1 << 8
	wrmsr

	mov	eax, cr0
	or	eax, 0x80000000
	mov	cr0, eax

	jmp	0x08:long_mode

[bits 64]
long_mode:
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	mov	rsp, 0x90000

	mov	rax, 0x00030000
	jmp	rax
