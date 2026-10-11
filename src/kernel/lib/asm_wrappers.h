#ifndef SYS_IO_H
#define SYS_IO_H

void outb(unsigned char data, unsigned short port);
void outw(unsigned short data, unsigned short port);
unsigned char inb(unsigned short port);
void rep_insw(unsigned short* buffer, unsigned short count, unsigned short port);
void halt(void);

#endif
