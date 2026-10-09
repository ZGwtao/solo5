
#include "bindings.h"
#include "../crt_init.h"
#include "version.h"

#if defined(__aarch64__)
/*
 * The common Solo5 crt_init_ssp() reads CNTVCT_EL0, which seL4 does
 * not expose to native PDs in this configuration. Use the exported
 * physical counter, CNTPCT_EL0, instead.
 */
__attribute__((always_inline)) static inline void microkit_crt_init_ssp(void)
{
    uint64_t ticks0;
    uint64_t ticks1;

    __asm__ __volatile__("mrs %0, cntpct_el0" : "=r"(ticks0));
    __asm__ __volatile__("mrs %0, cntpct_el0" : "=r"(ticks1));
    // generate stack canary for basic stack-smashing
    // @gt ?? this is not cryptographically safe
    SSP_GUARD_SYMBOL = ticks0 + (ticks1 << 32UL);
    SSP_GUARD_SYMBOL &= ~(uintptr_t)0xff00;
}
#else
#define microkit_crt_init_ssp crt_init_ssp
#endif

void _start(void)
{
    // @gt ?? replace 'crt_init_ssp' for stack canary
    microkit_crt_init_ssp();
    crt_init_tls();

    static struct solo5_start_info si;

    platform_init(NULL);
    si.cmdline = cmdline_parse(platform_cmdline());

    log(INFO, "            |      ___|\n");
    log(INFO, "  __|  _ \\  |  _ \\ __ \\\n");
    log(INFO, "\\__ \\ (   | | (   |  ) |\n");
    log(INFO, "____/\\___/ _|\\___/____/\n");
    log(INFO, "Solo5: Bindings version %s\n", SOLO5_VERSION);

    mem_init();
    block_init();
    net_init();

    mem_lock_heap(&si.heap_start, &si.heap_size);
    solo5_exit(solo5_app_main(&si));
}

/*
 * Place the .interp section in this module, as it comes first in the link
 * order.
 */
DECLARE_ELF_INTERP

/*
 * The "ABI1" Solo5 ELF note is declared in this module.
 */
ABI1_NOTE_DECLARE_BEGIN{.abi_target = MKT_ABI_TARGET,
                        .abi_version = MKT_ABI_VERSION} ABI1_NOTE_DECLARE_END

    /*
     * Pretend that we are an OpenBSD executable. See elf_abi.h for details.
     */
    DECLARE_OPENBSD_NOTE
