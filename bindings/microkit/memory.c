#include "bindings.h"

void mem_init(void)
{
    log(INFO, "Solo5: Microkit heap @ (0x%llx - 0x%llx)\n",
        (unsigned long long)MICROKIT_HEAP_BASE,
        (unsigned long long)(MICROKIT_HEAP_BASE + MICROKIT_HEAP_SIZE));
}

void mem_lock_heap(uintptr_t *start, size_t *size)
{
    *start = MICROKIT_HEAP_BASE;
    *size = MICROKIT_HEAP_SIZE;
}

void *mem_ialloc_pages(size_t num)
{
    (void)num;
    PANIC("Microkit early page allocation is not supported", NULL);
}
