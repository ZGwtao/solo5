
#include "bindings.h"

static const char *cmdline;
static uint64_t mem_size;

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
    // TODO
}

int platform_puts(const char *buf, int n)
{
    // TODO
    return n;
}

int platform_set_tls_base(uint64_t base)
{
#if defined(__x86_64__)
    // TODO
    return 0;
#elif defined(__aarch64__)
    // TODO
    return 0;
#else
#error Unsupported architecture
#endif
}

// TODO, we probably need a cooperative scheduler somewhere?
// or should this be provided by the LibOS instead of the bindings?
// Should bindings layer provide scheduling interfaces?
