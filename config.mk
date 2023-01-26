LOCAL_IP=192.168.1.152
CC=cross/bin/x86_64-elf-gcc
LD=cross/bin/x86_64-elf-ld
AS = as
ASFLAGS =
CFLAGS=-fexceptions --std=gnu11 -ffreestanding -fno-stack-protector -fno-pic \
	-Werror=implicit -Werror=implicit-function-declaration \
	-Werror=implicit-int -Werror=int-conversion \
	-Werror=incompatible-pointer-types -Werror=int-to-pointer-cast \
	-Werror=return-type -Wunused -mabi=sysv -mno-80387 -mno-mmx -mno-3dnow\
	-mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -pedantic \
	-I sys/include/ -D_KERNEL -Wno-pointer-sign -ggdb

QEMU_FLAGS = --enable-kvm -monitor stdio -cpu qemu64 -M q35 -m 256M -boot d \
						 -smp 2 -netdev tap,id=br0,ifname=tap0,script=no,downscript=no \
						 -device rtl8139,netdev=br0,mac=52:55:00:d1:55:01 \
						 -usb -device qemu-xhci
