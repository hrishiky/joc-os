#include "test_ata.h"

#include "stdio.h"
#include "stdbool.h"

#include "ata.h"

bool test_ata(void) {
	printf("test_ata starting\n");

	if (test_ata_1()) {
		printf("test_ata_1 passed\n");
	}

	printf("\ntest_ata done\n");
}

bool test_ata_1(void) {
	uint32_t buffer[128];

	for (uint16_t i = 0; i < 128; i++) {
		buffer[i] = i;
	}

	ata_write(TEST_ATA_DISK_SECTOR, (void*) buffer, sizeof(buffer));

	uint32_t buf[128];

	ata_read(TEST_ATA_DISK_SECTOR, (void*) buf, 1);

	for (uint16_t i = 0; i < 128; i++) {
		if (buffer[i] != buf[i]) {
			return false;
		}
	}

	return true;
}
