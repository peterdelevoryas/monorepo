all: kernel8.img

boot.o: boot.S
	aarch64-linux-gnu-as boot.S -o boot.o -c

kernel.o: kernel.cpp
	aarch64-linux-gnu-g++ -std=c++20 kernel.cpp -o kernel.o -ffreestanding -c -O2 -Wall -Wextra

kernel8.elf: boot.o kernel.o
	aarch64-linux-gnu-ld boot.o kernel.o -T link.ld -o kernel8.elf

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy kernel8.elf -O binary kernel8.img

.PHONY: test

test: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio
