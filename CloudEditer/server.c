/* 서버 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <wait.h>
#include <signal.h>


#define BUF 1024
int main(int argc,char *argv[]){
	int server_socket, client_socket, client_addr_size;
	int pid;
	struct sockaddr_in server_addr, client_addr;
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	char Msgrcv[BUF];
	char Msgsnd[BUF];

	/* signal을 child가 죽어도 시그널을 무시로 해놓는다. */
	sigaction(SIGCHLD, &sa, NULL);
	
	/* TCP방식 소켓 생성 */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(-1 == server_socket){
		printf("server socket 생성 실패\n");
		exit(1);
	}
	/* 주소 할당 및 포트번호 설정 */
	memset(&server_addr, 0, sizeof(server_addr));		//메모리를 server_addr에 할당
	server_addr.sin_family = AF_INET;						//IPv4 인터넷 프로토콜
	server_addr.sin_port = htons(4000);						//사용할 포트번호는 4000번
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//32bit IPv4 주소, INADDR_ANY를 사용하면 고정 IP가 아닌 자동 IP

	/* 소켓에 필요한 정보를 할당하고 커널에 등록 */
	if(-1 == bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))){
		printf("bind() 실행 에러\n");
		exit(1);
	}

	/* bind된 server 소켓으로 클라이언트 접속 요청을 확인하도록 설정, 5명 까지 대기 가능 */
	if(-1 == listen(server_socket, 5)){
			printf("listen() 실행 실패 \n");
			exit(1);
	}
	/* 서버는 클라이언트의 요청이 올때까지 listen상태에 머무르며 무한히 반복합니다. */
	while(1){
		/* 클라이언트를 기다림 */
		waitpid(pid,NULL,WNOHANG);
		client_addr_size = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
		if( -1 == client_socket){
			printf("클라이언트 연결 수락 실패\n");
			exit(1);
		}
		pid = fork();

		/* 자식 프로세스이면 메시지를 계속 주고받고 exit오면 종료 */
		if(pid == 0){
			while(1){	
				read(client_socket, Msgrcv, BUF);
				if(!strcmp(Msgrcv,"exit") || !strcmp(Msgrcv,"EXIT")){
					printf("server say : exit를 누르셨습니다. 종료합니다.");
					exit(0);
				}
				sprintf(Msgsnd, "server say : %ld : %s", strlen(Msgrcv),Msgrcv);
				write(client_socket, Msgsnd, strlen(Msgsnd)+1);
			}
		}
		/* 부모 프로세스이면 포크 떠주고 다시 기다림 */
		else if(pid > 0){
				printf("프로세스[%d]가 할당되었습니다.\n",pid);
				close(client_socket);
		}
		else{
			printf("fork error \n");
			exit(0);
		}
	}
	return 0;
}

