
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

// #define SYS_STDOUT 1

// long sys_read(long fd, void *buf, long size);
// long sys_write(long fd, const void *buf, long size);
// long sys_pread64(long fd, void *buf, long size, long pos);
// long sys_pwrite64(long fd, const void *buf, long size, long pos);

// void sys_exit_group(long status) __attribute__((noreturn));

// struct sys_timespec {
//     uint64_t tv_sec;
//     long tv_nsec;
// };

// struct sys_itimerspec {
//     struct sys_timespec it_interval;
//     struct sys_timespec it_value;
// };

// #define SYS_CLOCK_REALTIME  0
// #define SYS_CLOCK_MONOTONIC 1

// long sys_clock_gettime(const long which, void *ts);

// #define SYS_EINTR  -4
// #define SYS_EAGAIN -11

// /*
//  * Ah, the wonders of Linux ABIs...
//  */
// #if defined(__x86_64__)
// #define EPOLL_PACKED __attribute__((packed))
// #else
// #define EPOLL_PACKED
// #endif

// struct sys_epoll_event {
//     unsigned events;
//     uint64_t data;
// } EPOLL_PACKED;

// long sys_epoll_pwait(long epfd, void *events, long maxevents, long timeout,
//                      void *sigmask, long sigsetsize);

// #define SYS_TFD_TIMER_ABSTIME (1 << 0)

// long sys_timerfd_settime(long fd, long flags, const void *utmr, void *otmr);

// #define SYS_ARCH_SET_FS 0x1002

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

#define MICROKIT_HEAP_BASE 0x200000000ULL
#define MICROKIT_HEAP_SIZE 0x4000000ULL

#endif /* __MKT_BINDINGS_H__ */
