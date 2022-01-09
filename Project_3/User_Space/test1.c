// code v0.1

#include <stdio.h>
#include <syscall.h>
#include <unistd.h>

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
    int i, t = 2, u = 3, v;
    struct cs w;

    for (i = 0; i < NUMBER_OF_ITERATIONS; i++)
        v = (++t) * (u++);

    int a = syscall(__getnum_context_switches, (void *)&w);
    if (a != 0)
        printf("syscall calling fail with return value: %d\n\n", a);
    else
        printf("This process encounters %lu times context switches.\n", (w.nvcsw + w.nivcsw));
}
