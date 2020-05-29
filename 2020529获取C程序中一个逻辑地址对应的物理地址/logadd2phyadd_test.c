#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE 4096

char buf[BUFSIZE];
typedef struct data {
	unsigned long addr;
	int p;
}mydata;

int main() { 
	unsigned long tmp,addr;
	int fd,len;
	mydata wdata;
	tmp = 0x12345678;
	addr = (unsigned long)&tmp;
	//输出tmp?
	//输出addr?
	printf("tmp value is : %lx \n",tmp); 
	printf("tmp address is : %lx \n",addr);

	wdata.addr = addr;
	wdata.p = getpid();
	//输出进程号pid?
	printf("the pid is : %d \n",wdata.p);

	//打开文件  
	fd = open("/proc/logadd2phyadd",O_RDWR);
	//将wdata写入文件?   
	//读文件?
	//输出文件长度及内容?
	write(fd,&wdata, sizeof(wdata));
	read(fd,buf,BUFSIZE);
	printf("%s\n",buf);
	sleep(2);
	close(fd);
	return 0;
} 
