// code v2.0

#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define __sys_pass_kernel_data 333

struct data_segment
{
    unsigned long mmap_base;
    unsigned long start_code, end_code, start_data, end_data;
    unsigned long start_brk, brk, start_stack;
};

void print_struct_data_segment(struct data_segment *__data_segment_now)
{
    printf("start_stack: \t%lx\tmmap_base: \t%lx\n", __data_segment_now->start_stack, __data_segment_now->mmap_base);
    printf("start_brk: \t%lx\tbrk: \t\t%lx\n", __data_segment_now->start_brk, __data_segment_now->brk);
    printf("start_data: \t%lx\tend_data: \t%lx\n", __data_segment_now->start_data, __data_segment_now->end_data);
    printf("start_code: \t%lx\tend_code: \t%lx\n", __data_segment_now->start_code, __data_segment_now->end_code);
    printf("\n");
}

void forkCatMaps(char **argv)
{
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        execv("/bin/cat", argv);
    }
    else if (pid_child > 0)
    {
        wait(NULL);
    }
    else
    {
        return;
    }
}

int main()
{
    struct data_segment __data_segment;

    printf("##### Before #####\n");
    print_struct_data_segment(&__data_segment);

    int a = syscall(__sys_pass_kernel_data, getpid(), (void *)&__data_segment);
    if (a != 0)
    {
        printf("syscall calling fail with return value: %d\n\n", a);
    }

    printf("##### After #####\n");
    print_struct_data_segment(&__data_segment);

    char argv_[6];
    sprintf(argv_, "%d", getpid());

    char str[20] = "/proc/";
    strcat(str, argv_);
    strcat(str, "/maps");
    printf("##### /bin/cat %s #####\n", str);

    char *argv[] = {"cat",
                    str,
                    (char *)0};
    forkCatMaps(argv);

    return 0;
}
