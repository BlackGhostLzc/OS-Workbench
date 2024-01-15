#include <common.h>
#include <inttypes.h>

#define HB_MIN (1 << 4)
#define HB_MAX (1 << 16)
#define MB (1 << 20)

// 这是线段树数组，每一个元素是char类型，占一个字节
#define HB_ARRAY_SIZE (2 * HB_MAX / HB_MIN + 8)
#define HB_CONT_SIZE (HB_MAX)

/*
  | ------ heap_block(结构体数组) --------| | ------ array(线段树) -------| |-------content(实际内容)--------|
*/

static size_t HB_number;     // number of HB
static void *HB_struct_base; // basic addr of HB head
static void *HB_array_base;  // basic addr of HB array
static void *HB_cont_base;   // basic addr of HB content

typedef struct heap_block
{
  char *array; // 起始头部
  void *cont;  // 内容
  char stat;   // 0 普通的小块分配使用
               // 1 > HB_MAX块的起始        array[1] = 1
               // 2         中间            array[1] = 1
               // 3         最后            array[1] = 1
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

int lowbit(int x)
{
  return x & (-x);
}

int highbit(int x)
{
  int ret = x;
  while (x - lowbit(x) != 0)
  {
    ret = ret - lowbit(x);
    x = x - lowbit(x);
  }
  return ret;
}

size_t idx2size(int idx)
{
  // 1 -> HB_MAX              1
  // 2, 3   HB_MAX / 2        10  11
  // 4 5 6 7   HB_MAX / 4     100 101 110 111
  // 最高位的 1
  size_t size = HB_MAX;
  while (idx >>= 1)
  {
    size >>= 1;
  }
  return size;
}

void *idx2addr(heap_block *hb, int idx)
{
  // 1  ->   [0, 32)
  // 10 ->   [0, 16)      11->[16, 32)
  // 100 ->  [0, 8)   101 -> [8, 16)   110 -> [16, 24)   111 -> [25, 32)
  assert(idx > 0 && idx <= HB_ARRAY_SIZE - 8 + 1);
  // 找到最高位
  int high = highbit(idx);
  assert(high != 0);
  size_t size = idx2size(idx);
  assert(size <= HB_MAX && size >= 16);
  return hb->cont + (idx - high) * size;
}

// 更新操作
/* array
   0:  完全未被使用
   1:  全部被使用，且是被整体分配
   2： 部分被使用，还有剩余空间
   3： 全部被使用，是多次分配的结果
*/
void update_array(heap_block *hb, int parent)
{
  size_t le = parent << 1, ri = le + 1;

  // if both free
  if (hb->array[le] == 0 && hb->array[ri] == 0)
  {
    hb->array[parent] = 0;
  }
  // if both full
  else if (hb->array[le] & 1 && hb->array[ri] & 1)
  {
    hb->array[parent] = 3;
  }
  // if both not full
  else
  {
    hb->array[parent] = 2;
  }
}

/*
  array[1] :[0, HB_MAX - 1]
  array[2] :[0, HB_MAX/2 - 1]        array[3] :[HB_MAX/2, HB_MAX - 1]
  ...............
*/

// 记住：array[i]表示的是一段区间！！！
// array[i] = 0      完全未被使用
// array[i] = 1      全部被使用
// array[i] = 2      使用了一部分但不是全部
// array[i] = 3      零星分配，使用了全部
int find_hb(int idx, heap_block *hb, int block_size, int size)
{
  // 这段区间已经全部被占用了 或者大小不满足
  if (hb->array[idx] == 1 || hb->array[idx] == 3 || block_size < size)
  {
    return 0;
  }

  if (hb->array[idx] == 0 && block_size == size)
  {
    hb->array[idx] = 1;
    return idx;
  }

  if (idx2size(idx) == HB_MIN)
  {
    return 0;
  }

  int l = idx * 2;
  int r = idx * 2 + 1; // 两棵子树
  int ret = 0;
  ret = find_hb(l, hb, block_size / 2, size);
  if (ret > 0)
  {
    update_array(hb, idx);
    return ret;
  }
  ret = find_hb(r, hb, block_size / 2, size);
  if (ret > 0)
  {
    update_array(hb, idx);
    return ret;
  }

  return 0;
}

static void *kalloc(size_t size)
{
  size = conv2pow(size);
  printf("Allocating %d bytes \n", (int)(size));
  if (size >= 16 * MB)
  {
    return NULL;
  }

  if (size < HB_MAX)
  {
    for (int i = 0; i < HB_number; i++)
    {
      heap_block *hb = (heap_block *)(HB_struct_base + i * sizeof(heap_block));
      int idx = find_hb(1, hb, HB_MAX, size);
      if (idx > 0)
      {
        // idx 转内存地址
        return idx2addr(hb, idx);
      }
    }
  }

  else
  {
    for (int i = 0; i < HB_number; i++)
    {
      int j = i;
      heap_block *hb = (heap_block *)(HB_struct_base + i * sizeof(heap_block));

      while (hb->array[1] == 0 && (j - i + 1) * HB_MAX < size && j < HB_number)
      {
        j++;
        hb = (heap_block *)(HB_struct_base + j * sizeof(heap_block));
      }

      if (j == HB_number)
      {
        return NULL;
      }

      if ((j - i + 1) * HB_MAX >= size)
      {
        for (int k = i; k <= j; k++)
        {
          hb = (heap_block *)(HB_struct_base + k * sizeof(heap_block));
          hb->array[1] = 1; // 整体占用
          if (k == i)
          {
            hb->stat = 1; // beginning
          }
          else if (k == j)
          {
            hb->stat = 3; // ending
          }
          else if (i < k && k < j)
          {
            hb->stat = 2; // internal
          }
        }
        hb = (heap_block *)(HB_struct_base + i * sizeof(heap_block));
        return hb->cont;
      }
    }
  }

  printf("Allocating memory failed\n");
  return NULL;
}

static void kfree(void *ptr)
{
  if (ptr == NULL)
  {
    return;
  }
  // 如何根据 ptr 地址找到 Idx ?
  // 还有如果是连续分配的，那先要找 heap_block
  assert((uintptr_t)(ptr) % HB_MIN == 0);
  uintptr_t offset = (uintptr_t)(ptr) - (uintptr_t)(HB_cont_base);
  int id = (offset + 1) / HB_MAX; // [0, HB_MAX - 1]   [HB_MAX, 2*HB_MAX - 1]
  heap_block *hb = (heap_block *)(HB_struct_base + id * sizeof(heap_block));
  assert((uintptr_t)(ptr) >= (uintptr_t)(hb->cont) && (uintptr_t)(ptr) < (uintptr_t)(hb->cont + HB_MAX));

  if (hb->stat != 0)
  {
    // 连续分配
    assert(ptr != hb->cont);
    while (1)
    {
      hb->array[1] = 0;
      hb->stat = 0;
      if (hb->stat == 3)
      {
        break;
      }
      hb++;
      assert((uintptr_t)(hb) < (uintptr_t)(HB_array_base));
    }
  }
  else
  {
    // 不是连续分配
    // 思路：找到该块中array起始地址为ptr的 idx集合，有且仅有一个为 1 ,也可以知道分配块的大小
    uintptr_t off = (uintptr_t)(ptr) - (uintptr_t)(hb->cont);

    // [1], [2 , 3]  ......... [HB_MAX/HB_MIN, HB_MAX/HB_MIN+1 ............ 2*HB_MAX/HB_MIN - 1]
    // HB_MAX = 2<<12, max_idx是array中起始地址为ptr的最大索引
    int max_idx = (1 << 12) + off / HB_MIN;

    // 自底向上找，知道找到第一个 array[x] 为 1 的索引,且该节点必须是父亲的左儿子,左儿子是偶数
    while (hb->array[max_idx] != 1)
    {
      if (max_idx % 2)
      {
        printf("Not found\n");
        break;
      }
      else
      {
        max_idx = max_idx / 2;
      }
    }

    assert(hb->array[max_idx] == 1);
    hb->array[max_idx] = 0;
    printf("Free memory size is %d\n", idx2size(max_idx));

    // 一直向上更新 array 数组
    int x = max_idx;
    while (x)
    {
      update_array(hb, x);
      x /= 2;
    }
  }
}

static void pmm_init()
{
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap", pmsize >> 20);
  printf("[%p, %p)\n", (uintptr_t)heap.start, (uintptr_t)heap.end);
  kinit();
}

MODULE_DEF(pmm) = {
    .init = pmm_init,
    .alloc = kalloc,
    .free = kfree,
};

// test 函数
void pmm_test()
{
  void *addr_array[1000];
  while (1)
  {
    printf("CPU #%d\n allocating .....\n", cpu_current());
    for (int i = 0; i < 1000; i++)
    {
      addr_array[i] = pmm->alloc(rand());
    }

    // for (int i = 0; i < 100; i++)
    // {
    //   printf("%p \t", addr_array[i]);
    // }

    for (int i = 0; i < 1000; i++)
    {
      kfree(addr_array[i]);
    }

    int x = 100000000;
    while (x)
    {
      x--;
    }
  }
}
