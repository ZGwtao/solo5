
#include "bindings.h"

// @gt ?? Should we support ppc??
static volatile seL4_Word pending_notifications;

void solo5_yield(solo5_time_t deadline, solo5_handle_set_t *ready_set)
{
    solo5_time_t now;
    const seL4_Word timer_mask = sddf_timer_event_mask();

    if (ready_set != NULL)
        *ready_set = 0;

    /*
     * TODO: Poll block and network queues for pending_notifications, add
     * ready handles to ready_set, and clear only the notifications handled.
     */

    for (;;) {
        seL4_Word badge;

        // @gt ?? is deadline always a monotonic time?
        now = sddf_timer_monotonic();
        if (deadline <= now)
            return;
        // Wait until a given timeout event arrives.
        sddf_timer_set_timeout_ns(deadline - now);
        (void)seL4_Recv(MICROKIT_INPUT_CAP, &badge, MICROKIT_REPLY_CAP);

        /* Preserve every non-timer event consumed by seL4_Recv(). */
        pending_notifications |= badge & ~timer_mask;

        if ((badge & timer_mask) != 0)
            return;

        /* Non-timer events remain pending in software while waiting. */
    }
}
