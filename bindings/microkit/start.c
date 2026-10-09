
#include "bindings.h"
#include "../crt_init.h"
#include "version.h"

void _start(void *arg)
{
    crt_init_ssp();
    crt_init_tls();

    static struct solo5_start_info si;

    platform_init(arg);
    si.cmdline = cmdline_parse(platform_cmdline());

    log(INFO, "            |      ___|\n");
    log(INFO, "  __|  _ \\  |  _ \\ __ \\\n");
    log(INFO, "\\__ \\ (   | | (   |  ) |\n");
    log(INFO, "____/\\___/ _|\\___/____/\n");
    log(INFO, "Solo5: Bindings version %s\n", SOLO5_VERSION);

    mem_init();
    block_init(arg);
    net_init(arg);

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
ABI1_NOTE_DECLARE_BEGIN{.abi_target = SPT_ABI_TARGET,
                        .abi_version = SPT_ABI_VERSION} ABI1_NOTE_DECLARE_END

    /*
     * Pretend that we are an OpenBSD executable. See elf_abi.h for details.
     */
    DECLARE_OPENBSD_NOTE
