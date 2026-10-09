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
    