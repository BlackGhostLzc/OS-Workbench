#include <stdio.h>
#include <unistd.h> // 用于 sleep 函数

// 定义光标移动的方向
#define A "A"
#define B "B"
#define C "C"
#define D "D"

// 根据实验指南说明，其为图像中展示的不同的系统调用的个数
#define SYSCALL_INFO_SHOW_SIZE (5)

// 设置终端展示时候的窗口高
#define SYSCALL_INFO_WINDOW_HEIGHT (40)
// 设置终端展示时候的窗口高
#define SYSCALL_INFO_WINDOW_WIDTH (80)

#define syscall_info_show_format(color) ("\e[" #color ";37m%s\e[0m")
const char *syscall_info_show_formats[SYSCALL_INFO_SHOW_SIZE] = {syscall_info_show_format(42), syscall_info_show_format(45), syscall_info_show_format(43), syscall_info_show_format(44), syscall_info_show_format(46)};
#define syscall_info_show(idx, str) (fprintf(stderr, syscall_info_show_formats[(idx)], (str)))

#define syscall_info_show_move(opcode) (fprintf(stderr, "\e[1" #opcode))
// 将当前光标上移n行，列数不变
void syscall_info_show_move_up(int idx)
{
    for (int i = 0; i < idx; ++i)
    {
        syscall_info_show_move(A);
    }
}
// 将当前光标下移n行，列数不变
void syscall_info_show_move_down(int idx)
{
    for (int i = 0; i < idx; ++i)
    {
        syscall_info_show_move(B);
    }
}
// 将当前光标左移n列，行数不变
void syscall_info_show_move_left(int idx)
{
    for (int i = 0; i < idx; ++i)
    {
        syscall_info_show_move(D);
    }
}
// 将当前光标右移n列，行数不变
void syscall_info_show_move_right(int idx)
{
    for (int i = 0; i < idx; ++i)
    {
        syscall_info_show_move(C);
    }
}
// 将光标默认移动到第0行，第0列
#define syscall_info_show_position_init() (fprintf(stderr, "\e[0;0H"))

int main()
{
    // 初始化终端位置
    char buf[10];
    snprintf(buf, sizeof(buf), "%s(%%%d)", "open", (int)(0.65 * 100));

    printf("%s\n", buf);
}
