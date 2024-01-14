#include <common.h>
#include <inttypes.h>

#define HB_MIN (1 << 4)
#define HB_MAX (1 << 16)

// 这是线段树数组，每一个元素是char类型，占一个字节
#define HB_HEAD_SIZE (2 * HB_MAX / HB_MIN + 8)
#define HB_CONT_SIZE (HB_MAX)

static size_t HB_number;   // number of HB
static void *HB_head_base; // basic addr of HB head
static void *HB_cont_base; // basic addr of HB content

typedef struct heap_block
{
  char *head; // 起始头部
  void *cont; // 内容
  char stat;  // 信息
} heap_block;

void kinit()
{
  printf("HB_HEAD size : %d\n", HB_HEAD_SIZE);
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Heap size: %d\n", pmsize);
  HB_number = pmsize / (sizeof(heap_block) + HB_HEAD_SIZE + HB_CONT_SIZE);
  printf("Block number : %d\n", HB_number);
  HB_head_base = heap.start;
  HB_cont_base = HB_head_base + sizeof(heap_block) * HB_number;
  printf("HB_head_base : %p \t HB_cont_base : %p\n", HB_head_base, HB_cont_base);
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
