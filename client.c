#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
void clearScreen() {
    // Works on almost all modern terminals (Linux, macOS, Windows 10+)
    printf("\033[1;1H\033[2J");
    fflush(stdout);
}
void check(int ,char *);
void handle(int);
int connect_server(char *);

#define SERVER_PORT "3000"

int main(int argc,char **argv){
	int server = connect_server(SERVER_PORT);
	char buf[1024];
	fd_set readfs;
	while(1)
	{
		FD_ZERO(&readfs);
		FD_SET(STDIN_FILENO,&readfs);
		FD_SET(server,&readfs);
		int maxfd = server > STDIN_FILENO ? server : STDIN_FILENO;
		
		if(select(maxfd+1,&readfs,NULL,NULL,NULL) < 0){
			perror("select error");
			break;
		}

		//keyboard input	
		if(FD_ISSET(STDIN_FILENO,&readfs)){
			clearScreen();
			printf("Press Ctrl + C to quit\n");
			printf("> ");
			if(fgets(buf,sizeof buf,stdin) == NULL){
				break;
			}
			send(server,buf,sizeof buf, 0);
		}
		//server data
		if(FD_ISSET(server,&readfs)){
			int n = recv(server,buf,sizeof buf,0);
		
			if(n == 0){
				printf("Server not found\n");
				close(server);
				break;
			}
			if(n < 0){
				perror("recv error");
				close(server);
				break;
			}
			buf[n] = '\0';
			printf("%s",buf);
			fflush(stdout);

		}

	}
	close(server);
	return 0;
}

int connect_server(char *port){

	struct addrinfo hints, *res;
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	check(getaddrinfo("0.0.0.0",port,&hints,&res),"getaddrinfo error");

	int server = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

	if((connect(server,res->ai_addr,res->ai_addrlen)) < 0 ){
		perror("connect");
		close(server);
		return -1;
	}
	return server;


}


void check(int status,char *err){
	if(status < 0){
		perror(err);	
	}

}
