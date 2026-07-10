#ifndef ATA_H
#define ATA_H

#include "stdint.h"
#include "stdbool.h"

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
#define ATA_COMMAND_WRITE 0x30

#define ATA_DRIVE_MASTERLBA 0xE0
#define ATA_LBA_MASK 0x0FFFFFFF

#define ATA_SECTOR_WORD_COUNT 256

typedef enum {
	ATA_SUCCESS = 0,
	ATA_ERROR = 1,
	ATA_TIMEOUT = 2,
	ATA_IRQ_DISABLED = 3
} ata_return_t;


bool ata_check_error(void);
uint8_t ata_wait_bsy(void);
uint8_t ata_wait_drdy(void);
uint8_t ata_wait_drq(void);
uint8_t ata_read(uint64_t lba, void* buffer, uint8_t count);
uint8_t ata_write(uint64_t lba, void* buffer, uint64_t count);

#endif
