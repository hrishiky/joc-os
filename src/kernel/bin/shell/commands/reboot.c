#include "shell.h"

void shell_command_reboot(Shell_Arguments arguments) {
	__asm__ volatile (
		"lidt (0)"
		:
		:
		: "memory"
	);

	__asm__ volatile ("int $3");
}
