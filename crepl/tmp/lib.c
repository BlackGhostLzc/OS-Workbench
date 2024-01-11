
int fib(int n) { if (n <= 1) return 1; return fib(n - 1) + fib(n - 2); }

int wrapper_0() { return fib(5); }
int wrapper_1() { return fib(6); }
int wrapper_2() { return fib(5)+fib(6)*2; }

int add(int a, int b){return a + b;}

int wrapper_3() { return add(7,8); }
int wrapper_4() { return fib(add(1,5)); }
