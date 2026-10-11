#ifndef ATA_H
#define ATA_H

#define ATA_REGISTER_DATA 0x1F0
#define ATA_REGISTER_SECTORCOUNT 0x1F2
#define ATA_REGISTER_LBA0 0x1F3
#define ATA_REGISTER_LBA1 0x1F4
#define ATA_REGISTER_LBA2 0x1F5
#define ATA_REGISTER_DRIVEHEAD 0x1F6
#define ATA_REGISTER_COMMAND 0x1F7
#define ATA_REGISTER_STATUS 0x1F7
#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_ERR 0x01
#define ATA_COMMAND_READ 0x20
#define ATA_DRIVE_MASTERLBA 0xE0
#define ATA_LBA_MASK 0x0FFFFFFF
#define SECTOR_WORD_COUNT 0x100

void outb(unsigned char data, unsigned short port);
unsigned char inb(unsigned short port);
void rep_insw(unsigned short* buffer, unsigned short count, unsigned short port);

void ata_check_error(void);
void ata_wait_bsy(void);
void ata_wait_drdy(void);
void ata_wait_drq(void);
void ata_prepare_read(unsigned int lba, unsigned char count);
void ata_read(unsigned short* buffer, unsigned char count);

#endif
