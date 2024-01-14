#include <common.h>
#include <inttypes.h>

#define HB_MIN (1 << 4)
#define HB_MAX (1 << 16)
#define MB (1 << 20)

// 这是线段树数组，每一个元素是char类型，占一个字节
#define HB_ARRAY_SIZE (2 * HB_MAX / HB_MIN + 8)
#define HB_CONT_SIZE (HB_MAX)

/*
     | ------ heap_block --------| | ------array-------||-------content--------|
*/

static size_t HB_number;     // number of HB
static void *HB_struct_base; // basic addr of HB head
static void *HB_array_base;  // basic addr of HB content
static void *HB_cont_base;

typedef struct heap_block
{
  char *array; // 起始头部
  void *cont;  // 内容
  char stat;
} heap_block;

void hb_init(struct heap_block *hb, void *array, void *cont)
{
  hb->array = (char *)(array);
  hb->cont = cont;
  hb->stat = 0;

  memset(hb->array, 0, HB_ARRAY_SIZE);
  memset(hb->cont, 0, HB_CONT_SIZE);
}

void kinit()
{
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Heap size: %d\n", pmsize);
  HB_number = pmsize / (sizeof(heap_block) + HB_ARRAY_SIZE + HB_CONT_SIZE);
  printf("Block number : %d\n", HB_number);
  HB_struct_base = heap.start;
  HB_array_base = heap.start + HB_number * sizeof(struct heap_block);
  HB_cont_base = HB_array_base + HB_number * HB_ARRAY_SIZE;
  printf("HB_struct_base : %p \t HB_array_base : %p \t HB_cont_base : %p\n", HB_struct_base, HB_array_base, HB_cont_base);

  for (int i = 0; i < HB_number; i++)
  {
    hb_init((heap_block *)(HB_struct_base + i * sizeof(heap_block)),
            HB_array_base + i * HB_ARRAY_SIZE,
            HB_cont_base + i * HB_CONT_SIZE);
  }
}

// 先把 size 转为 2 的幂
size_t conv2pow(size_t size)
{
  if (size <= 16)
  {
    return 16;
  }
  size_t res = 16;
  while (res < size)
  {
    res *= 2;
  }
  return res;
}

static void *kalloc(size_t size)
{
  size = conv2pow(size);
  if (size >= 16 * MB)
    return NULL;

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
