#include "ata.h"

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "asm_wrappers.h"


// add byte reading capability to ata read


void ata_init(void) {
	;
}

bool ata_check_error(void) {
	if ((inb(ATA_REGISTER_STATUS) & ATA_STATUS_ERR) != 0) {
		return true;
	}

	return false;
}

uint8_t ata_wait_bsy(void) {
	while (inb(ATA_REGISTER_STATUS) & ATA_STATUS_BSY) {}

	// return ATA_TIMEOUT;
}

uint8_t ata_wait_drdy(void) {
	while (1) {
		unsigned char status = inb(ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			return ATA_ERROR;
		}

		if ((status & ATA_STATUS_DRDY) != 0) {
			return ATA_SUCCESS;
		}
	}

	return ATA_TIMEOUT;
}

uint8_t ata_wait_drq(void) {
	while (1) {
		unsigned char status = inb(ATA_REGISTER_STATUS);

		if (status & ATA_STATUS_ERR) {
			return ATA_ERROR;
		}

		if (!(status & ATA_STATUS_BSY) &&
		    (status & ATA_STATUS_DRQ)) {
			return ATA_SUCCESS;
		}
	}

	return ATA_TIMEOUT;
}

uint8_t ata_read(uint64_t lba, void* buffer, uint8_t sector_count) {
	// printf("ata read called ");

	lba &= ATA_LBA_MASK;

	uint16_t* buf = (uint16_t*) buffer;

	if (ata_wait_bsy() == ATA_TIMEOUT) {
		return ATA_TIMEOUT;
	}

	outb(sector_count, ATA_REGISTER_SECTORCOUNT);
	outb(lba & 0xFF, ATA_REGISTER_LBA0);
	outb((lba >> 8) & 0xFF, ATA_REGISTER_LBA1);
	outb((lba >> 16) & 0xFF, ATA_REGISTER_LBA2);
	outb(ATA_DRIVE_MASTERLBA | ((lba >> 24) & 0x0F), ATA_REGISTER_DRIVEHEAD);

	uint8_t drdy_return = ata_wait_drdy();

	if (drdy_return != ATA_SUCCESS) {
		return drdy_return;
	}

	outb(ATA_COMMAND_READ, ATA_REGISTER_COMMAND);

	for (uint8_t i = 0; i < sector_count; i++) {
		uint8_t drq_return = ata_wait_drq();

		if (drq_return != ATA_SUCCESS) {
			return drq_return;
		}

		rep_insw(buf, ATA_SECTOR_WORD_COUNT, ATA_REGISTER_DATA);
		buf += ATA_SECTOR_WORD_COUNT;

		if (ata_check_error()) {
			return ATA_ERROR;
		}

		if (ata_wait_bsy() == ATA_TIMEOUT) {
			return ATA_TIMEOUT;
		}
	}

	// printf("done.\n");

	return ATA_SUCCESS;
}

uint8_t ata_write(uint64_t lba, void* buffer, uint64_t size) {
	// printf("ata write called ");

	lba &= ATA_LBA_MASK;

	uint64_t words = size / 2;
	uint8_t sector_count = (words + ATA_SECTOR_WORD_COUNT - 1) / ATA_SECTOR_WORD_COUNT;

	uint16_t* buf = (uint16_t*) buffer;
	uint64_t buf_pos = 0;

	if (ata_wait_bsy() == ATA_TIMEOUT) {
		return ATA_TIMEOUT;
	}

	outb(sector_count, ATA_REGISTER_SECTORCOUNT);
	outb(lba & 0xFF, ATA_REGISTER_LBA0);
	outb((lba >> 8) & 0xFF, ATA_REGISTER_LBA1);
	outb((lba >> 16) & 0xFF, ATA_REGISTER_LBA2);
	outb(ATA_DRIVE_MASTERLBA | ((lba >> 24) & 0x0F), ATA_REGISTER_DRIVEHEAD);

	uint8_t drdy_return = ata_wait_drdy();

	if (drdy_return != ATA_SUCCESS) {
		return drdy_return;
	}

	outb(ATA_COMMAND_WRITE, ATA_REGISTER_COMMAND);

	for (uint8_t i = 0; i < sector_count; i++) {
		if (ata_wait_bsy() == ATA_TIMEOUT) {
			return ATA_TIMEOUT;
		}

		uint8_t status = inb(ATA_REGISTER_STATUS);

		if ((status & ATA_STATUS_ERR) ||
		    !(status & ATA_STATUS_DRQ)) {
			return ATA_ERROR;
		}

		if (words >= 256) {
			for (uint16_t j = buf_pos; j < buf_pos + ATA_SECTOR_WORD_COUNT; j++) {
				outw(buf[j], ATA_REGISTER_DATA);
			}
		} else {
			for (uint8_t j = buf_pos; j < buf_pos + words; j++) {
				outw(buf[j], ATA_REGISTER_DATA);
			}

			for (uint8_t j = 0; j < ATA_SECTOR_WORD_COUNT - words; j++) {
				outw(0, ATA_REGISTER_DATA);
			}
		}

		words -= ATA_SECTOR_WORD_COUNT;
		buf_pos += ATA_SECTOR_WORD_COUNT;
	}

	outb(0xE7, ATA_REGISTER_COMMAND);

	if (ata_wait_bsy() == ATA_TIMEOUT) {
		return ATA_TIMEOUT;
	}

	// printf("done.\n");

	return ATA_SUCCESS;
}
