#include "bindings.h"
#include <microkit.h>

/* libsel4 accesses the IPC buffer through this runtime-provided pointer. */
seL4_IPCBuffer *__sel4_ipc_buffer =
    (seL4_IPCBuffer *)(seL4_UserVSpaceTop & ~((1ULL << seL4_PageBits) - 1ULL));

/* Symbols patched by the Microkit image tool for every native PD. */
char microkit_name[64];
bool microkit_passive;
seL4_Word microkit_irqs;
seL4_Word microkit_notifications;
seL4_Word microkit_pps;
seL4_Word microkit_ioports;

void microkit_dbg_putc(int c)
{
    seL4_DebugPutChar(c);
}

void microkit_dbg_puts(const char *s)
{
    while (*s != '\0')
        seL4_DebugPutChar(*s++);
}

void microkit_dbg_put32(seL4_Uint32 value)
{
    char digits[10];
    unsigned int n = 0;

    do {
        digits[n++] = '0' + value % 10;
        value /= 10;
    } while (value != 0);
    while (n != 0)
        seL4_DebugPutChar(digits[--n]);
}
