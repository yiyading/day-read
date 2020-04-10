// 多线程互斥锁demo

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUMBER 3 /* 线程数 */
#define REPEAT_NUMBER 3 /* 每个线程的小任务数 */
#define DELAY_TIME_LEVELS 4.0 /*小任务之间的最大时间间隔*/

#define PTHREAD_MUTEX_EN 1  //是否开启互斥锁

// 全局声明互斥锁
pthread_mutex_t mutex;

void *thrd_func(void *arg)
{
    int thrd_num = (int)arg;
    int delay_time = 0, count = 0;
    int res;

#if PTHREAD_MUTEX_EN
    /*互斥锁上锁*/
    res=pthread_mutex_lock(&mutex);
#endif
    do{
	#if PTHREAD_MUTEX_EN
        if(res){
            printf("Thread %d lock failed\n", thrd_num);
            break;
        }
        #endif
        printf("Thread %d is starting\n", thrd_num);

        for(count = 0; count < REPEAT_NUMBER; count++)
        {
            delay_time=(int)(rand()*DELAY_TIME_LEVELS/(RAND_MAX))+1;
            sleep(delay_time);
            printf("\tThread %d: job %d delay = %d\n",thrd_num, count, delay_time);
        }
        printf("Thread %d finished\n", thrd_num);
    }while(0);

    #if PTHREAD_MUTEX_EN
    /* 互斥锁解锁 */
    pthread_mutex_unlock(&mutex);
    #endif
    pthread_exit(NULL);
}

int main()
{
    pthread_t thread[THREAD_NUMBER];
    int no = 0, res;
    void *thrd_ret;
    srand(time(NULL));//用时间作为产生随机数的种子

    /*互斥锁初始化*/
    pthread_mutex_init(&mutex, NULL);

    for( ; no < THREAD_NUMBER; no++)
    {
        res = pthread_create(&thread[no], NULL, thrd_func, (void *)no);
        if (res != 0){
            printf("Create thread %d failed\n", no);
            exit(res);
        }
    }
    printf("Create treads success\n Waiting for threads tofinish...\n");

    /*等待线程执结束*/
    for (no = 0; no < THREAD_NUMBER; no++)
    {
        res = pthread_join(thread[no], &thrd_ret);
        if (!res)
        {
            printf("Thread %d joined\n", no);
        }
        else
        {
            printf("Thread %d join failed\n", no);
        }
    }
    pthread_mutex_destroy(&mutex);
    printf("demo exit\n");
    return 0;
}
