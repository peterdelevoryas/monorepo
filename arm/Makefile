all: kernel8.img

boot.o: boot.S
	aarch64-linux-gnu-as boot.S -o boot.o -c

main.o: main.c
	aarch64-linux-gnu-gcc -std=c11 main.c -o main.o -ffreestanding -c -O2 -Wall -Wextra

kernel8.elf: boot.o main.o
	aarch64-linux-gnu-ld boot.o main.o -T link.ld -o kernel8.elf

kernel8.img: kernel8.elf
	aarch64-linux-gnu-objcopy kernel8.elf -O binary kernel8.img

.PHONY: test

test: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio
