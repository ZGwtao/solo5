
#include "bindings.h"

static const struct mft *mft;

void net_init(struct mkt_boot_info *bi)
{
    mft = bi->mft;
    // TODO
}

solo5_result_t solo5_net_acquire(const char *name, solo5_handle_t *handle,
                                 struct solo5_net_info *info)
{
    unsigned index;
    const struct mft_entry *e =
        mft_get_by_name(mft, name, MFT_DEV_NET_BASIC, &index);
    if (e == NULL)
        return SOLO5_R_EINVAL;
    assert(e->attached);

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
    if (e == NULL)
        return SOLO5_R_EINVAL;

    // TODO
    long nbytes = 0;
    if (nbytes < 0) {
        if (nbytes == SYS_EAGAIN)
            return SOLO5_R_AGAIN;
        else
            return SOLO5_R_EUNSPEC;
    }

    *read_size = (size_t)nbytes;
    return SOLO5_R_OK;
}

solo5_result_t solo5_net_write(solo5_handle_t handle, const uint8_t *buf,
                               size_t size)
{
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_NET_BASIC);
    if (e == NULL)
        return SOLO5_R_EINVAL;

    // TODO
    long nbytes = 0;

    return (nbytes == (int)size) ? SOLO5_R_OK : SOLO5_R_EUNSPEC;
}
