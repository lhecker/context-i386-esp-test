ARCH := $(shell getconf LONG_BIT)

ASM_32 := src/jump_i386_sysv_elf_gas.S src/make_i386_sysv_elf_gas.S
ASM_64 := src/jump_x86_64_sysv_elf_gas.S src/make_x86_64_sysv_elf_gas.S

ASM := $(ASM_$(ARCH))

all:
	g++ -g -std=c++11 -o esp $(ASM) src/esp.cc
	g++ -g -std=c++11 -msse2 -o echosse $(ASM) src/echosse.cc
