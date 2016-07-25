#include <stdint.h>
#include <xen.h>
#include <grant_table.h>
#include "hypercalls-x86_64.h"

/* Some static space for the stack */
char stack[8192];

uint32_t shared_page[32]; 
grant_entry_t * grant_table;

void convert(long num, int base, char buff[21]) {
    char *nums = "0123456789abcdef";
    char *ptr;
    ptr = &buff[21];

    do {
        *--ptr = nums[num%base];
        num /= base;
    } while(num != 0);
}
//HACK here!! 
// To make simple Friend have to be spawned after this domain, 
//so it SELFID should be +1 (Don't spawn other VMs in the middle).
domid_t DOMID_FRIEND = DOMID_SELF+1;

void offer_page()
{
        uint16_t flags;
        /* Create the grant table */
        gnttab_setup_table_t setup_op;

        setup_op.dom = DOMID_SELF;
        setup_op.nr_frames = 1;
        setup_op.frame_list = (uint64_t*)grant_table;

        HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup_op, 1);

        /* Offer the grant */
        grant_table[0].domid = DOMID_FRIEND;
        grant_table[0].frame = *shared_page >> 12;
        flags = GTF_permit_access & GTF_reading & GTF_writing;
        grant_table[0].flags = flags;
}


/* Main kernel entry point, called by trampoline */
void start_kernel(start_info_t * start_info)
{
	static char buff[21];
	for(int i=0; i<21; i++) buff[i] = '0';

	HYPERVISOR_console_io(CONSOLEIO_write,10,"Offering.\n");
	offer_page();

	// // copy to bufner
	//convert(tp.tv_usec, 10, buff); 
	//HYPERVISOR_console_io(CONSOLEIO_write,21, buff);

	while(1);
}

