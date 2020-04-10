#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/msg.h>
#include<errno.h>

#define MAX_TEXT 512

struct msg_st{
	long int msg_type;
	char text[MAX_TEXT];
};

int main(int argc, char *argv[]){
	struct msg_st* data;
	char buffer[BUFSIZ];
	int msgid = -1;

	// 建立消息队列
	msgid = msgget((key_t)6666, 0666|IPC_CREAT);
	if(msgid == -1){
		fprintf(stderr, "msgget failed error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	int i = 0;
	// 向消息队列中写信息，直到写入end
	while(1){
		printf("Enter some text: ");
		fgets(buffer, BUFSIZ, stdin);
		data->msg_type = 1;
		strcpy(data->text, buffer);

		// 像队列里发送数据
	//	i = msgsnd(msgid, (void *)data, MAX_TEXT, 0);
		msgsnd(msgid, data, MAX_TEXT, 0);
		if(i == -1){
			fprintf(stderr, "msgsend(msgid) is failed\n");	
			exit(EXIT_FAILURE);
		}

		// 输入end结束
		if(strncmp(buffer, "end", 3) == 0)
			break;
			
		sleep(3);
	}
	
	exit(EXIT_FAILURE);
}

