#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>

#define PATH "./fifo"	// FIFO文件创建再本地
#define SIZE 128

int main(){
	umask(0);
	
	// 判断是否有FIFO文件，如果没有，则使用mkfifo函数创建
	if(access(PATH, F_OK) == -1){
		if (mkfifo (PATH,0777) == -1){
			// 创建FIFO失败，输出错误信息
			perror ("mkefifo error");
			exit(0);
		}
	}
	
	// 只读阻塞打开FIFO文件，配合只写阻塞
	int fd = open(PATH,O_RDONLY);
	if (fd<0){
		printf("open fd is error\n");
		return 0;
	}
	
	printf("This is read! fd=%d\n", fd);	// 测试open的阻塞
	
	// 设置Buf缓冲区，使用无限循环，不断地从FIFO中读出内容
	char Buf[SIZE];
	while(1){
		// 将fd中内容读入Buf，并返回读入字符数量
		ssize_t s = read(fd, Buf, sizeof(Buf));

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
		
	// 关闭文件描述符fd
	close (fd);
	return 3;
}
