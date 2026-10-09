
#include "bindings.h"

void solo5_console_write(const char *buf, size_t size)
{
    (void)platform_puts(buf, (int)size);
}

/* solo5_exit is in exit.c */

/* solo5_abort is in abort.c */

solo5_time_t solo5_clock_monotonic(void)
{
    return sddf_timer_monotonic();
}

// @gt ?? TODO
//     This function tries to get the real time since 1970-01-01 00:00:00 UTC
//     We will leave it as unimplemented here.
solo5_time_t solo5_clock_wall(void)
{
    /* sDDF currently exposes elapsed time, but no UTC/RTC time source. */
    return sddf_timer_monotonic();
}

/* solo5_set_tls_base is in tls.c */
