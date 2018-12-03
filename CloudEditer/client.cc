/* 클라이언트 */
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
using namespace std;

#define BUF 1024
#define TRUE 1
int main(int argc,char *argv[]){
	/* 소켓 관련 변수 */
	int client_socket;
	struct sockaddr_in server_addr;
	/* 메시지 관련 변수 */
	char msg[BUF];
	/* 경로 관련 변수 */
	char path[BUF];

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
	system("clear");
	printf("안녕하세요. SP TermProject에 오신걸 환영합니다. \n 'help'를 입력하여 명령어를 확인하세요.\n\n");
	while(1){
		read(client_socket, path, BUF);	
		/* 명령어 입력 */
		printf("(%d)%s$ ",getpid(),path);
		cin.getline(msg, 1000,'\n');
		/* 서버로 명령어 전송 */
		write(client_socket, msg, strlen(msg)+1);
		/* help  */
		if(!strcmp(msg,"help")){
			system("clear");
			printf("'help' 명령어를 출력합니다.\n");
			printf("'whoami' 접속하신 프로세스의 id를 출력합니다.\n");
			printf("'clear' 화면을 지워줍니다.\n");
			printf("'ls' 서버의 공유 파일을 읽어옵니다.\n");
			printf("'cd [폴더이름]' 해당 폴더로 이동합니다.\n");
			printf("'./[파일이름]' 해당 파일을 오픈합니다.\n");
			printf("'exit' 연결을 종료합니다.\n\n");
		}
		/* pid 출력 */
		else if(!strcmp(msg,"whoami")){
			printf("접속하신 계정의 id는 (%d) 입니다.\n",getpid());
		}
		/* 화면 clear */
		else if(!strcmp(msg,"clear")){
			system("clear");
		}
		/* 파일 목록 출력 */
		else if(!strcmp(msg,"ls")){
			char end[10];
			while(1){
				read(client_socket, end, BUF);
				if(end[0] == '0')	break;
				printf("%s\n",end);
			}
			printf("\n");
		}
		/* 폴더 이동 */
		else if(msg[0] == 'c' && msg[1] == 'd' && msg[2] == ' '){
			read(client_socket, msg, BUF);
			if(msg[0] == '0'){
				printf("folder 아니거나 파일명이 잘못됐습니다.\n");
			}
		}
		/* 파일 실행 */
		else if(msg[0] == '.' && msg[1] == '/'){
			printf("파일 실행\n");
		}
		/* 종료 */
		else if(!strcmp(msg,"exit") || !strcmp(msg,"EXIT")){
			close(client_socket);
			break;
		}
		else{
			printf("그런 명령어는 없습니다.\n");
			continue;
		}
	}
	/* 소켓 종료 */
	close(client_socket);
	return 0;
}
