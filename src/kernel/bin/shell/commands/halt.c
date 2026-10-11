#include "shell.h"

#include "asm_wrappers.h"

void shell_command_halt(Shell_Arguments arguments) {
	halt();
}
