
#include "bindings.h"

void solo5_yield(solo5_time_t deadline, solo5_handle_set_t *ready_set)
{
    seL4_Word badge;

    (void)deadline;
    if (ready_set != NULL)
        *ready_set = 0;

    (void)seL4_Recv(MICROKIT_INPUT_CAP, &badge, MICROKIT_REPLY_CAP);
}
