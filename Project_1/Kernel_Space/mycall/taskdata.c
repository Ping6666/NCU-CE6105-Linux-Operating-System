// code v2.0

#include <linux/init_task.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include <linux/uaccess.h>

struct data_segment
{
    unsigned long mmap_base;
    unsigned long start_code, end_code, start_data, end_data;
    unsigned long start_brk, brk, start_stack;
};

asmlinkage int sys_pass_kernel_data(pid_t user_pid, void *__user user_address)
{
    struct data_segment my_data_segment;
    struct task_struct *task;
    unsigned long copyCheck = -1;
    int counter = 10;

    printk(KERN_INFO "sucessfull call sys_pass_kernel_data\n");

    for_each_process(task)
    {
        printk(KERN_INFO "sucessfull looping the task between all task_struct\n");

        if (task->pid == user_pid)
        {
            printk(KERN_INFO "sucessfull finding the correct pid which we want\n");

            // start_stack / mmap_base
            my_data_segment.start_stack = task->mm->start_stack;
            my_data_segment.mmap_base = task->mm->mmap_base;
            // brk
            my_data_segment.start_brk = task->mm->start_brk;
            my_data_segment.brk = task->mm->brk;
            // data
            my_data_segment.start_data = task->mm->start_data;
            my_data_segment.end_data = task->mm->end_data;
            // code
            my_data_segment.start_code = task->mm->start_code;
            my_data_segment.end_code = task->mm->end_code;

            while (copyCheck != 0)
            {
                copyCheck = copy_to_user(user_address, &my_data_segment, sizeof(struct data_segment));
                printk(KERN_WARNING "copy_to_user with %lx B remaining.\n", copyCheck);
                counter--;
                if (counter == 0)
                {
                    printk(KERN_WARNING "copy_to_user fail with many times.\n");
                    return -1;
                }
            }
            return 0;
        }
    }
    return -2;
}
