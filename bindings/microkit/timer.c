#include "bindings.h"

__attribute__((section(".timer_client_config"), used))
static timer_client_config_t timer_config;

static sddf_channel timer_channel;
static bool timer_ready;

int sddf_timer_init(void)
{
    if (!timer_config_check_magic(&timer_config))
        return -1;

    timer_channel = timer_config.driver_id;
    timer_ready = true;
    return 0;
}

solo5_time_t sddf_timer_monotonic(void)
{
    if (!timer_ready)
        PANIC("sDDF timer is not initialized", NULL);
    return sddf_timer_time_now(timer_channel);
}
