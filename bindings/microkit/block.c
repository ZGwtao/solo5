#include "bindings.h"
#include <microkit.h>
#include <sddf/blk/config.h>
#include <sddf/blk/queue.h>
#include <sddf/blk/storage_info.h>

__attribute__((section(".blk_client_config"), used))
static blk_client_config_t blk_config;

static struct mft *mft;
static blk_queue_handle_t blk_queue;
static blk_storage_info_t *blk_storage_info;
static uint32_t blk_request_id;

void block_init(void)
{
    struct mft_entry *entry = NULL;

    mft = microkit_manifest();
    for (unsigned i = 0; i < mft->entries; i++) {
        if (mft->e[i].type != MFT_DEV_BLOCK_BASIC)
            continue;
        if (entry != NULL)
            PANIC("Only one sDDF block device is supported", NULL);
        entry = &mft->e[i];
    }

    if (entry == NULL)
        return;
    if (!blk_config_check_magic(&blk_config))
        PANIC("Invalid sDDF block client configuration", NULL);

    blk_queue_init(&blk_queue, blk_config.virt.req_queue.vaddr,
            blk_config.virt.resp_queue.vaddr, blk_config.virt.num_buffers);
    blk_storage_info = blk_config.virt.storage_info.vaddr;

    // @gt ?? If the block drive is dead, we need to wait forever?
    while (!blk_storage_is_ready(blk_storage_info))
        seL4_Yield();

    entry->u.block_basic.block_size = BLK_TRANSFER_SIZE;
    entry->u.block_basic.capacity =
        blk_storage_info->capacity * BLK_TRANSFER_SIZE;
    entry->attached = true;
}

solo5_result_t solo5_block_acquire(const char *name, solo5_handle_t *handle,
                                   struct solo5_block_info *info)
{
    unsigned index;
    const struct mft_entry *e =
        mft_get_by_name(mft, name, MFT_DEV_BLOCK_BASIC, &index);

    if (e == NULL || !e->attached)
        return SOLO5_R_EINVAL;

    *handle = index;
    info->capacity = e->u.block_basic.capacity;
    info->block_size = e->u.block_basic.block_size;
    return SOLO5_R_OK;
}

static solo5_result_t sddf_block_request(blk_req_code_t code,
        solo5_off_t offset, void *buf)
{
    blk_resp_status_t status;
    uint16_t success_count;
    uint32_t response_id;
    uint32_t request_id = blk_request_id++;

    if (code == BLK_REQ_WRITE)
        memcpy(blk_config.data.vaddr, buf, BLK_TRANSFER_SIZE);

    // @gt ?? abort only when the client mess up with the protocol?
    int err = blk_enqueue_req(&blk_queue, code, 0,
            offset / BLK_TRANSFER_SIZE, 1, request_id);
    assert(!err);
    microkit_notify(blk_config.virt.id);

    /* Solo5 requires blocking block I/O */
    while (blk_queue_empty_resp(&blk_queue)) {
        seL4_Word badge;
        (void)seL4_Recv(MICROKIT_INPUT_CAP, &badge, MICROKIT_REPLY_CAP);
    }

    err = blk_dequeue_resp(&blk_queue, &status, &success_count,
            &response_id);
    assert(!err);

    if (response_id != request_id || status != BLK_RESP_OK ||
            success_count != 1)
        return SOLO5_R_EUNSPEC;

    if (code == BLK_REQ_READ)
        memcpy(buf, blk_config.data.vaddr, BLK_TRANSFER_SIZE);
    return SOLO5_R_OK;
}

static solo5_result_t sddf_block_io(solo5_handle_t handle, solo5_off_t offset,
        void *buf, size_t size, blk_req_code_t code)
{
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_BLOCK_BASIC);

    if (e == NULL || !e->attached || size != e->u.block_basic.block_size ||
            offset % e->u.block_basic.block_size != 0 ||
            e->u.block_basic.capacity < size ||
            offset > e->u.block_basic.capacity - size)
        return SOLO5_R_EINVAL;

    return sddf_block_request(code, offset, buf);
}

solo5_result_t solo5_block_read(solo5_handle_t handle, solo5_off_t offset,
                                uint8_t *buf, size_t size)
{
    return sddf_block_io(handle, offset, buf, size, BLK_REQ_READ);
}

solo5_result_t solo5_block_write(solo5_handle_t handle, solo5_off_t offset,
                                 const uint8_t *buf, size_t size)
{
    if (blk_storage_info != NULL && blk_storage_info->read_only)
        return SOLO5_R_EUNSPEC;

    return sddf_block_io(handle, offset, (void *)(uintptr_t)buf, size,
            BLK_REQ_WRITE);
}
