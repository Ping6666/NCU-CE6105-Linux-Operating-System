// code v0.1

#include <stdio.h>
#include <syscall.h>
#include <unistd.h>

#define NUMBER_OF_IO_ITERATIONS 6
#define NUMBER_OF_ITERATIONS 99999999
#define __getnum_context_switches 333
#define __getnum_wait_queue 334

struct cs
{
    unsigned long nvcsw;
    unsigned long nivcsw;
};

int main()
{
    char c;
    int i, t = 2, u = 3, v;
    struct cs w1;
    unsigned long w2;

    for (i = 0; i < NUMBER_OF_IO_ITERATIONS; i++)
    {
        v = 1;
        c = getchar();
    }

    for (i = 0; i < NUMBER_OF_ITERATIONS; i++)
        v = (++t) * (u++);

    int a = syscall(__getnum_context_switches, (void *)&w1);
    if (a != 0)
        printf("__getnum_context_switches\nsyscall calling fail with return value: %d\n\n", a);
    else
        printf("This process encounters %lu times context switches.\n", (w1.nvcsw + w1.nivcsw));

    a = syscall(__getnum_wait_queue, (void *)&w2);
    if (a != 0)
        printf("__getnum_wait_queue\nsyscall calling fail with return value: %d\n\n", a);
    else
        printf("This process enters a wait queue %lu times.\n", w2);

    for (i = 0; i < NUMBER_OF_IO_ITERATIONS; i++)
    {
        v = 1;
        printf("I love my home.\n");
    }

    a = syscall(__getnum_wait_queue, (void *)&w2);
    if (a != 0)
        printf("__getnum_wait_queue\nsyscall calling fail with return value: %d\n\n", a);
    else
        printf("This process enters a wait queue %lu times.\n", w2);
}
