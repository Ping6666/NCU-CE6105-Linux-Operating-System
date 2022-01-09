# Project 3

## Result

### Question 1 - context switches

```cmd!
./test1
This process encounters 10 times context switches.
```

#### dmesg

```cmd=1
[  372.491580] getrusage successful.
[  372.491581] current->nvcsw: 0
[  372.491581] current->nivcsw: 10
[  372.491582] copy_to_user successful with 16.
```

### Question 2 - entering a wait queue

```cmd!
./test2
123456 (<-input)
This process encounters 4 times context switches.
This process enters a wait queue 1 times.
I love my home.
I love my home.
I love my home.
I love my home.
I love my home.
I love my home.
This process enters a wait queue 1 times.
```

#### dmesg

```cmd=1
[  407.226048] getrusage successful.
[  407.226050] current->nvcsw: 1
[  407.226050] current->nivcsw: 3
[  407.226051] copy_to_user successful with 16.
[  407.226123] get wait_queue successful.
[  407.226124] copy_to_user successful with 8.
[  407.226154] get wait_queue successful.
[  407.226155] copy_to_user successful with 8.
```

## Problem Set

### state of the process

new, ready, run, waiting, terminated
wake_up(), preempt(), schedule(), yield(), terminate()

### wake_up, schedule

> [linux-kernel-slides.pdf#page.311 | bootlin.com](https://bootlin.com/doc/training/linux-kernel/linux-kernel-slides.pdf)

### trace code

nvcsw => voluntary_ctxt_switches
nivcsw => nonvoluntary_ctxt_switches

#### new process

No matter syscall: `fork`, `vfork`, `clone` or function: `do_fork`, `_do_fork`, `kernel_thread`, `copy_process`. Will check flags and copy mem.

##### copy_mm

copy_mm initialize task_struct values: nvcsw, nivcsw.

ps. CONFIG_DETECT_HUNG_TASK enable: checking TASK_UNINTERRUPTIBLE (aka. "D" state) which will lead to unable receiving signal.

##### copy_thread_tls

function under `./arch/x86/kernel/process_64.c` or `./arch/x86/kernel/process_32.c`: copy_thread_tls initialize self define variable inside task_struct my_wait_queue

> CONFIG_HAVE_COPY_THREAD_TLS `./arch/Kconfig#L804`: Architecture provides copy_thread_tls to accept tls argument via normal C parameter passing, rather than extracting the syscall argument from pt_regs.
>>  TLS: Thread Local Storage [Thread Local Storage (TLS) | Microsoft Docs](https://docs.microsoft.com/en-us/cpp/parallel/thread-local-storage-tls?view=msvc-170)

#### process been context switches

> kernel/sched/core.c

schedule => pick_next_task (sched_class (rt_sched_class, fair_sched_class, idle_sched_class))

```c=3379
++*switch_count
```

further reading: preempt, schedule, **pick_next_task**, getrusage, accumulate_thread_rusage

#### Process got out of (into) wait queue

##### TO default_wake_function calling chain

> include/linux/wait.h
> kernel/exit.c

syscall: wait4, waitid, waitpid => kernel_waitid, kernel_wait4 => do_wait => ... => init_waitqueue_func_entry, init_waitqueue_entry (wake_up handler)

##### FROM default_wake_function calling chain

> kernel/sched/core.c

default_wake_function => try_to_wake_up => ttwu_queue => ttwu_do_activate => ttwu_do_wakeup

> ttwu_do_wakeup

1. check state: TASK_RUNNING
2. manipulate my_wait_queue

##### example: function handler inside IO wait

> drivers/infiniband/hw/hfi1/iowait.h
