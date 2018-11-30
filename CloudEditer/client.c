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

	char buf[BUF+5];
	
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(-1 == client_socket){
		printf("client socket 생성 실패\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if( -1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))){
		printf("접속실패\n");
		exit(1);
	}

	write(client_socket, argv[1], strlen(argv[1])+1);
	read(client_socket, buf, BUF);
	printf("%s\n",buf);
	close(client_socket);

	return 0;
}

