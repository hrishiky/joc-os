#include "stdio.h"
#include "stdbool.h"

#include "ata.c"

#define TEST_ATA_DISK_SECTOR 500

bool test_ata_1(bool print) {
	uint32_t son[128];

	for (uint16_t i = 0; i < 128; i++) {
		son[i] = i;
	}

	ata_write(TEST_ATA_DISK_SECTOR, (void*) son, sizeof(son));

	uint32_t buffer[128];

	ata_read(TEST_ATA_DISK_SECTOR, (void*) buffer, 1);

	for (uint16_t i = 0; i < 128; i++) {
		if (son[i] != buffer[i]) {
			return false;
		}
	}

	if (print) {
		printf("test_ata_1 passed\n");
	}

	return true;
}
