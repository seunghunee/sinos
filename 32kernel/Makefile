nasm_flags = -f elf32
g++_flags = -c -m32 -ffreestanding -fno-exceptions -fno-rtti -std=c++11
ld_flags = -T sinos32.x -Ttext 0x10200 -e main -melf_i386 -nostdlib
objcopy_flags = -j .text -j .data -j .rodata -j .bss -j .ctors -j .dtors -S -O binary

asm_files = $(wildcard *.s)
asm_modules = $(subst 32kernel_entrypoint.s, ,$(asm_files))
cc_files = $(wildcard *.cc)
cc_modules = $(subst main.cc, , $(cc_files))
obj_modules = $(cc_modules:.cc=.o) $(asm_modules:.s=.o)

target = 32kernel.bin



all: $(target) objdep.dep

$(target): 32kernel_entrypoint.bin 32kernel_main.bin
	cat $^ > $@

32kernel_entrypoint.bin: 32kernel_entrypoint.s
	nasm -o $@ $<

32kernel_main.bin: 32kernel_main.elf
	objcopy $(objcopy_flags) $< $@

32kernel_main.elf: main.o $(obj_modules)
	ld $(ld_flags) -o $@ $^

%.o: %.cc
	g++ $(g++_flags) $<
%.o: %.s
	nasm $(nasm_flags) -o $@ $<

ifeq (objdep.dep, $(wildcard objdep.dep))
include objdep.dep
endif

objdep.dep:
	g++ -MM $(cc_files) > $@


.PHONY: all clean
clean:
	rm -rf *.bin *.elf *.o *.dep
