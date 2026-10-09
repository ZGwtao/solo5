
#include "bindings.h"
#include <microkit.h>

void solo5_yield(solo5_time_t deadline, solo5_handle_set_t *ready_set)
{
    solo5_time_t now;

    if (ready_set != NULL)
        *ready_set = 0;

    for (;;) {
        seL4_Word badge;

        // "ready" contains a bitmap of ready network handles,
        // which will be returned to the LibOS via "ready_set".
        // The LibOS can use it to implement its cooperative scheduler.
        solo5_handle_set_t ready = sddf_net_poll();

        if (ready != 0) {
            if (ready_set != NULL)
                *ready_set = ready;
            return;
        }

        // "deadline" is an absolute time in the monotonic clock domain.
        now = sddf_timer_monotonic();
        if (deadline <= now)
            return;

        // Wait until the timeout or another device event arrives.
        sddf_timer_set_timeout_ns(deadline - now);
        (void)seL4_Recv(MICROKIT_INPUT_CAP, &badge, MICROKIT_REPLY_CAP);
        (void)badge;
    }
}
