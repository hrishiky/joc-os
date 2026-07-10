#ifndef COMMANDS_H
#define COMMANDS_H

#include "shell.h"

extern void shell_command_heapinfo(Shell_Arguments);
extern void shell_command_vgatest(Shell_Arguments);
extern void shell_command_pmminfo(Shell_Arguments);
extern void shell_command_halt(Shell_Arguments);
extern void shell_command_color(Shell_Arguments);
extern void shell_command_reboot(Shell_Arguments);
extern void shell_command_clear(Shell_Arguments);
extern void shell_command_echo(Shell_Arguments);
extern void shell_command_help(Shell_Arguments);
extern void shell_command_meminfo(Shell_Arguments);
extern void shell_command_ded(Shell_Arguments);
extern void shell_command_temp(Shell_Arguments);

#endif
