#include<stdio.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

void check(int,char*);

int setup_server(char*,int);
int accept_client(int);
void handle_connection(int,fd_set);


typedef struct{
	int fd;
	int buf_len;
	char buf[4096];
	char name[20];
	int has_name;

}client_t;

void send_msg(client_t *,int *,int *,fd_set *);

client_t clients[FD_SETSIZE];



#define SERVER_PORT "3000"
#define BACKLOG 10
#define MAX_USERNAME 20
#define BUFFER_SIZE 8
int main(){
	int serverfd = setup_server(SERVER_PORT,BACKLOG);
	fd_set current_fds, read_fds;
	FD_ZERO(&current_fds);
	FD_SET(serverfd,&current_fds);
	char *names = malloc( 100 * MAX_USERNAME * sizeof(char));
	int max_fd = serverfd;
	while (1) {
		fd_set read_fds = current_fds;

		select(max_fd +1, &read_fds, NULL, NULL, NULL);


		for (int fd = 0; fd <= max_fd; fd++) {

			if (!FD_ISSET(fd, &read_fds))
				continue;

			if (fd == serverfd) {

				int client = accept(serverfd, NULL, NULL);
				FD_SET(client, &current_fds);

				printf("Client Connected\n");
				client_t client_data;

				client_data.fd = client;
				client_data.has_name = -1;

				clients[client] = client_data;

				char *server_prompt = "Enter your name: ";
				send(client,server_prompt,strlen(server_prompt),0);
				
				if (client > max_fd)
					max_fd = client;


			}
			else{
				send_msg(&clients[fd],&serverfd,&max_fd,&current_fds);
			}
		}
	}


}

void send_msg(client_t *sender, int *server, int *max_clients, fd_set *clientArr) {

	int n = recv(sender->fd,sender->buf + sender->buf_len,sizeof(sender->buf) - sender->buf_len,0);
		
	if(n == 0){
		sender->has_name > 0 ? printf("%s diconnected !\n",sender->name) : printf("Disconnected\n"); ;
		close(sender->fd);
		FD_CLR(sender->fd,clientArr);
	}
	if(n < 0){
		perror("recv");
	}

	if(sender->has_name <= 0){
		char *ch = sender->buf;
		int i = 0;
		while(*ch!='\n'){
			sender->name[i] = *ch;
			ch++;
			i++;
		}
		sender->has_name = 1;
		memset(sender->buf,0,n);
		return;
	}

	sender->buf_len +=n;
	char *newline = memchr(sender->buf,'\n',sender->buf_len);
	if(newline == NULL){
		return ;
	}

	char msg[2048];
	int len = snprintf(
			msg,
			sizeof(msg),
			"[%s] %s",
			sender->name,
			sender->buf
			);
	printf("%s",msg);
	memset(sender->buf,0,sender->buf_len);
	sender->buf_len = 0;
}

int setup_server(char *port,int backlog){
	struct addrinfo hints, *res;
	memset(&hints,0,sizeof(hints));
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	check(getaddrinfo(NULL,port,&hints,&res),"getaddrinfo");

	int sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	int yes = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));


	check(bind(sockfd,res->ai_addr,res->ai_addrlen),"bind error");

	listen(sockfd,backlog);

	return sockfd;
}

int accept_client(int serverfd){
	struct sockaddr_storage clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);
	int clientfd = accept(serverfd,(struct sockaddr *)&clientaddr,&clientaddr_len);
	check(clientfd,"Cannot accept client");
	printf("Client Connected\n");
	return clientfd;
}

void handle_connection(int clientfd,fd_set current_fds){
	char buf[8];

    int n = recv(clientfd, buf, sizeof(buf), 0);

    if (n > 0) {
	    send(clientfd,buf,n,0);
    }
    else if (n == 0) {
        printf("Client disconnected\n");

        close(clientfd);
        FD_CLR(clientfd, &current_fds);
    }
    else {
        perror("recv");
    }

}


void check(int status,char *err){
	if(status < 0){
		perror(err);	
	}

}
