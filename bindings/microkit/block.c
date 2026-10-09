
#include "bindings.h"

static const struct mft *mft;

void block_init(void)
{
    mft = microkit_manifest();
}

solo5_result_t solo5_block_acquire(const char *name, solo5_handle_t *handle,
                                   struct solo5_block_info *info)
{
    unsigned index;
    const struct mft_entry *e =
        mft_get_by_name(mft, name, MFT_DEV_BLOCK_BASIC, &index);
    if (e == NULL)
        return SOLO5_R_EINVAL;
    assert(e->attached);

    *handle = index;
    info->capacity = e->u.block_basic.capacity;
    info->block_size = e->u.block_basic.block_size;
    return SOLO5_R_OK;
}

solo5_result_t solo5_block_read(solo5_handle_t handle, solo5_off_t offset,
                                uint8_t *buf, size_t size)
{
    (void)buf;
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_BLOCK_BASIC);
    if (e == NULL)
        return SOLO5_R_EINVAL;

    if (size != e->u.block_basic.block_size)
        return SOLO5_R_EINVAL;
    if (offset & (e->u.block_basic.block_size - 1))
        return SOLO5_R_EINVAL;
    if (offset > (e->u.block_basic.capacity - e->u.block_basic.block_size))
        return SOLO5_R_EINVAL;

    // TODO
    long nbytes = 0;

    return (nbytes == (int)size) ? SOLO5_R_OK : SOLO5_R_EUNSPEC;
}

solo5_result_t solo5_block_write(solo5_handle_t handle, solo5_off_t offset,
                                 const uint8_t *buf, size_t size)
{
    (void)buf;
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_BLOCK_BASIC);
    if (e == NULL)
        return SOLO5_R_EINVAL;

    if (size != e->u.block_basic.block_size)
        return SOLO5_R_EINVAL;
    if (offset & (e->u.block_basic.block_size - 1))
        return SOLO5_R_EINVAL;
    if (offset > (e->u.block_basic.capacity - e->u.block_basic.block_size))
        return SOLO5_R_EINVAL;

    // TODO
    long nbytes = 0;

    return (nbytes == (int)size) ? SOLO5_R_OK : SOLO5_R_EUNSPEC;
}
