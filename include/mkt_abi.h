
/*
 * mkt_abi.h: microkit guest/tender interface definitions.
 *
 * This header file must be kept self-contained with no external dependencies
 * other than C99 headers.
 */

#ifndef MKT_ABI_H
#define MKT_ABI_H

#include <stddef.h>
#include <stdint.h>
#include "elf_abi.h"

/*
 * ABI version. This must be incremented before cutting a release of Solo5 if
 * any material changes are made to the interfaces or data structures defined
 * in this file.
 */

#define MKT_ABI_VERSION 1

/*
 * Lowest virtual address at which guests can be loaded.
 */
#define MKT_GUEST_MIN_BASE 0x100000

/*
 * A pointer to this structure is passed by the tender as the sole argument to
 * the guest entrypoint.
 */
struct mkt_boot_info {
    uint64_t mem_size; /* Memory size in bytes */
    uint64_t kernel_end; /* Address of end of kernel */
    const char *cmdline; /* Address of command line (C string) */
    const void *mft; /* Address of application manifest */
    // TODO
    /* epoll() set for yield() */
    // TODO
    /* internal timerfd for yield() */
};

/*
 * Identifier (data.u64) for internal timerfd in epoll() set.
 */
#define MKT_INTERNAL_TIMERFD (~1U)

/*
 * The lowest memory address at which we can mmap() memory on the host. See
 * mkt_main.c for an explanation.
 */
#define MKT_HOST_MEM_BASE 0x10000

/*
 * Guest low memory layout.
 */
#define MKT_BOOT_INFO_BASE (MKT_HOST_MEM_BASE + 0x1000)

/*
 * Maximum size of guest command line, including the string terminator.
 */
#define MKT_CMDLINE_SIZE 8192

#endif /* MKT_ABI_H */
