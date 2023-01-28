.POSIX:
all: stand/limine sbin/kernel.sys 

include config.mk

OBJECTS=sys/kern_init.o 					\
				sys/amd64/idt.o						\
				sys/amd64/interrupts.o		\
				sys/amd64/exceptions.o		\
				sys/amd64/lapic.o					\
				sys/amd64/ioapic.o				\
				sys/amd64/cpu.o						\
				sys/amd64/smp.o						\
				sys/amd64/io.o						\
				sys/amd64/gdt.o						\
				sys/amd64/tss.o						\
				sys/acpi/acpi.o						\
				sys/sync/mutex.o					\
				sys/sched/process.o				\
				sys/mm/pmm.o							\
				sys/mm/vmm.o							\
				sys/mm/tlsf.o							\
				sys/mm/heap.o							\
				sys/fs/vfs.o							\
				sys/dev/video/fb.o				\
				sys/dev/timer/hpet.o			\
				sys/dev/net/rtl8139.o			\
				sys/dev/pci/pci.o					\
				sys/dev/init.o						\
				sys/tty/console.o 				\
				sys/tty/font.o 						\
				sys/libkern/string.o			\
				sys/libkern/bitmap.o

ASMOBJS=sys/amd64/asm.o 			\
				sys/amd64/tlbflush.o	\
				sys/amd64/cpuasm.o		\
				sys/amd64/tssasm.o

sbin/kernel.sys: $(OBJECTS) $(ASMOBJS)
	mkdir -p sbin/
	$(LD) -nostdlib -zmax-page-size=0x1000 -static -Tsys/link.ld\
		$(OBJECTS) $(ASMOBJS) -o $@
	@ # Create needed directories
	mkdir -p iso_root/boot/
	# Build ISO
	cp etc/limine.cfg stand/limine/limine.sys stand/limine/limine-cd.bin \
		stand/limine/limine-cd-efi.bin iso_root/
		cp sbin/kernel.sys iso_root/boot/
	xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 \
		-boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part \
		--efi-boot-image --protective-msdos-label iso_root -o Qnixx.iso
	stand/limine/limine-deploy Qnixx.iso
	rm -rf iso_root

run:
	@echo "Creating tap..."
	@sudo ip tuntap add dev tap0 mode tap user $(shell id -u)
	@sudo ip address add $(LOCAL_IP) dev tap0
	@sudo ip link set dev tap0 up
	qemu-system-x86_64 $(QEMU_FLAGS) -cdrom Qnixx.iso
	@echo "Removing tap..."
	@sudo ip tuntap del dev tap0 mode tap

stand/limine:
	mkdir -p stand/
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.0-binary --depth=1 stand/limine/
	make -C stand/limine

clean:
	@ rm -f $(OBJECTS) $(ASMOBJS) sbin/kernel.sys
