#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer1;
jmp_buf jump_buffer2;

void foo()
{
    int value = setjmp(jump_buffer2);

    if (value == 0)
    {
        printf("Inside foo, first time\n");
        longjmp(jump_buffer2, 1); // Jump back to the saved context with a value of 1
    }
    else
    {
        printf("Inside foo, after longjmp with value: %d\n", value);
        longjmp(jump_buffer1, 1);
    }
}

int main()
{
    int value = setjmp(jump_buffer1);

    if (value == 0)
    {
        printf("Inside main, first time\n");
        foo(); // This will longjmp back to here with a value of 1
    }
    else
    {
        printf("Inside main, after longjmp with value: %d\n", value);
    }

    return 0;
}
