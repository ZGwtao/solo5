
/*
 * bindings.h: Solo5 bindings, microkit implementation additions.
 *
 * This header file includes (supersedes) the common bindings.h for the microkit
 * implementation.
 */

#ifndef __MKT_BINDINGS_H__
#define __MKT_BINDINGS_H__

// @gt ?? assert conflict?
#define _assert_fail sddf_assert_fail
#include <sddf/serial/config.h>
#include <sddf/serial/queue.h>
#include <sddf/timer/client.h>
#include <sddf/timer/config.h>
#undef assert
#undef _assert_fail

#include "../bindings.h"
#include "mkt_abi.h"
#include <sel4/sel4.h>

void block_init(void);
void net_init(void);
struct mft *microkit_manifest(void);

#define MICROKIT_INPUT_CAP 1
#define MICROKIT_REPLY_CAP 4

int sddf_serial_init(void);
int sddf_serial_write(const char *buf, int n);
int sddf_timer_init(void);
solo5_time_t sddf_timer_monotonic(void);
void sddf_timer_set_timeout_ns(solo5_time_t timeout);


solo5_handle_set_t sddf_net_poll(void);

/* Patched from the heap mapping by the Microkit image tool. */
extern uintptr_t microkit_heap_base;
extern size_t microkit_heap_size;

#endif /* __MKT_BINDINGS_H__ */
