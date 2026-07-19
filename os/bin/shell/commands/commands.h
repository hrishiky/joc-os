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
extern void shell_command_print(Shell_Arguments);
extern void shell_command_help(Shell_Arguments);
extern void shell_command_meminfo(Shell_Arguments);
extern void shell_command_ded(Shell_Arguments);
extern void shell_command_temp(Shell_Arguments);
extern void shell_command_ls(Shell_Arguments);
extern void shell_command_mkdir(Shell_Arguments);
extern void shell_command_cd(Shell_Arguments);
extern void shell_command_rmdir(Shell_Arguments);
extern void shell_command_pwd(Shell_Arguments);
extern void shell_command_touch(Shell_Arguments);
extern void shell_command_rm(Shell_Arguments);
extern void shell_command_out(Shell_Arguments);

#endif
