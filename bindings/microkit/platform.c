
#include "bindings.h"
#include <microkit.h>

static const char cmdline[] = "Hello_Solo5";
static const struct mft *mft;


void platform_init(const void *arg)
{
    (void)arg;
    assert(sddf_serial_init() == 0);
    assert(sddf_timer_init() == 0);
}

const char *platform_cmdline(void)
{
    return cmdline;
}

uint64_t platform_mem_size(void)
{
    return MICROKIT_HEAP_BASE + MICROKIT_HEAP_SIZE;
}

const struct mft *microkit_manifest(void)
{
    return mft;
}

void platform_exit(int status, void *cookie __attribute__((unused)))
{
    // @gt ?? no_return, let the PD fault at a 'status' address
    uintptr_t fault_addr = (unsigned int)status;
#if defined(__aarch64__)
    __asm__ __volatile__("str xzr, [%0]" : : "r"(fault_addr) : "memory");
#elif defined(__x86_64__)
    __asm__ __volatile__("movl $0, (%0)" : : "r"(fault_addr) : "memory");
#else
#error Unsupported architecture
#endif
    __builtin_unreachable();
}

int platform_puts(const char *buf, int n)
{
    int written = sddf_serial_write(buf, n);

    /* Keep early initialisation and configuration failures observable. */
    if (written < 0) {
        for (int i = 0; i < n; i++)
            seL4_DebugPutChar(buf[i]);
        return n;
    }
    return written;
}

int platform_set_tls_base(uint64_t base)
{
#if defined(__x86_64__)
    /* seL4 enables CR4.FSGSBASE for native x86_64 PDs. */
    __asm__ __volatile__("wrfsbase %0" : : "r"(base));
    return 0;
#elif defined(__aarch64__)
    __asm__ __volatile__("msr tpidr_el0, %0" : : "r"(base));
    return 0;
#else
#error Unsupported architecture
#endif
}
