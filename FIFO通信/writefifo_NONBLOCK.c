#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>

#define PATH "./fifo_NONBLOCK"
#define SIZE 128

int main(){
	int fd=open(PATH, O_WRONLY | O_NONBLOCK);
	
	if(fd<0){
		perror("open error");
		exit(0);
	}

	printf("This is write!, fd=%d\n", fd);	// 测试阻塞
	char buf[SIZE];
	while(1){
		printf("Please Enter: ");
		fflush(stdout);
		ssize_t s=read(0, buf, sizeof(buf));
		
		if(s<0){
			perror("read is failed\n");
			exit(1);
		}else if(s==0){
			printf("read is closed!\n");
			return 1;
		}else{
			buf[s]='\0';
			write(fd, buf, strlen(buf));
		}
	}
	
	return 0;
}
