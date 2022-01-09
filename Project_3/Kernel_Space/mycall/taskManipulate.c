// code v0.1

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

struct cs
{
    unsigned long nvcsw;
    unsigned long nivcsw;
};

asmlinkage int sys_getnum_context_switches(void *__user user_addr)
{
    struct cs result_context_switches;
    struct rusage curr_ru;
    unsigned long copyCheck;

    // kernel/sys.c
    getrusage(current, RUSAGE_SELF, &curr_ru);
    printk(KERN_WARNING "getrusage successful.\n");

    result_context_switches.nvcsw = current->nvcsw;
    result_context_switches.nivcsw = current->nivcsw;
    printk(KERN_WARNING "current->nvcsw: %lu\n", current->nvcsw);
    printk(KERN_WARNING "current->nivcsw: %lu\n", current->nivcsw);

    // just in case
    if ((current->nvcsw != curr_ru.ru_nvcsw) || (current->nivcsw != curr_ru.ru_nivcsw))
    {
        printk(KERN_WARNING "curr_ru.ru_nvcsw: %lu\n", curr_ru.ru_nvcsw);
        printk(KERN_WARNING "curr_ru.ru_nivcsw: %lu\n", curr_ru.ru_nivcsw);
        printk(KERN_WARNING "something went worng in kernel code, context_switches numbers didnot match on current and getrusage.\n");
        return -3;
    }

    copyCheck = copy_to_user(user_addr, &result_context_switches, sizeof(struct cs));
    if (copyCheck != 0)
    {
        printk(KERN_WARNING "calling copy_to_user with %ld B remaining.\n", copyCheck);
        return -2;
    }
    printk(KERN_WARNING "copy_to_user successful with %ld.\n", sizeof(struct cs));

    return 0;
}

asmlinkage int sys_getnum_wait_queue(void *__user user_addr)
{
    unsigned long result_wait_queue;
    unsigned long copyCheck;
    result_wait_queue = current->my_wait_queue;
    printk(KERN_WARNING "get wait_queue successful.\n");

    copyCheck = copy_to_user(user_addr, &result_wait_queue, sizeof(unsigned long));
    if (copyCheck != 0)
    {
        printk(KERN_WARNING "calling copy_to_user with %ld B remaining.\n", copyCheck);
        return -2;
    }
    printk(KERN_WARNING "copy_to_user successful with %ld.\n", sizeof(unsigned long));

    return 0;
}
