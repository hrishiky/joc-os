BUILD_DIR := ../build/os

OS_SRC_DIRS := $(shell find . -type d)
OS := $(BUILD_DIR)/os.elf

CFLAGS += -ffreestanding -nostdlib -gdwarf-4 -m64 -ggdb3 -g
CFLAGS += $(patsubst %, -I%, $(OS_SRC_DIRS))

OS_C_SRCS := $(foreach dir, $(shell find . -type d), $(wildcard $(dir)/*.c))
OS_ASM_SRCS := $(foreach dir, $(shell find . -type d), $(wildcard $(dir)/*.asm))

OS_C_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_C_SRCS))
OS_ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(OS_ASM_SRCS))

OS_OBJS := $(OS_C_OBJS) $(OS_ASM_OBJS)

DRIVER_OBJS := $(wildcard $(DRIVER_DIR)/*.o)

all: $(OS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

$(OS): $(OS_OBJS)
	ld -m elf_x86_64 -nmagic -T os.lds $(OS_OBJS) $(DRIVER_OBJS) -o $@

clean:
	rm -f $(OS_OBJS)
