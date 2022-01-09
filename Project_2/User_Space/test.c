// code v2.3

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <syscall.h>
#include <unistd.h>

#include <linux/types.h>
#include <sys/wait.h>

#define __sys_get_phy_addr 333
#define ThreadStackSize 65536
#define insideThreadVariableNum 2

struct procmaps
{
    unsigned long start_vir; // %lx, %lu
    unsigned long end_vir;
    unsigned long start_phy;
    unsigned long end_phy;
    char *target_;
};

pthread_mutex_t mutex_;
__thread int threadtmp;
struct procmaps threadLocalAddr[100];
int threadLocalIndex = 0;
int bss_var[100];

char *ProcPIDMaps()
{
    // /proc/PID/maps
    char *str = (char *)malloc(sizeof(char) * 25);
    sprintf(str, "/proc/%d/maps", getpid());
    return str;
}

void ForkCatMaps(int printf_)
{
    printf("pid_t id:\t%d\n", getpid());
    if (printf_ == 0)
    {
        return;
    }

    char *str = ProcPIDMaps();
    // cat
    char *argv_[] = {"cat",
                     str,
                     (char *)0};
    free(str);
    // implement fork
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        execv("/bin/cat", argv_);
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

void PrintAddr_(FILE *inputFile, unsigned long **addr_arr_phy, int arr_len)
{
    // PrintAddr_((unsigned long **)&result_addr, arr_len);
    int index_ = 0;
    while (index_ < arr_len)
    {
        fprintf(inputFile, "index:\t%d\t", index_);
        fprintf(inputFile, "addr:\t%p\n", *(addr_arr_phy + index_));
        index_++;
    }
    fprintf(inputFile, "\n");
    return;
}

struct procmaps *CharToInt64(char *original)
{
    struct procmaps *newnode_ = (struct procmaps *)malloc(sizeof(struct procmaps));
    char *tmp1 = original, *tmp2 = NULL, *tmp3 = original;
    for (int i = 0;; i++)
    {
        if (tmp2 == NULL && tmp1[i] == '-')
        {
            // catch start
            tmp1[i] = '\0';
            tmp2 = &tmp1[i + 1];
            i = 0; // reset the index
        }
        else if (tmp2 != NULL && tmp2[i] == ' ')
        {
            // catch end
            tmp2[i] = '\0';
            tmp3 = &tmp2[i + 1];
            break;
        }
    }
    newnode_->start_vir = (unsigned long)strtoul(tmp1, NULL, 16);
    newnode_->end_vir = (unsigned long)strtoul(tmp2, NULL, 16);
    newnode_->target_ = tmp3;
    return newnode_;
}

int ReadMapsFile(struct procmaps **node_)
{
    FILE *in_file;
    FILE *outFile;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int counter = 0;
    char *filename = ProcPIDMaps();                        // result
    char *fileNameNEW = (char *)malloc(sizeof(char) * 25); // dulp. /proc/PID/maps
    sprintf(fileNameNEW, "%ld_maps.txt", syscall(SYS_gettid));
    // file open read mode
    in_file = fopen(filename, "r");
    free(filename);
    outFile = fopen(fileNameNEW, "w");
    free(fileNameNEW);
    if (in_file == NULL || outFile == NULL)
    {
        printf("file can not open.\n");
        return -1;
    }
    // file fscanf
    while ((read = getline(&line, &len, in_file)) >= 0)
    {
        fprintf(outFile, "%s", line);
        *(node_ + counter) = CharToInt64(line);
        line = NULL;
        counter += 1;
    }
    // file close
    fclose(in_file);
    fclose(outFile);
    return counter;
}

int mapsManipulate(struct procmaps **head_node, int counter)
{
    // bss
    struct procmaps *newnode;
    newnode = (struct procmaps *)malloc(sizeof(struct procmaps));
    newnode->start_vir = (unsigned long)(&bss_var[0]);
    newnode->end_vir = (unsigned long)(&bss_var[0] + (getpagesize() / 4)); // may have BUG
    newnode->target_ = " BSS variable\n";
    *(head_node + counter) = newnode;
    counter += 1;
    // thread
    for (int i = 0; i < threadLocalIndex; i++)
    {
        pthread_mutex_lock(&mutex_);
        newnode = (struct procmaps *)malloc(sizeof(struct procmaps));
        newnode->start_vir = (unsigned long)(threadLocalAddr[i].start_vir);
        newnode->end_vir = (unsigned long)(threadLocalAddr[i].start_vir + getpagesize());
        char *tmp = (char *)malloc(50 * sizeof(char));
        sprintf(tmp, " Thread %d, local variable %d.\n", i / insideThreadVariableNum, i % insideThreadVariableNum);
        newnode->target_ = tmp;
        *(head_node + counter) = newnode;
        counter += 1;
        pthread_mutex_unlock(&mutex_);
    }
    return counter;
}

int call__sys_get_phy_addr(FILE *inputFile)
{
    // all maps
    struct procmaps **maps_head = (struct procmaps **)malloc(200 * sizeof(struct procmaps *));
    int maps_head_len = ReadMapsFile(maps_head);
    maps_head_len = mapsManipulate(maps_head, maps_head_len);
    // core syscall
    int arr_len = 2 * maps_head_len;
    unsigned long *initial_addr[arr_len];
    unsigned long **ptr_vir = (unsigned long **)&initial_addr;
    int perdict_return_len;
    size_t page_size = getpagesize();
    // write on file
    fprintf(inputFile, "########## PID: %d ##########\n", getpid());
    for (int i = 0; i < maps_head_len; i++)
    {
        // copy to unsigned long array
        *(ptr_vir + 2 * i + 0) = (unsigned long *)((*(maps_head + i))->start_vir);
        *(ptr_vir + 2 * i + 1) = (unsigned long *)((*(maps_head + i))->end_vir);
        // write
        fprintf(inputFile, "########## next segment ##########\n");
        fprintf(inputFile, "target:%s\n", (*(maps_head + i))->target_);
        fprintf(inputFile, "virt addr: only start and end\n");
        PrintAddr_(inputFile, (unsigned long **)(ptr_vir + 2 * i), 2);
        // compute return_len
        perdict_return_len = (int)((((*(maps_head + i))->end_vir) - ((*(maps_head + i))->start_vir)) / page_size);
        unsigned long *result_addr[perdict_return_len];
        // syscall
        int result_len = syscall(__sys_get_phy_addr, (void *)(ptr_vir + 2 * i), 2, (void *)&result_addr, perdict_return_len);
        if (result_len == -1)
        {
            printf("syscall %d does not exist, please use uname check kernel version.\n", __sys_get_phy_addr);
            return -1;
        }
        else if (result_len <= 0)
        {
            printf("syscall calling fail with return value: %d, at index: %d.\n\n", result_len, i);
        }
        // write
        fprintf(inputFile, "phy addr: from start to end (exclude end)\n");
        PrintAddr_(inputFile, (unsigned long **)&result_addr, result_len);
    }
    fprintf(inputFile, "########## end of all segment ##########\n");
    // free memory
    for (int i = 0; i < maps_head_len; i++)
    {
        free(*(maps_head + i));
    }
    free(maps_head);
    return 0;
}

void *thread__sys_get_phy_addr()
{
    printf("pid_t id:\t%ld\tpthread_t id:\t%lu\n", syscall(SYS_gettid), pthread_self());
    // pthread_detach(pthread_self());
    if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL))
    {
        printf("tid: %ld, thread set cancel state fail.\n", pthread_self());
    }

    /*
    global __thread variable | global array
    */
    pthread_mutex_lock(&mutex_);
    threadLocalAddr[threadLocalIndex].start_vir = (unsigned long)&threadtmp;
    threadLocalIndex += 1;
    pthread_mutex_unlock(&mutex_);
    /*
    thread local variable | global array
    */
    pthread_mutex_lock(&mutex_);
    int a;
    threadLocalAddr[threadLocalIndex].start_vir = (unsigned long)&a;
    threadLocalIndex += 1;
    pthread_mutex_unlock(&mutex_);
    /*
    thread malloc variable | global array
    */
    // pthread_mutex_lock(&mutex_);
    // int *b = (int *)malloc(sizeof(int));
    // threadLocalAddr[threadLocalIndex].start_vir = (unsigned long)b;
    // threadLocalIndex += 1;
    // free(b);
    // pthread_mutex_unlock(&mutex_);

    sleep(2);
    // should not exit the thread before call the syscall and user_virt_to_phys
    // since it might lead to seg. fault
    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))
    {
        printf("tid: %ld, thread set cancel state fail.\n", pthread_self());
    }
    // Should never get here.
    sleep(5);
    printf("Hi there. I'm tid %ld\n", pthread_self());
    sleep(100);
}

void doThread(int num)
{
    // pthread
    pthread_attr_t attrs;
    pthread_t threadID[num];
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, ThreadStackSize);
    // counter
    int badNum1 = 0;
    int badNum2 = 0;
    int badNum3 = 0;
    for (int i = 0; i < num; i++)
    {
        if (pthread_create(&threadID[i], &attrs, thread__sys_get_phy_addr, NULL) != 0)
        {
            printf("thread %d create fail.\n", i);
            threadID[i] = -1;
            badNum1 += 1;
        }
    }
    printf("Successful create %d thread!\t(%d fail)\n", num - badNum1, badNum1);
    sleep(5);
    for (int i = 0; i < num; i++)
    {
        if ((threadID[i] != -1) && (pthread_cancel(threadID[i]) != 0))
        {
            printf("tid: %ld, thread cancel fail.\n", threadID[i]);
            badNum2 += 1;
        }
    }
    printf("Successful cancel %d thread!\t(%d fail)\n", num - badNum1 - badNum2, badNum2);
    sleep(1);
    for (int i = 0; i < num; i++)
    {
        if ((threadID[i] != -1) && (pthread_join(threadID[i], NULL) != 0))
        {
            printf("tid: %ld, thread join fail.\n", threadID[i]);
            pthread_kill(threadID[i], 1);
            badNum3 += 1;
        }
    }
    printf("Successful join %d thread!\t(%d fail)\n", num - badNum1 - badNum2 - badNum3, badNum3);
    sleep(1);
    return;
}

void InputPause()
{
    printf("input pause: ");
    int tmpchar;
    scanf("%d", &tmpchar);
    return;
}

int main()
{
    ForkCatMaps(0);
    pthread_mutex_init(&mutex_, NULL);
    doThread(5);
    printf("First stage done!\n");

    FILE *outFile;
    char *fileName;
    fileName = (char *)malloc(sizeof(char) * 40);
    sprintf(fileName, "%d_virt_phy_maps.txt", getpid());
    outFile = fopen(fileName, "w+");
    free(fileName);
    call__sys_get_phy_addr(outFile);
    fclose(outFile);

    pthread_mutex_destroy(&mutex_);

    printf("All stage done!\n");
    // ForkCatMaps(0);
    InputPause();
    return 0;
}
