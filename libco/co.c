#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define K 1024
#define STACK_SIZE (64 * K)

static inline void stack_switch_call(void *sp, void *entry, void *arg)
{
  asm volatile(
#if __x86_64__
      "movq %%rcx, 0(%0); movq %%rax, 8(%0); movq %0, %%rsp; movq %2, %%rdi; call *%1"
      :
      : "b"((uintptr_t)sp - 16), "d"((uintptr_t)entry), "a"((uintptr_t)arg)
#else
      "movl %%ecx, 4(%0); movl %0, %%esp; movl %2, 0(%0); call *%1"
      :
      : "b"((uintptr_t)sp - 8), "d"((uintptr_t)entry), "a"((uintptr_t)arg)
#endif
  );
}

static inline void restore_return()
{
  asm volatile(
#if __x86_64__
      "movq 0(%%rsp), %%rcx"
      "movq 8(%%rsp), %%rax"
      :
      :
#else
      "movl 4(%%esp), %%ecx"
      :
      :
#endif
  );
}

enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过（或者执行过然后被切换出去了）
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

struct co
{
  char *name;
  void (*func)(void *); // co_start 指定的入口地址和参数
  void *arg;

  enum co_status status;     // 协程的状态
  struct co *waiter;         // 是否有其他协程在等待当前协程
  jmp_buf context;           // 寄存器现场 (setjmp.h)
  uint8_t stack[STACK_SIZE]; // 协程的堆栈
};

struct co *current; // 当前运行的协程

typedef struct Co_node
{
  struct co *coroutine;
  struct Co_node *prev;
  struct Co_node *next;
} Co_node;

static Co_node *co_node = NULL;

void insert_node(struct co *new_co)
{
  Co_node *node = (struct Co_node *)malloc(sizeof(struct Co_node));
  node->coroutine = new_co;
  if (co_node == NULL)
  {
    node->prev = node->next = node;
    co_node = node;
  }
  else
  {
    node->prev = co_node->prev;
    node->next = co_node;
    node->prev->next = node->next->prev = node;
  }
}

static Co_node *remove_node()
{
  Co_node *victim = NULL;

  if (co_node == NULL)
  {
    return NULL;
  }
  else if (co_node->next == co_node)
  {
    victim = co_node;
    co_node = NULL;
  }
  else
  {
    victim = co_node;

    co_node = co_node->next;
    co_node->prev = victim->prev;
    co_node->prev->next = co_node;
  }

  return victim;
}

struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
  // printf("hello world\n");
  struct co *new_co = (struct co *)malloc(sizeof(struct co));
  new_co->arg = arg;
  new_co->func = func;
  new_co->name = (char *)name;
  new_co->status = CO_NEW;
  new_co->waiter = NULL;
  insert_node(new_co);

  return new_co;
}

void co_wait(struct co *coroutine)
{

  if (coroutine->status != CO_DEAD)
  {
    coroutine->waiter = current;
    // 该携程不会被调度
    current->status = CO_WAITING;
    co_yield ();
  }

  while (co_node->coroutine != coroutine)
  {
    co_node = co_node->next;
  }

  assert(co_node->coroutine == coroutine);

  free(coroutine);
  free(remove_node());
}

#define JMP_RET 1

void co_yield ()
{
  int val = setjmp(current->context);
  if (val == 0)
  {
    // printf("val == 0\n");
    co_node = co_node->next;
    while (co_node->coroutine->status != CO_RUNNING && co_node->coroutine->status != CO_NEW)
    {
      co_node = co_node->next;
    }

    assert(co_node->coroutine->status == CO_RUNNING || co_node->coroutine->status == CO_NEW);
    current = co_node->coroutine;

    if (co_node->coroutine->status == CO_RUNNING)
    {
      longjmp(co_node->coroutine->context, JMP_RET);
    }
    else if (co_node->coroutine->status == CO_NEW)
    {
      // 调用函数
      ((struct co volatile *)current)->status = CO_RUNNING;
      // printf("before\n");
      stack_switch_call(current->stack + STACK_SIZE, current->func, current->arg);

      restore_return();
      // 返回回来后
      ((struct co volatile *)current)->status = CO_DEAD;

      // 它的等待者需要改为 RUNNING
      if (current->waiter != NULL)
      {

        current->waiter->status = CO_RUNNING;
      }

      co_yield ();
    }
  }
  else
  {
    // printf("val == 1\n");
    // 被切换为继续执行，状态一定是 RUNNING
    assert(val == JMP_RET && current->status == CO_RUNNING);
    return;
  }
}

static __attribute__((constructor)) void co_constructor(void)
{
  // current 为 当前执行的协程
  current = co_start("main", NULL, NULL);
  current->status = CO_RUNNING;
}

static __attribute__((destructor)) void co_deconstruct(void)
{
  while (co_node != NULL)
  {
    free(co_node->coroutine);
    free(remove_node());
  }
}
