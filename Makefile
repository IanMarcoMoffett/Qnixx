.POSIX:
all: stand/limine sbin/kernel.sys 

include config.mk

OBJECTS=sys/init.o 						\
				sys/amd64/idt.o				\
				sys/tty/console.o 		\
				sys/libkern/string.o

ASMOBJS=

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
	qemu-system-x86_64 $(QEMU_FLAGS) -cdrom Qnixx.iso

stand/limine:
	mkdir -p stand/
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.0-binary --depth=1 stand/limine/
	make -C stand/limine

clean:
	@ rm -f $(OBJECTS) $(ASMOBJS) sbin/kernel.sys
