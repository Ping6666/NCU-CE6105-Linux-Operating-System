# Project 1

## Result

### gdb ./test (with peda)

```cmd=1
gdb-peda$ vmmap
Start              End                Perm	Name
0x0000555555554000 0x0000555555555000 r-xp	/home/xxx/xxx/test
0x0000555555755000 0x0000555555756000 r--p	/home/xxx/xxx/test
0x0000555555756000 0x0000555555757000 rw-p	/home/xxx/xxx/test
0x00007ffff79e2000 0x00007ffff7bc9000 r-xp	/lib/x86_64-linux-gnu/libc-2.27.so
0x00007ffff7bc9000 0x00007ffff7dc9000 ---p	/lib/x86_64-linux-gnu/libc-2.27.so
0x00007ffff7dc9000 0x00007ffff7dcd000 r--p	/lib/x86_64-linux-gnu/libc-2.27.so
0x00007ffff7dcd000 0x00007ffff7dcf000 rw-p	/lib/x86_64-linux-gnu/libc-2.27.so
0x00007ffff7dcf000 0x00007ffff7dd3000 rw-p	mapped
0x00007ffff7dd3000 0x00007ffff7dfc000 r-xp	/lib/x86_64-linux-gnu/ld-2.27.so
0x00007ffff7fe2000 0x00007ffff7fe4000 rw-p	mapped
0x00007ffff7ff7000 0x00007ffff7ffa000 r--p	[vvar]
0x00007ffff7ffa000 0x00007ffff7ffc000 r-xp	[vdso]
0x00007ffff7ffc000 0x00007ffff7ffd000 r--p	/lib/x86_64-linux-gnu/ld-2.27.so
0x00007ffff7ffd000 0x00007ffff7ffe000 rw-p	/lib/x86_64-linux-gnu/ld-2.27.so
0x00007ffff7ffe000 0x00007ffff7fff000 rw-p	mapped
0x00007ffffffde000 0x00007ffffffff000 rw-p	[stack]
0xffffffffff600000 0xffffffffff601000 r-xp	[vsyscall]
```

```cmd=1
gdb-peda$ c
Continuing.
##### Before #####
start_stack: 	f0b5ff	mmap_base: 	0
start_brk: 	7ffff7dd5660	brk: 		7fffffffdf98
start_data: 	0	end_data: 	9
start_code: 	0	end_code: 	0

##### After #####
start_stack: 	7fffffffe060	mmap_base: 	7ffff7fff000
start_brk: 	555555757000	brk: 		555555778000
start_data: 	555555755d68	end_data: 	555555756010
start_code: 	555555554000	end_code: 	555555554ec8

##### /bin/cat /proc/5530/maps #####
[New process 5535]
process 5535 is executing new program: /bin/cat
Error in re-setting breakpoint 1: Function "main" not defined.
555555554000-555555555000 r-xp 00000000 08:01 1720492                    /home/xxx/xxx/test
555555755000-555555756000 r--p 00001000 08:01 1720492                    /home/xxx/xxx/test
555555756000-555555757000 rw-p 00002000 08:01 1720492                    /home/xxx/xxx/test
555555757000-555555778000 rw-p 00000000 00:00 0                          [heap]
7ffff79e2000-7ffff7bc9000 r-xp 00000000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7bc9000-7ffff7dc9000 ---p 001e7000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7dc9000-7ffff7dcd000 r--p 001e7000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7dcd000-7ffff7dcf000 rw-p 001eb000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7ffff7dcf000-7ffff7dd3000 rw-p 00000000 00:00 0 
7ffff7dd3000-7ffff7dfc000 r-xp 00000000 08:01 1976757                    /lib/x86_64-linux-gnu/ld-2.27.so
7ffff7fe2000-7ffff7fe4000 rw-p 00000000 00:00 0 
7ffff7ff7000-7ffff7ffa000 r--p 00000000 00:00 0                          [vvar]
7ffff7ffa000-7ffff7ffc000 r-xp 00000000 00:00 0                          [vdso]
7ffff7ffc000-7ffff7ffd000 r--p 00029000 08:01 1976757                    /lib/x86_64-linux-gnu/ld-2.27.so
7ffff7ffd000-7ffff7ffe000 rw-p 0002a000 08:01 1976757                    /lib/x86_64-linux-gnu/ld-2.27.so
7ffff7ffe000-7ffff7fff000 rw-p 00000000 00:00 0 
7ffffffde000-7ffffffff000 rw-p 00000000 00:00 0                          [stack]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
```

### readelf

```cmd=1
xxx@xxx-virtual-machine:~/xxx$ readelf -l ./test

Elf 檔案類型為 DYN (共享物件檔案)
Entry point 0x830
There are 9 program headers, starting at offset 64

程式標頭：
  類型           偏移量             虛擬位址           實體位址
                 檔案大小          記憶大小              旗標   對齊
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x00000000000001f8 0x00000000000001f8  R      0x8
  INTERP         0x0000000000000238 0x0000000000000238 0x0000000000000238
                 0x000000000000001c 0x000000000000001c  R      0x1
      [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000ec8 0x0000000000000ec8  R E    0x200000
  LOAD           0x0000000000001d68 0x0000000000201d68 0x0000000000201d68
                 0x00000000000002a8 0x00000000000002b0  RW     0x200000
  DYNAMIC        0x0000000000001d78 0x0000000000201d78 0x0000000000201d78
                 0x00000000000001f0 0x00000000000001f0  RW     0x8
  NOTE           0x0000000000000254 0x0000000000000254 0x0000000000000254
                 0x0000000000000044 0x0000000000000044  R      0x4
  GNU_EH_FRAME   0x0000000000000d30 0x0000000000000d30 0x0000000000000d30
                 0x000000000000004c 0x000000000000004c  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000001d68 0x0000000000201d68 0x0000000000201d68
                 0x0000000000000298 0x0000000000000298  R      0x1

 區段到節區映射中:
  節區段…
   00     
   01     .interp 
   02     .interp .note.ABI-tag .note.gnu.build-id .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt .init .plt .plt.got .text .fini .rodata .eh_frame_hdr .eh_frame 
   03     .init_array .fini_array .dynamic .got .data .bss 
   04     .dynamic 
   05     .note.ABI-tag .note.gnu.build-id 
   06     .eh_frame_hdr 
   07     
   08     .init_array .fini_array .dynamic .got
```

### ./test

```cmd=1
xxx@xxx-virtual-machine:~/xxx$ ./test 
##### Before #####
start_stack: 	f0b5ff	mmap_base: 	0
start_brk: 	7f0072f28660	brk: 		7ffe1099ec48
start_data: 	0	end_data: 	9
start_code: 	0	end_code: 	0

##### After #####
start_stack: 	7ffe1099ed10	mmap_base: 	7f0073152000
start_brk: 	5646a144d000	brk: 		5646a146e000
start_data: 	56469f736d68	end_data: 	56469f737010
start_code: 	56469f535000	end_code: 	56469f535ec8

##### /bin/cat /proc/3781/maps #####
56469f535000-56469f536000 r-xp 00000000 08:01 1720492                    /home/xxx/xxx/test
56469f736000-56469f737000 r--p 00001000 08:01 1720492                    /home/xxx/xxx/test
56469f737000-56469f738000 rw-p 00002000 08:01 1720492                    /home/xxx/xxx/test
5646a144d000-5646a146e000 rw-p 00000000 00:00 0                          [heap]
7f0072b35000-7f0072d1c000 r-xp 00000000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7f0072d1c000-7f0072f1c000 ---p 001e7000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7f0072f1c000-7f0072f20000 r--p 001e7000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7f0072f20000-7f0072f22000 rw-p 001eb000 08:01 1976761                    /lib/x86_64-linux-gnu/libc-2.27.so
7f0072f22000-7f0072f26000 rw-p 00000000 00:00 0 
7f0072f26000-7f0072f4f000 r-xp 00000000 08:01 1976757                    /lib/x86_64-linux-gnu/ld-2.27.so
7f007313a000-7f007313c000 rw-p 00000000 00:00 0 
7f007314f000-7f0073150000 r--p 00029000 08:01 1976757                    /lib/x86_64-linux-gnu/ld-2.27.so
7f0073150000-7f0073151000 rw-p 0002a000 08:01 1976757                    /lib/x86_64-linux-gnu/ld-2.27.so
7f0073151000-7f0073152000 rw-p 00000000 00:00 0 
7ffe1097f000-7ffe109a0000 rw-p 00000000 00:00 0                          [stack]
7ffe109ba000-7ffe109bd000 r--p 00000000 00:00 0                          [vvar]
7ffe109bd000-7ffe109bf000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
```

## Problem Set

### rwxp

r = read
w = write
x = execute
s = shared
p = private (copy on write)

### gdb

#### _get_allmaps_linux

##### trace code: (peda)

/proc/{PID}/maps

##### trace code: (linux kernel)

proc_pid_maps_op -> show_map -> show_map_vma -> vma (vm_area_struct)

#### dbg_print_vars

inspect.currentframe().f_back.f_locals

### mm_struct (mm)

task_struct -> mm_struct

### vm_area_struct (mmap)

task_struct -> mm_struct -> vm_area_struct

### Address Translation (inside a CPU) (IA-32)

Logical Addresses -(Segmentation Unit)> Linear Address (Virtual Address) -(Paging Unit)> Physical address

### asmlinkage tag (stack)

This is a #define for some gcc magic that tells the compiler that the function should not expect to find any of its arguments in registers (a common optimization), but only on the CPU’s stack.

#### system calls (Kernel Interface) (six registers)

Reference: [assembly - What are the calling conventions for UNIX & Linux system calls (and user-space functions) on i386 and x86-64 - Stack Overflow](https://stackoverflow.com/questions/2535989/what-are-the-calling-conventions-for-unix-linux-system-calls-and-user-space-f)

##### x86-32 aka i386 Linux System Call convention

In x86-32 parameters for Linux system call are passed using **registers**. `%eax` for syscall_number. `%ebx`, `%ecx`, `%edx`, `%esi`, `%edi`, `%ebp` are used for passing 6 parameters to system calls.

##### x86-64 Linux System Call convention

1. User-level applications use as integer **registers** for passing the sequence `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8` and `%r9`. The kernel interface uses `%rdi`, `%rsi`, `%rdx`, `%r10`, `%r8` and `%r9`.
2. A system-call is done via the syscall instruction. This [clobbers %rcx and %r11](https://stackoverflow.com/questions/47983371/why-do-x86-64-linux-system-calls-modify-rcx-and-what-does-the-value-mean) as well as the `%rax` return value, but other registers are preserved.
3. The number of the syscall has to be passed in register %rax.
4. System-calls are limited to six arguments, no argument is passed directly on the stack.
5. Returning from the syscall, register %rax contains the result of the system-call. A value in the range between -4095 and -1 indicates an error, it is -errno.
6. Only values of class INTEGER or class MEMORY are passed to the kernel.

### SYSCALL_DEFINE

Since, there are more aliased functions that need to be set in v5.11 compare to v4.14 (see the kernel codes below). Therefore in kernel version upper than 4.17, can’t use simple (cheat) way to define a system call by only changing the function name without define it.
And more importantly, return value that need to be convert from long to int, since it may cause denial of service or even gain privileges.

Further reading: Linux CVE-2009-0029 Vulnerability

### memcpy vs. copy_to_user

Function copy_to_user will do much more check than memcpy. And function memcpy can only touch the page that in the same process, but copy_to_user can copy from kernel space to user space.
