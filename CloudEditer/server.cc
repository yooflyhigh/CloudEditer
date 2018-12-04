/* 서버 */
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <wait.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
using namespace std;

#define BUF 1024
#define TRUE 1

int main(int argc,char *argv[]){
	/* 프로세스 관련 변수 */
	int pid;
	/* 소켓 관련 변수 */
	int server_socket, client_socket, client_addr_size;
	struct sockaddr_in server_addr, client_addr;
	/* 시그널 관련 변수 */
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	/* 디렉토리 관련 변수 */
	DIR *Userdir;
	struct dirent* entry = NULL;
	struct stat st;
	/* 메시지 관련 변수 */
	char Msgrcv[BUF];
	char Msgsnd[BUF];
	vector<string> filename;
	/* 경로 관련 변수 */
	char path[BUF] = "../ClientFolder";

	/* signal을 child가 죽어도 시그널을 무시로 해놓는다. */
	sigaction(SIGCHLD, &sa, NULL);
	
	/* TCP방식 소켓 생성 */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_socket == -1){
		printf("server socket 생성 실패\n");
		exit(1);
	}
	/* 주소 할당 및 포트번호 설정 */
	memset(&server_addr, 0, sizeof(server_addr));		//메모리를 server_addr에 할당
	server_addr.sin_family = AF_INET;						//IPv4 인터넷 프로토콜
	server_addr.sin_port = htons(4000);						//사용할 포트번호는 4000번
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//32bit IPv4 주소, INADDR_ANY를 사용하면 고정 IP가 아닌 자동 IP

	/* 소켓에 필요한 정보를 할당하고 커널에 등록 */
	if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		printf("bind() 실행 에러\n");
		exit(1);
	}

	/* bind된 server 소켓으로 클라이언트 접속 요청을 확인하도록 설정, 5명 까지 대기 가능 */
	if(listen(server_socket, 5) == -1){
			printf("listen() 실행 실패 \n");
			exit(1);
	}
	/* 서버는 클라이언트의 요청이 올때까지 listen상태에 머무르며 무한히 반복합니다. */
	while(TRUE){
		/* 클라이언트를 기다림 */
		waitpid(pid,NULL,WNOHANG);
		client_addr_size = sizeof(client_addr);
		client_socket = accept(server_socket, (sockaddr*)&client_addr, (socklen_t*)&client_addr_size);
		if(client_socket == -1){
			printf("클라이언트 연결 수락 실패\n");
			exit(1);
		}
		/* 클라이언트 디렉토리로 이동 */
		chdir(path);
		if((Userdir = opendir(path)) == NULL){
			printf("Dir open error\n");
			exit(0);
		}

		pid = fork();
		/* 자식 프로세스이면 메시지를 계속 주고받고 exit오면 종료 */
		if(pid == 0){
			while(TRUE){	
				/* 폴더 경로 보내 주기 */
				getcwd(path,BUF);
				write(client_socket, path, strlen(path)+1);
				/*클라이언트로 부터 메시지 읽기 */
				read(client_socket, Msgrcv, BUF);

				/* 파일 목록 쓰기 */
				if(!strcmp(Msgrcv,"ls")){
					Userdir = opendir(path);
					while((entry = readdir(Userdir)) != NULL){
						filename.push_back(entry->d_name);
						write(client_socket, entry->d_name, BUF);
						sleep(0.5);
					}
					write(client_socket, "0", BUF);
				}
				/* 폴더 이동 */
				else if(Msgrcv[0] == 'c' && Msgrcv[1] == 'd' && Msgrcv[2] == ' '){
					int i = 3, j = 0, flag = 0;
					char temp[BUF];
					/* 이동할 폴더이름 가져오기 */
					while(*(temp+(j++)) = *(Msgrcv+(i++)));

					/* 벡터에서 폴더 있는지 없는지 검사 */
					for(i = 0; i < filename.size(); i++){
						if(!filename[i].compare(temp)){
							flag = 1;
						}
					}
					/* 폴더 or 파일 검사 */
					if(lstat(temp,&st) == -1){
						perror("stat");
						exit(0);
					}
					if(S_ISDIR(st.st_mode) && flag){
						write(client_socket, "1", BUF);
						//path name set -> chdir -> vector clear
					}
					else{
						write(client_socket, "0", BUF);
					}
				}
				/* 파일 실행 */
				else if(Msgrcv[0] == '.' && Msgrcv[1] == '/'){
					int i = 2, j = 0, flag = 0;
					char temp[BUF];
					/* 실행할 폴더이름 가져오기 */
					while(*(temp+(j++)) = *(Msgrcv+(i++)));

					/* 벡터에서 폴더가 있는지 확인  */
					for(i = 0; i < filename.size(); i++){
						if(!filename[i].compare(temp)){
							flag = 1;
						}
					}
					/* 파일 상태 확인  */
					if(lstat(temp,&st) == -1){
						perror("stat");
						exit(0);
					}
					
					if(S_ISREG(st.st_mode) && flag){//일반파일 & 실행파일
						//툴 실행
					}else{//다른 파일일 때

					}
				}
				/* 종료 */
				else if(!strcmp(Msgrcv, "exit") || !strcmp(Msgrcv, "EXIT")){
					closedir(Userdir);
					exit(0);
				}
			}
		}
		/* 부모 프로세스이면 포크 떠주고 다시 기다림 */
		else if(pid > 0){
				close(client_socket);
		}
		else{
			printf("fork error \n");
			exit(0);
		}
	}
	return 0;
}
