BUILD_DIR=../build

BOOTLOADER=$(BUILD_DIR)/bootloader/bootloader.bin
MODE=$(BUILD_DIR)/bootloader/mode.bin
LOADER=$(BUILD_DIR)/bootloader/elf_loader.bin
KERNEL=$(BUILD_DIR)/kernel/kernel.elf

DISK_IMG=$(BUILD_DIR)/disk.img

MODE_SECTORS=$(shell echo $$((($$(stat --printf="%s" $(MODE)) + 511) / 512)))
LOADER_SECTORS=$(shell echo $$((($$(stat --printf="%s" $(LOADER)) + 511) / 512)))
KERNEL_SECTORS=$(shell echo $$((($$(stat --printf="%s" $(KERNEL)) + 511) / 512)))


all: bootdisk

.PHONY: bootdisk bootloader kernel

bootloader:
	make -C bootloader

kernel:
	make -C kernel

bootdisk: bootloader kernel
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=1 seek=0
	dd conv=notrunc if=$(MODE) of=$(DISK_IMG) bs=512 count=$(MODE_SECTORS) seek=1
	dd conv=notrunc if=$(LOADER) of=$(DISK_IMG) bs=512 count=$(LOADER_SECTORS) seek=$$(($(MODE_SECTORS) + 1))
	dd conv=notrunc if=$(KERNEL) of=$(DISK_IMG) bs=512 count=$(KERNEL_SECTORS) seek=$$(($(MODE_SECTORS) + $(LOADER_SECTORS) + 1))

qemu:
	qemu-system-x86_64 -d in_asm,cpu_reset,int,guest_errors -no-reboot -machine pc -drive file=$(DISK_IMG),format=raw,if=ide -boot c

qemu-stdout:
	qemu-system-x86_64 -d cpu_reset,guest_errors -no-reboot -machine pc -drive file=$(DISK_IMG),format=raw,if=ide -boot c -display curses

qemu-debug:
	qemu-system-x86_64 -d in_asm,cpu_reset,int,guest_errors -no-reboot -machine pc -drive file=$(DISK_IMG),format=raw,if=ide -boot c -gdb tcp::26000 -S

clean:
	make -C bootloader clean
	make -C kernel clean
	rm $(DISK_IMG)
