/* 클라이언트 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF 1024
int main(int argc,char *argv[]){
	int client_socket;
	struct sockaddr_in server_addr;
	char msg[BUF];

	/* 네트워크 통신을 위한  TCP 소켓 생성 */
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1){
		printf("client socket 생성 실패\n");
		exit(1);
	}
	/* 서버에 연결하기 전에 서버의 주소와 포트번호를 지정합니다. */
	memset(&server_addr, 0, sizeof(server_addr));	//메모리 할당
	server_addr.sin_family = AF_INET;					//IPv4 인터넷 프로토콜
	server_addr.sin_port = htons(4000);					//포트번호는 4000
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//서버의 주소

	/* 위의 서버로 접속 합니다. */
	if( -1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))){
		printf("접속실패\n");
		exit(1);
	}
	/* 서버와 통신합니다. */
	while(1){
		gets(msg);
		write(client_socket, msg, strlen(msg)+1);
		if(!strcmp(msg,"exit") || !strcmp(msg,"EXIT")){
			close(client_socket);
			break;
		}
		read(client_socket, msg, BUF);
		printf("client say : i receved [%s]\n",msg);
	}
	/* 소켓 종료 */
	close(client_socket);
	return 0;
}

