BUILD_DIR = ../../build/bootloader

BOOTLOADER_SRCS := $(wildcard *.asm)
BOOTLOADER_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.bin, $(BOOTLOADER_SRCS))

LOADER_SRCS := $(wildcard *.c)
LOADER_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(LOADER_SRCS))

LOADER_ELF := $(BUILD_DIR)/elf_loader.elf
LOADER_BIN := $(BUILD_DIR)/elf_loader.bin

CFLAGS += -ffreestanding -nostdlib -gdwarf-4 -m64 -ggdb3

all: $(BOOTLOADER_OBJS) $(LOADER_BIN)

$(BUILD_DIR)/%.bin: %.asm
	nasm -f bin $< -o $@

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(LOADER_BIN): $(LOADER_OBJS)
	ld -m elf_x86_64 -T elf_loader.lds -o $(LOADER_ELF) $(LOADER_OBJS)
	objcopy -O binary $(LOADER_ELF) $(LOADER_BIN)

clean:
	rm $(BUILD_DIR)/*

-include $(LOADER_OBJS:.o=.d)
