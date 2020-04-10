/* @purpose: 基于信号量的多线程同步，操作系统原理中的p，v操作
 * @author: yiyading@pku.edu.cn
 * create: 2020-4-7 Thu
 */

#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

/* @scene: 某家海底捞只能服务两桌顾客。
 * 有多桌顾客到来，顾客如果发现已经坐满了人，就等待，
 * 如果有可用的桌子，就接受服务。
 */

// 将信号量定义为全局变量，方便多线程共享
sem_t sem;

// 每个线程要运行的例程
void *get_service(void *thread_id){
	// 立即保存threa_id的值，因为thread_id是对主线程中循环变量i的引用，它可能马上修改
	int customer_id = *((int *)thread_id);

	if(sem_wait(&sem) == 0){
		usleep(100);	// service time: 100ms
		printf("customer %d receive service ...\n", customer_id);
		sem_post(&sem);
	}
}

#define CUSTOMER_NUM 10

int main(int argc, char *argv[]){
	// 初始化信号量，初始值为2，表示有两个顾客可以同时接受服务
	sem_init(&sem, 0, 2);

	// 为每个顾客定义一个线程id，pthread_t其实是unsigned long int
	pthread_t customers[CUSTOMER_NUM];

	int i, ret;
	// 为每个顾客生成一个线程
	for(i = 0; i < CUSTOMER_NUM; i++){
		int customer_id = i;
		ret = pthread_create(&customers[i], NULL, get_service, &customer_id);
		if(ret != 0){
			perror("pthread_create");
			exit(1);
		}else{
			printf("Customer %d arrived.\n", i);
		}
		usleep(10);
	}

	// 等待所有顾客的线程结束
	// 这里不能再用i做循环变量，因为可能线程中正在访问i的值
	int j;
	for(j = 0; j < CUSTOMER_NUM; j++){
		pthread_join(customers[j], NULL);
	}
	
	sem_destroy(&sem);
	return 0;
}

