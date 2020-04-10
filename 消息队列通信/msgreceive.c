#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

struct msg_st
{
	long int mst_type;
	char text[BUFSIZ];
};

int main(int argc, char **argv)
{
	int msgid = -1;
	struct msg_st* data;
	long int msgtype = 0;	// 注意1

	// 建立消息队列
	// 0666中0表示八进制数000
	// 0666中666表示用户、同组用户以及其他用户有读写权限
	msgid = msgget((key_t)6666, 0666|IPC_CREAT);
	if(msgid == -1){
		// fprintf将字数输出到流（文件）中，printf输出到标准输出设备（stdout）中，也就是屏幕
		fprintf(stderr, "msgget failed width error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	// 从队列中获得消息，直到遇到end
	int i = 0;
	while(1){
	//	i = msgrcv(msgid, (void *)data, BUFSIZ, msgtype, 0);
		msgrcv(msgid, data, BUFSIZ, msgtype, 0);
		if(i == -1){
			// Linux中系统调用的错误都存储于 errno中，errno由操作系统维护，存储就近发生的错误，即下一次的错误码会覆盖掉上一次的错误。
			fprintf(stderr, "msgrcv failed width error: %d", errno);
			exit(EXIT_FAILURE);
		}

		printf("You wrote is %s\n", data->text);

		if(strncmp(data->text, "end", 3) == 0)
			break;
	}
	
	// 删除消息队列
	if(msgctl(msgid, IPC_RMID, 0) == -1){
		fprintf(stderr, "msgctl(IPC_RMID) is failed!\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
