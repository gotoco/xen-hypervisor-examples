/*
 * gettimeofday() example implementation.
 * Requires the shared info page to be mapped and stored in a global variable
 *
 */
#include <stdint.h>
#include <xen/xen.h> 
#include "time.h"
#include "hypercall_dbg-x86_64.h"

#define NANOSECONDS(tsc, s) (tsc << s->vcpu_info[0].time.tsc_shift)\
        * s->vcpu_info[0].time.tsc_to_system_mul

#define RDTSC(x)     asm volatile ("RDTSC":"=A"(tsc))

int gettimeofday(struct timeval *tp, void *tzp)
{
        uint64_t tsc;
        /* Get the time values from the shared info page */
        uint32_t version, wc_version;
        uint32_t seconds, nanoseconds, system_time;
        uint64_t old_tsc;
	shared_info_t *s = HYPERVISOR_shared_info;
        /* Loop until we can read all required values from the same update */
        do
        {
                /* Spin if the time value is being updated */
                do
                {
                        wc_version = s->wc_version;
                        version = s->vcpu_info[0].time.version;
                } while( version & 1 == 1 || wc_version & 1 == 1);
               
		/* Read the values */
                seconds = s->wc_sec;
                nanoseconds = s->wc_nsec;
                system_time = s->vcpu_info[0].time.system_time;
                old_tsc = s->vcpu_info[0].time.tsc_timestamp;
        } while(
                        version != s->vcpu_info[0].time.version
                        ||
                        wc_version != s->wc_version
                        );
        /* Get the current TSC value */
        RDTSC(tsc);
        /* Get the number of elapsed cycles */
        tsc -= old_tsc;
        /* Update the system time */
        system_time += NANOSECONDS(tsc, s);
        /* Update the nanosecond time */
        nanoseconds += system_time;
        /* Move complete seconds to the second counter */
        seconds += nanoseconds / 1000000000;
        nanoseconds = nanoseconds % 1000000000;
        /* Return second and millisecond values */
        tp->tv_sec = seconds;
        tp->tv_usec = nanoseconds * 1000;
        
        return 0;
}
