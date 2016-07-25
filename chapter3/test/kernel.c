#include "time.h"
#include <stdint.h>
#include <stddef.h>
#include <xen.h>
#include "hypercall_dbg-x86_64.h"

/* Some static space for the stack */
char stack[8192];
/* Page of memory used to hold shared info*/
extern char shared_info[4096];

extern shared_info_t *HYPERVISOR_shared_info;

/* This structure contains start-of-day info, such as pagetable base pointer,
 * address of the shared_info structure, and things like that. */
union start_info_union
{
    start_info_t start_info;
    char padding[512];
};
union start_info_union start_info_union;
#define start_info (start_info_union.start_info)

/* Helpers to don't use external libs */
static
shared_info_t *map_shared_info(unsigned long pa)
{
    int rc;
    
    if ( (rc = HYPERVISOR_update_va_mapping(
         (unsigned long)shared_info, __pte(pa | 7), UVMF_INVLPG)) )
         return (shared_info_t *)0;

    return (shared_info_t *)shared_info;
}

void convert(long num, int base, char buff[30])
{
    char *nums = "0123456789abcdef";
    char *ptr;
    ptr = &buff[29];
    do {
        *--ptr = nums[num%base];
        num /= base;
    } while(num != 0);
}

void * memcpy(void * dest,const void *src,size_t count)
{
    char *tmp = (char *) dest;
    const char *s = src;

    while (count--)
        *tmp++ = *s++;

    return dest;
}
/* End of helpers section */

/* Main kernel entry point, called by trampoline */
void start_kernel(start_info_t * si)
{
	struct timeval tp;
	static char buff[] = "00000000000000000000000000000\n";
        //Have to map Hypervisor shared info to VM kernel space 
	memcpy(&start_info, si, sizeof(*si));
        HYPERVISOR_console_io(CONSOLEIO_write,14,"\nKernel Start\n");
        HYPERVISOR_shared_info = map_shared_info(start_info.shared_info);
	
	if(gettimeofday(&tp, 0) != 0){
		HYPERVISOR_console_io(CONSOLEIO_write,15,"#: gtod FAILED\n");
	} else{
		// Test if gtod works? print usec
		convert(tp.tv_usec, 16, buff); 
		HYPERVISOR_console_io(CONSOLEIO_write,29, buff);
	}
        HYPERVISOR_console_io(CONSOLEIO_write,5,"End.\n");
	while(1);
}

