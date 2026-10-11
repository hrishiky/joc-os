extern int_central_handler

%macro PUSH_REGS 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POP_REGS 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro


%macro isr_no_err 2
global %2
%2:
	push qword 0
	push qword %1
	jmp common_noerr_entry
%endmacro

%macro isr_err 2
global %2
%2:
	push qword %1
	jmp common_err_entry
%endmacro

common_noerr_entry:
	PUSH_REGS
	mov rdi, rsp
	call int_central_handler
	POP_REGS
	add rsp, 16
	iretq

common_err_entry:
	PUSH_REGS
	mov rdi, rsp
	call int_central_handler
	POP_REGS
	add rsp, 8
	iretq

isr_no_err 0, isr0
isr_no_err 1, isr1
isr_no_err 2, isr2
isr_no_err 3, isr3
isr_no_err 4, isr4
isr_no_err 5, isr5
isr_no_err 6, isr6
isr_no_err 7, isr7
isr_no_err 9, isr9
isr_no_err 15, isr15
isr_no_err 16, isr16
isr_no_err 18, isr18
isr_no_err 19, isr19
isr_no_err 20, isr20
isr_no_err 21, isr21
isr_no_err 22, isr22
isr_no_err 23, isr23
isr_no_err 24, isr24
isr_no_err 25, isr25
isr_no_err 26, isr26
isr_no_err 27, isr27
isr_no_err 28, isr28
isr_no_err 29, isr29
isr_no_err 30, isr30
isr_no_err 31, isr31

isr_err 8, isr8
isr_err 10, isr10
isr_err 11, isr11
isr_err 12, isr12
isr_err 13, isr13
isr_err 14, isr14
isr_err 17, isr17

isr_no_err 32, irq0
isr_no_err 33, irq1
isr_no_err 34, irq2
isr_no_err 35, irq3
isr_no_err 36, irq4
isr_no_err 37, irq5
isr_no_err 38, irq6
isr_no_err 39, irq7
isr_no_err 40, irq8
isr_no_err 41, irq9
isr_no_err 42, irq10
isr_no_err 43, irq11
isr_no_err 44, irq12
isr_no_err 45, irq13
isr_no_err 46, irq14
isr_no_err 47, irq15

isr_no_err 48, isr48
isr_no_err 49, isr49
isr_no_err 50, isr50
isr_no_err 51, isr51
isr_no_err 52, isr52
isr_no_err 53, isr53
isr_no_err 54, isr54
isr_no_err 55, isr55
isr_no_err 56, isr56
isr_no_err 57, isr57
isr_no_err 58, isr58
isr_no_err 59, isr59
isr_no_err 60, isr60
isr_no_err 61, isr61
isr_no_err 62, isr62
isr_no_err 63, isr63
isr_no_err 64, isr64
isr_no_err 65, isr65
isr_no_err 66, isr66
isr_no_err 67, isr67
isr_no_err 68, isr68
isr_no_err 69, isr69
isr_no_err 70, isr70
isr_no_err 71, isr71
isr_no_err 72, isr72
isr_no_err 73, isr73
isr_no_err 74, isr74
isr_no_err 75, isr75
isr_no_err 76, isr76
isr_no_err 77, isr77
isr_no_err 78, isr78
isr_no_err 79, isr79
isr_no_err 80, isr80
isr_no_err 81, isr81
isr_no_err 82, isr82
isr_no_err 83, isr83
isr_no_err 84, isr84
isr_no_err 85, isr85
isr_no_err 86, isr86
isr_no_err 87, isr87
isr_no_err 88, isr88
isr_no_err 89, isr89
isr_no_err 90, isr90
isr_no_err 91, isr91
isr_no_err 92, isr92
isr_no_err 93, isr93
isr_no_err 94, isr94
isr_no_err 95, isr95
isr_no_err 96, isr96
isr_no_err 97, isr97
isr_no_err 98, isr98
isr_no_err 99, isr99
isr_no_err 100, isr100
isr_no_err 101, isr101
isr_no_err 102, isr102
isr_no_err 103, isr103
isr_no_err 104, isr104
isr_no_err 105, isr105
isr_no_err 106, isr106
isr_no_err 107, isr107
isr_no_err 108, isr108
isr_no_err 109, isr109
isr_no_err 110, isr110
isr_no_err 111, isr111
isr_no_err 112, isr112
isr_no_err 113, isr113
isr_no_err 114, isr114
isr_no_err 115, isr115
isr_no_err 116, isr116
isr_no_err 117, isr117
isr_no_err 118, isr118
isr_no_err 119, isr119
isr_no_err 120, isr120
isr_no_err 121, isr121
isr_no_err 122, isr122
isr_no_err 123, isr123
isr_no_err 124, isr124
isr_no_err 125, isr125
isr_no_err 126, isr126
isr_no_err 127, isr127
isr_no_err 128, isr128
isr_no_err 129, isr129
isr_no_err 130, isr130
isr_no_err 131, isr131
isr_no_err 132, isr132
isr_no_err 133, isr133
isr_no_err 134, isr134
isr_no_err 135, isr135
isr_no_err 136, isr136
isr_no_err 137, isr137
isr_no_err 138, isr138
isr_no_err 139, isr139
isr_no_err 140, isr140
isr_no_err 141, isr141
isr_no_err 142, isr142
isr_no_err 143, isr143
isr_no_err 144, isr144
isr_no_err 145, isr145
isr_no_err 146, isr146
isr_no_err 147, isr147
isr_no_err 148, isr148
isr_no_err 149, isr149
isr_no_err 150, isr150
isr_no_err 151, isr151
isr_no_err 152, isr152
isr_no_err 153, isr153
isr_no_err 154, isr154
isr_no_err 155, isr155
isr_no_err 156, isr156
isr_no_err 157, isr157
isr_no_err 158, isr158
isr_no_err 159, isr159
isr_no_err 160, isr160
isr_no_err 161, isr161
isr_no_err 162, isr162
isr_no_err 163, isr163
isr_no_err 164, isr164
isr_no_err 165, isr165
isr_no_err 166, isr166
isr_no_err 167, isr167
isr_no_err 168, isr168
isr_no_err 169, isr169
isr_no_err 170, isr170
isr_no_err 171, isr171
isr_no_err 172, isr172
isr_no_err 173, isr173
isr_no_err 174, isr174
isr_no_err 175, isr175
isr_no_err 176, isr176
isr_no_err 177, isr177
isr_no_err 178, isr178
isr_no_err 179, isr179
isr_no_err 180, isr180
isr_no_err 181, isr181
isr_no_err 182, isr182
isr_no_err 183, isr183
isr_no_err 184, isr184
isr_no_err 185, isr185
isr_no_err 186, isr186
isr_no_err 187, isr187
isr_no_err 188, isr188
isr_no_err 189, isr189
isr_no_err 190, isr190
isr_no_err 191, isr191
isr_no_err 192, isr192
isr_no_err 193, isr193
isr_no_err 194, isr194
isr_no_err 195, isr195
isr_no_err 196, isr196
isr_no_err 197, isr197
isr_no_err 198, isr198
isr_no_err 199, isr199
isr_no_err 200, isr200
isr_no_err 201, isr201
isr_no_err 202, isr202
isr_no_err 203, isr203
isr_no_err 204, isr204
isr_no_err 205, isr205
isr_no_err 206, isr206
isr_no_err 207, isr207
isr_no_err 208, isr208
isr_no_err 209, isr209
isr_no_err 210, isr210
isr_no_err 211, isr211
isr_no_err 212, isr212
isr_no_err 213, isr213
isr_no_err 214, isr214
isr_no_err 215, isr215
isr_no_err 216, isr216
isr_no_err 217, isr217
isr_no_err 218, isr218
isr_no_err 219, isr219
isr_no_err 220, isr220
isr_no_err 221, isr221
isr_no_err 222, isr222
isr_no_err 223, isr223
isr_no_err 224, isr224
isr_no_err 225, isr225
isr_no_err 226, isr226
isr_no_err 227, isr227
isr_no_err 228, isr228
isr_no_err 229, isr229
isr_no_err 230, isr230
isr_no_err 231, isr231
isr_no_err 232, isr232
isr_no_err 233, isr233
isr_no_err 234, isr234
isr_no_err 235, isr235
isr_no_err 236, isr236
isr_no_err 237, isr237
isr_no_err 238, isr238
isr_no_err 239, isr239
isr_no_err 240, isr240
isr_no_err 241, isr241
isr_no_err 242, isr242
isr_no_err 243, isr243
isr_no_err 244, isr244
isr_no_err 245, isr245
isr_no_err 246, isr246
isr_no_err 247, isr247
isr_no_err 248, isr248
isr_no_err 249, isr249
isr_no_err 250, isr250
isr_no_err 251, isr251
isr_no_err 252, isr252
isr_no_err 253, isr253
isr_no_err 254, isr254
isr_no_err 255, isr255
