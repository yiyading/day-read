#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<pthread.h>
#include<semaphore.h>

struct thread_args{
	int start;
	int end;
};

void *thread1st(void *);
void *thread2nd(void *);
int prime(int);

int main(){
	int ret;
	// 线程ID
	pthread_t thid1st, thid2nd;
	// threaddata是线程运行函数的参数
	struct thread_args threaddata1, threaddata2;
	void *thread_ret;
	pthread_attr_t thread1st_attr, thread2nd_attr;
	
	// 
	ret = pthread_attr_init(&thread1st_attr);
	if(ret){
		printf("Attribute creation error!\n");
		exit(EXIT_FAILURE);
	}

	//
	ret = pthread_attr_setdetachstate(&thread1st_attr, PTHREAD_CREATE_DETACHED);
	if(ret){
		printf("Set detached arrtibute error!\n");
		exit(EXIT_FAILURE);
	}

	// 创建线程
	threaddata1.start=10;
	threaddata1.end=20;
	ret=pthread_create(&thid1st, &thread1st_attr, thread1st, (void *)&threaddata1);
	if(ret){
		printf("Phtread creation error!\n");
		exit(EXIT_FAILURE);
	}

	//
	ret = pthread_attr_init(&thread2nd_attr);
	if(ret){
		printf("Attribute creation error!\n");
		exit(EXIT_FAILURE);
	}

	//
	ret = pthread_attr_setdetachstate(&thread2nd_attr, PTHREAD_CREATE_DETACHED);
	if(ret){
		printf("Set detached attribute error!\n");
		exit(EXIT_FAILURE);
	}

	threaddata2.start=30;
	threaddata2.end=40;
	ret=pthread_create(&thid2nd, NULL, thread2nd, (void *)&threaddata2);
	if(ret){
		printf("Pthread creation error!\n");
		exit(EXIT_FAILURE);
	}

#if 0
	// 线程阻塞
	ret=pthread_join(thid1st, &thread_ret);
	if(ret){
		printf("Pthread creationg failured!\n");
		exit(EXIT_FAILURE);
	}
	printf("Thread1st joined, return result: %s\n", (char *)thread_ret);

	ret=pthread_join(thid2nd, &thread_ret);
	if(ret){
		printf("Pthread join error!\n");
		exit(EXIT_FAILURE);
	}
	printf("Thread2nd joined, return result: %s\n", (char *)thread_ret);
#endif
	exit(EXIT_SUCCESS);
}

void *thread1st(void *args){
	int i, j, j2, k;
	int N1, N2;
	struct thread_args *data;
	data=(struct thread_args *)args;
	N1=data->start / 2;
	N2=data->end / 2;
	printf("Thread1st: Start=%d\n", data->start);

	for(j=N1; j<=N2; j++){
		j2=j*2;
		printf("T1_%d = ", j2);
		k=0;
		for(i=2; i<=j; i++){
			if(prime(i) && prime(j2-i)){
				if(k>=1) printf(",");
				if(k<=5) printf(" %d+%d", i, j2-i);
				k++;
			}
		}
		
		printf("\n");
		sleep(1);
	}	
	printf("Thread1st: End=%d\n", data->end);
	sleep(1);
	pthread_exit("thread1st is exited!");
}

void *thread2nd(void *args){
	int i, j, j2, k;
	int N1, N2;
	struct thread_args *data;
	data = (struct thread_args *)args;
	
	N1 = data->start / 2;
	N2 = data->end / 2;
	printf("Thread2nd: Start=%d\n", data->start);
	for(j = N1; j <= N2; j++){
		j2 = j * 2;
		printf("T2_%d = ", j2);
		k = 0;
		for(i = 2; i <= j; i++){
			if(prime(i) && prime(j2 - i)){
				if(k >= 1) printf(",");
				if(k <= 5) printf(" %d + %d", i, j2 - i);
				k++;
			}
		}
		
		printf("\n");
		sleep(1);
	}
	
	printf("Thread2nd: End=%d\n", data->end);
	sleep(1);
	pthread_exit("thread2nd is exited!!");
}

int prime(int n){
	int i, m;
	m=sqrt(n);
	for(i=2; i<=m; i++)
		if(n%i==0)
			return 0;

	return 1;
}
