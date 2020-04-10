#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>

#define PATH "./fifo_NONBLOCK"
#define SIZE 128

int main(){
	umask(0);
	
	if(access(PATH, F_OK) == -1){
		if (mkfifo (PATH,0777) == -1){
			perror ("mkefifo error");
			exit(0);
		}
	}

	int fd = open(PATH,O_RDONLY | O_NONBLOCK);
	if (fd<0){
		printf("open fd is error\n");
		return 0;
	}
	
	printf("This is read! fd=%d\n", fd);	// 测试open的阻塞
	
	char Buf[SIZE];
	while(1){
		ssize_t s = read(fd,Buf,sizeof(Buf));

		if (s<0){
			perror("read error");
			exit(1);
		}else if(s==0){
			// open中加入非阻塞，会出现s=0的情况
			printf("client quit! I shoud quit!\n");
			break;
		}else{
			Buf[s] = '\0';
			printf("client# %s ",Buf);
			fflush(stdout);
		}
	}
		
	close (fd);
	return 3;
}
