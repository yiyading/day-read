// 创建共享内存，并读取其中信息
#include <stddef.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shmdata.h"
 
int main(int argc, char **argv)
{
    void *shm = NULL;
    struct shared_use_st *shared; // 指向shm
    int shmid; // 共享内存标识符
 
    // 创建共享内存
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
 
    // 将共享内存连接到当前进程的地址空间
    shm = shmat(shmid, 0, 0);
    if (shm == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
 
    printf("\nMemory attached at %X\n", (int)shm);
 
    // 设置共享内存
    shared = (struct shared_use_st*)shm; // 注意：shm有点类似通过 malloc() 获取到的内存，所以这里需要做个类型强制转换
    shared->written = 0;
    while (1) // 读取共享内存中的数据
    {
        // 没有进程向内存写数据，有数据可读取
        if (shared->written == 1)
        {
            printf("You wrote: %s", shared->text);
            sleep(1);
 
            // 读取完数据，设置written使共享内存段可写
            shared->written = 0;
 
            // 输入了 end，退出循环（程序）
            if (strncmp(shared->text, "end", 3) == 0)
            {
                break;	// break跳出while和for循环
            }
        }
        else // 有其他进程在写数据，不能读取数据
        {
            sleep(1);
        }
    }
 
    // 把共享内存从当前进程中分离
    if (shmdt(shm) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
 
    // 删除共享内存
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
 
    exit(EXIT_SUCCESS);
}
