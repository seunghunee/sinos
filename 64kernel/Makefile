nasm_flags = -f elf64
g++_flags = -c -m64 -ffreestanding -fno-exceptions -fno-rtti -std=c++14
ld_flags = -T sinos64.x -Ttext 0x100000 -e main -melf_x86_64 -nostdlib
objcopy_flags = -j .text -j .data -j .rodata -j .bss -j .ctors -j .dtors -S -O binary

cc_files = $(wildcard *.cc)
obj_files = $(cc_files:.cc=.o)

target = 64kernel.bin


all: $(target) header.dep

$(target): 64kernel.elf
	objcopy $(objcopy_flags) $< $@

64kernel.elf: 64kernel_entrypoint.o $(obj_files)
	ld $(ld_flags) -o $@ $^

%.o: %.cc
	g++ $(g++_flags) $<
%.o: %.s
	nasm $(nasm_flags) -o $@ $<

ifeq (header.dep, $(wildcard header.dep))
include header.dep
endif

header.dep:
	g++ -MM $(cc_files) > $@



.PHONY: all clean
clean:
	rm -rf *.bin *.elf *.o *.dep
