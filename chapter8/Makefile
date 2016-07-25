CC=gcc
ARCH = x86_64
XEN_SRC = ../../xen-master

INCLUDE  =  -Iinclude -Iinclude/x86 -DCONFIG_X86_PAE -D__XEN_INTERFACE_VERSION__=0x00030203
LDFLAGS  += -nostdlib
CFLAGS   += -Wall -Werror -fno-stack-protector -fno-builtin-strlen -std=c99 $(INCLUDE)
ASFLAGS  += -D__ASSEMBLY__ $(INCLUDE)

ifeq ($(ARCH),x86_32)
$(info compilation for i386)
CFLAGS  += -m32 -march=i686
LDFLAGS += -m elf_i386 -T loader-x86.lds
ASFLAGS += -m32
OBJS    += bootstrap.x86_32.o
endif
ifeq ($(ARCH),x86_64)
$(info compilation for amd64)
CFLAGS  += -m64
ASFLAGS += -m64
LDFLAGS += -m elf_x86_64 -T loader-x86_64.lds
OBJS    += bootstrap.x86_64.o
endif


.PHONY: all

all: clean testkernel

testkernel: $(OBJS) kernel.o console.o traps.o xenstore.o
	@echo Compiled for ARCH=${ARCH}
	$(LD) $(LDFLAGS) $^ -o testkernel

info:
	@echo make can be run with var ARCH={x86_32,x86_64}
	@echo By default ARCH=x86_64

clean:
	rm -f *.o 
	rm -f testkernel
