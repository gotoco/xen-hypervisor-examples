CC=gcc
ARCH = x86_64
LIB_XEN_SERV = ../../libxenserver/src/include/


CFLAGS   += -fno-stack-protector -std=c99 -I$(LIB_XEN_SERV)
LDFLAGS  += -lxenserver -lcurl

enumerate_vms: enumerate_vms.o
	$(LD) $(LDFLAGS) $^ -o enumerate_vms 

clean:
	rm -f *.o 
	rm -f enumerate_vms
