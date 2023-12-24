#include <stdio.h>
#include <stdint.h>

// Define the stack size
#define STACK_SIZE 1024

static inline void stack_switch_call(void *sp, void (*entry)(uintptr_t), uintptr_t arg)
{
    asm volatile(
#if __x86_64__
        "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
        : : "b"((uintptr_t)sp), "d"(entry), "a"(arg) : "memory"
#else
        "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
        : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg) : "memory"
#endif
    );
}

void example_function(uintptr_t arg)
{
    printf("Argument passed to example_function: %lu\n", arg);
}

int main()
{

    char stack[STACK_SIZE];
    stack_switch_call(stack + STACK_SIZE - 8, example_function, 42);

    return 0;
}
