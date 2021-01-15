#include <stdio.h>
#include <stdlib.h>

void f1(void)
{
    puts("pushed first");
    fflush(stdout);
}

void f2(void) { puts("pushed second"); }

int main(void)
{
    at_quick_exit(f1);
    at_quick_exit(f2);
    quick_exit(EXIT_SUCCESS);
}
