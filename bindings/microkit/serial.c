#include "bindings.h"
#include <microkit.h>

/* Solo5 and sDDF use incompatible internal _assert_fail() signatures. */
#undef assert
#define _assert_fail sddf_assert_fail
#include <sddf/serial/config.h>
#include <sddf/serial/queue.h>
#undef _assert_fail
#undef assert

void sddf_assert_fail(const char *assertion, const char *file,
        unsigned int line, const char *function)
{
    (void)file;
    (void)line;
    (void)function;
    PANIC(assertion, NULL);
}

// Also declare in .lds to not make it an orphan section.
__attribute__((section(".serial_client_config"), used))
static serial_client_config_t serial_config;

// @gt ?? solo5 needs tx only?
static serial_queue_handle_t serial_tx_queue;
static uint32_t serial_tx_tail;
// Denotes whether a serial device is initialised
static bool serial_ready;

int sddf_serial_init(void)
{
    if (!serial_config_check_magic(&serial_config))
        return -1;

    serial_queue_init(&serial_tx_queue, serial_config.tx.queue.vaddr,
            serial_config.tx.data.size, serial_config.tx.data.vaddr);
    serial_tx_tail = serial_tx_queue.queue->tail;
    serial_ready = true;
    return 0;
}

int sddf_serial_write(const char *buf, int n)
{
    int written = 0;
    uint32_t initial_tail;

    if (!serial_ready)
        return -1;

    // Save the initial local tail to detect whether any data is enqueued.
    initial_tail = serial_tx_tail;
    while (written < n) {
        char c = buf[written];

        if (c == '\n') {
            if (serial_enqueue_local(&serial_tx_queue, &serial_tx_tail,
                        '\r') != 0)
                break;
        }
        if (serial_enqueue_local(&serial_tx_queue, &serial_tx_tail, c) != 0)
            break;
        written++;
    }

    // Make enqueued data visible to virtualiser
    if (serial_tx_tail != initial_tail) {
        serial_update_shared_tail(&serial_tx_queue, serial_tx_tail);
        microkit_notify(serial_config.tx.id);
    }

    return written;
}
