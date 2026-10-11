BUILD_DIR := ../../build/kernel

KERNEL_SRC_DIRS := $(shell find . -type d)
KERNEL := $(BUILD_DIR)/kernel.elf

CFLAGS += -ffreestanding -nostdlib -gdwarf-4 -m64 -ggdb3 -g
CFLAGS += $(patsubst %, -I%, $(KERNEL_SRC_DIRS))

KERNEL_C_SRCS := $(foreach dir, $(shell find . -type d), $(wildcard $(dir)/*.c))
KERNEL_ASM_SRCS := $(foreach dir, $(shell find . -type d), $(wildcard $(dir)/*.asm))

KERNEL_C_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(KERNEL_C_SRCS))
KERNEL_ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(KERNEL_ASM_SRCS))

KERNEL_OBJS := $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS)

DRIVER_OBJS := $(wildcard $(DRIVER_DIR)/*.o)

all: $(KERNEL)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

$(KERNEL): $(KERNEL_OBJS)
	ld -m elf_x86_64 -nmagic -T kernel.lds $(KERNEL_OBJS) $(DRIVER_OBJS) -o $@

clean:
	rm -f $(KERNEL_OBJS)
