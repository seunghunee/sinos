all: boot_loader 32kernel 64kernel Sinos.iso
	@echo
	@echo ">>> Sinos is completed"

boot_loader:
	@echo
	@echo ">>> Make Boot Loader"
	make -C $@

32kernel:
	@echo
	@echo ">>> Make 32bit mode Kernel"
	make -C $@

64kernel:
	@echo
	@echo ">>> Make 64bit mode Kernel"
	make -C $@

Sinos.iso: boot_loader/boot_loader.bin 32kernel/32kernel.bin 64kernel/64kernel.bin
	@echo
	@echo ">>> Make Sinos Image"
	gcc ./utility/sinos_iso_maker.c -o ./utility/sinos_iso_maker
	./utility/sinos_iso_maker $^


.PHONY: all clean boot_loader 32kernel 64kernel
clean:
	make -C boot_loader clean
	make -C 32kernel clean
	make -C 64kernel clean
	rm -rf Sinos.iso
