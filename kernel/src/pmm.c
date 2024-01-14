#include <common.h>
#include <inttypes.h>

#define HB_MIN (1 << 4)
#define HB_MAX (1 << 16)

typedef struct heap_block
{

} heap_block;

void kinit()
{
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Heap size: %d\n", pmsize);
}

static void *kalloc(size_t size)
{
  return NULL;
}

static void kfree(void *ptr)
{
}

static void pmm_init()
{
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  kinit();
}

MODULE_DEF(pmm) = {
    .init = pmm_init,
    .alloc = kalloc,
    .free = kfree,
};
