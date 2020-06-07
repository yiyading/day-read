#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
 
#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 1024

int main(){
	// 定义sockfd
	int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

	///定义sockaddr_in
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;			// 协议族
	server_sockaddr.sin_port = htons(1234);		// sin_point=0，自动选择一个未占用的端口
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// sim_addr.s_addr置为INADDR_ANY，自动填入本机IP地址
 
	//bind，成功返回0，出错返回-1
	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
		perror("bind");
		exit(1);
	}
 
	// listen，成功返回0，出错返回-1
	if(listen(server_sockfd, QUEUE) == -1){
		perror("listen");
		exit(1);
	}
 
	// 客户端套接字
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
 
	//成功返回非负描述字，出错返回-1
	int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);

	if(conn<0){
		perror("connect");
		exit(1);
	}
 
	while(1){
       		memset(buffer,0,sizeof(buffer));
        	int len = recv(conn, buffer, sizeof(buffer),0);
        	if(strcmp(buffer,"exit\n")==0)
            		break;
        	fputs(buffer, stdout);
        	send(conn, buffer, len, 0);
    	}

    	close(conn);
    	close(server_sockfd);
    	return 0;
}
