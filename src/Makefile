BUILD_DIR := ../build
ISO_DIR := ../isodir
BOOT_DIR := $(ISO_DIR)/boot

boot.o: boot.S
	nasm -f elf32 boot.S -o $(BUILD_DIR)/boot.o

kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o $(BUILD_DIR)/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

myos.bin: boot.o kernel.o linker.ld
	i686-elf-ld -T linker.ld -o $(BUILD_DIR)/myos.bin $(BUILD_DIR)/kernel.o $(BUILD_DIR)/boot.o 
	@if grub-file --is-x86-multiboot $(BUILD_DIR)/myos.bin; then \
		echo multiboot confirmed; \
	else \
		echo file is not multiboot; \
		exit 1; \
	fi

all: myos.bin
	@mkdir -p $(BOOT_DIR)
	@cp $(BUILD_DIR)/myos.bin $(BOOT_DIR)/myos.bin
	grub-mkrescue -o $(BUILD_DIR)/myos.iso $(ISO_DIR)

run: all
	qemu-system-i386 -cdrom $(BUILD_DIR)/myos.iso -nographic -serial stdio -monitor none

clean:
	rm $(BUILD_DIR)/*
	rm $(BOOT_DIR)/myos.bin