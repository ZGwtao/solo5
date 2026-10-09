
#include "bindings.h"
#include <microkit.h>

static const char cmdline[] = "Hello_Solo5";
static const struct mft *mft;


void platform_init(const void *arg)
{
    (void)arg;
    // TODO
    // try initialising all sddf-based device here
    // (1) timer, (2) serial, (3) network, (4) block
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
    (void)status;
    for (;;) {
        seL4_Word badge;
        (void)seL4_Recv(MICROKIT_INPUT_CAP, &badge, MICROKIT_REPLY_CAP);
    }
}

int platform_puts(const char *buf, int n)
{
    for (int i = 0; i < n; i++)
        seL4_DebugPutChar(buf[i]);
    return n;
}

int platform_set_tls_base(uint64_t base)
{
#if defined(__x86_64__)
    // TODO
    return 0;
#elif defined(__aarch64__)
    __asm__ __volatile__("msr tpidr_el0, %0" : : "r"(base));
    return 0;
#else
#error Unsupported architecture
#endif
}
