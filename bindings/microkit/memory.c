#include "bindings.h"

static uintptr_t heap_next;
static bool heap_locked;

/* Populated through the heap map's setvar_vaddr and setvar_size attributes. */
uintptr_t microkit_heap_base;
size_t microkit_heap_size;

void mem_init(void)
{
    assert(microkit_heap_base <= UINTPTR_MAX - microkit_heap_size);

    heap_next = microkit_heap_base;
    heap_locked = false;

    log(INFO, "Solo5: Microkit heap @ (0x%llx - 0x%llx)\n",
        (unsigned long long)microkit_heap_base,
        (unsigned long long)(microkit_heap_base + microkit_heap_size));
}

void mem_lock_heap(uintptr_t *start, size_t *size)
{
    uintptr_t heap_end = microkit_heap_base + microkit_heap_size;

    assert(!heap_locked);
    assert(heap_next <= heap_end);

    heap_locked = true;
    *start = heap_next;
    *size = heap_end - heap_next;
}

void *mem_ialloc_pages(size_t num)
{
    uintptr_t heap_end = microkit_heap_base + microkit_heap_size;
    size_t size;
    void *result;

    assert(!heap_locked);
    assert(num <= SIZE_MAX >> PAGE_SHIFT);

    size = num << PAGE_SHIFT;
    assert(heap_next <= heap_end);
    assert(size <= heap_end - heap_next);

    result = (void *)heap_next;
    heap_next += size;
    return result;
}
