#include <stdio.h>
#include <setjmp.h>
#include <stdint.h>

#define K 1024
#define STACK_SIZE (64 * K)

struct co
{
    char *name;
    void (*func)(void *); // co_start 指定的入口地址和参数
    void *arg;
    jmp_buf context;           // 寄存器现场 (setjmp.h)
    uint8_t stack[STACK_SIZE]; // 协程的堆栈
};

struct co *current;
struct co *A;
struct co *B;

void yiled()
{
    int val = setjump(current->context);
}

void A()
{
    while (1)
    {
        printf("A\n");
        yield();
    }
}

void B()
{
    while (1)
    {
        printf("B\n");
        yield();
    }
}

int main()
{
    A = malloc(sizeof(struct co));
    A->name
}