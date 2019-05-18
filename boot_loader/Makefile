pall: boot_loader.bin

boot_loader.bin: boot_loader.s
	nasm $< -o $@


.PHONY: all clean
clean:
	rm -rf boot_loader.bin
