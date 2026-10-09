
#include "bindings.h"
#include <microkit.h>

#include <sddf/network/config.h>
#include <sddf/network/constants.h>
#include <sddf/network/queue.h>

// @gt ??
// sDDF network client configuration does not expose an MTU. Use the same
// default Ethernet MTU as lib_sddf_lwip.
//
// Ref: $(SDDF)/include/sddf/network/lib_sddf_lwip.h
//
#define SOLO5_SDDF_NET_MTU 1500
_Static_assert(
    SOLO5_SDDF_NET_MTU + SOLO5_NET_HLEN <= NET_BUFFER_SIZE,
    "Solo5 Ethernet frame does not fit in an sDDF network buffer"
);

__attribute__((section(".net_client_config"), used))
static net_client_config_t net_config;

static struct mft *mft;
static net_queue_handle_t net_rx_queue;
static net_queue_handle_t net_tx_queue;
static unsigned net_handle;
static bool net_attached;

static void sddf_net_recycle_rx(net_buff_desc_t desc)
{
    desc.len = 0;
    assert(net_enqueue_free(&net_rx_queue, desc) == 0);
    if (net_require_signal_free(&net_rx_queue)) {
        net_cancel_signal_free(&net_rx_queue);
        microkit_notify(net_config.rx.id);
    }
}

void net_init(void)
{
    struct mft_entry *entry = NULL;

    // Make sure network device is explicitly requested for only once
    mft = microkit_manifest();
    for (unsigned i = 0; i < mft->entries; i++) {
        if (mft->e[i].type != MFT_DEV_NET_BASIC)
            continue;
        if (entry != NULL)
            PANIC("Only one sDDF network interface is supported", NULL);
        entry = &mft->e[i];
        net_handle = i;
    }
    if (entry == NULL)
        return;

    /* point of no return */

    if (!net_config_check_magic(&net_config))
        PANIC("Invalid sDDF network client configuration", NULL);

    net_queue_init(&net_rx_queue, net_config.rx.free_queue.vaddr,
            net_config.rx.active_queue.vaddr, net_config.rx.num_buffers);
    net_queue_init(&net_tx_queue, net_config.tx.free_queue.vaddr,
            net_config.tx.active_queue.vaddr, net_config.tx.num_buffers);
    net_buffers_init(&net_tx_queue, 0);

    /* Binds SDDF network device with manifest entry for the Solo5 network device */
    memcpy(entry->u.net_basic.mac, net_config.mac_addr.addr,
            sizeof entry->u.net_basic.mac);
    entry->u.net_basic.mtu = SOLO5_SDDF_NET_MTU;
    entry->b.data = &net_rx_queue;
    entry->attached = true;
    net_attached = true;
}

solo5_result_t solo5_net_acquire(const char *name, solo5_handle_t *handle,
                                 struct solo5_net_info *info)
{
    unsigned index;
    const struct mft_entry *e =
        mft_get_by_name(mft, name, MFT_DEV_NET_BASIC, &index);

    if (e == NULL || !e->attached)
        return SOLO5_R_EINVAL;

    *handle = index;
    info->mtu = e->u.net_basic.mtu;
    memcpy(info->mac_address, e->u.net_basic.mac, sizeof info->mac_address);
    return SOLO5_R_OK;
}

solo5_result_t solo5_net_read(solo5_handle_t handle, uint8_t *buf, size_t size,
                              size_t *read_size)
{
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_NET_BASIC);
    net_buff_desc_t desc;

    // Check if the network device is valid and attached.
    if (e == NULL || !e->attached ||
            size < (size_t)e->u.net_basic.mtu + SOLO5_NET_HLEN)
        return SOLO5_R_EINVAL;

    if (net_dequeue_active(&net_rx_queue, &desc) != 0)
        return SOLO5_R_AGAIN; /* If no data, try again later, do not block */

    // Discard invalid packet and return the RX buffer
    if (desc.len > size || desc.io_or_offset > net_config.rx_data.size ||
            desc.len > net_config.rx_data.size - desc.io_or_offset) {
        sddf_net_recycle_rx(desc);
        return SOLO5_R_EUNSPEC;
    }

    memcpy(buf, (uint8_t *)net_config.rx_data.vaddr + desc.io_or_offset,
            desc.len);
    *read_size = desc.len;
    sddf_net_recycle_rx(desc);
    return SOLO5_R_OK;
}

solo5_result_t solo5_net_write(solo5_handle_t handle, const uint8_t *buf,
                               size_t size)
{
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_NET_BASIC);
    net_buff_desc_t desc;

    // Check if the network device is valid and attached.
    if (e == NULL || !e->attached || size == 0 ||
            size > (size_t)e->u.net_basic.mtu + SOLO5_NET_HLEN ||
            size > NET_BUFFER_SIZE)
        return SOLO5_R_EINVAL;

    if (net_dequeue_free(&net_tx_queue, &desc) != 0)
        return SOLO5_R_EUNSPEC;

    if (desc.io_or_offset > net_config.tx_data.size ||
            size > net_config.tx_data.size - desc.io_or_offset) {
        desc.len = 0;
        assert(net_enqueue_free(&net_tx_queue, desc) == 0);
        return SOLO5_R_EUNSPEC;
    }

    memcpy((uint8_t *)net_config.tx_data.vaddr + desc.io_or_offset, buf, size);
    desc.len = size;
    if (net_enqueue_active(&net_tx_queue, desc) != 0) {
        desc.len = 0;
        assert(net_enqueue_free(&net_tx_queue, desc) == 0);
        return SOLO5_R_EUNSPEC;
    }

    /* Notify the TX virtualiser only if it requested a signal for new work. */
    if (net_require_signal_active(&net_tx_queue)) {
        net_cancel_signal_active(&net_tx_queue);
        microkit_notify(net_config.tx.id);
    }
    return SOLO5_R_OK;
}

solo5_handle_set_t sddf_net_poll(void)
{
    if (!net_attached)
        return 0;

    /* Return polling result when there are packets in buffer */
    if (!net_queue_empty_active(&net_rx_queue))
        return 1ULL << net_handle;

    // If no packet is here...
    // Mark the state and let the RX virtualiser know that I am ready
    net_request_signal_active(&net_rx_queue);
    
    if (!net_queue_empty_active(&net_rx_queue)) {
        net_cancel_signal_active(&net_rx_queue);
        return 1ULL << net_handle;
    }

    return 0;
}
