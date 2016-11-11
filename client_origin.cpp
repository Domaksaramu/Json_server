#include <iostream>
#include <pthread.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include "json/json.h"

#define BUFFER_SIZE 100000000 
using namespace std;



void *receive_handler(void *arg){
	char *client_path = (char *)arg;
	int client_sockfd, server_sockfd;
	struct sockaddr_un clientaddr,serveraddr;
	socklen_t	client_len,server_len; 
	Json::Value response;
	Json::StyledWriter writer;
	char *buf = new char[BUFFER_SIZE];
	memset(buf,0,BUFFER_SIZE);
	Json::Reader reader;
	string str;
	while(1){
		client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
		if(client_sockfd<0){
			perror("socket error ");
			exit(0);
		}
		unlink(client_path);

		bzero(&clientaddr, sizeof(clientaddr));
		clientaddr.sun_family = AF_UNIX;
		strcpy(clientaddr.sun_path,strtok(client_path,"\0"));
		if(bind(client_sockfd,(struct sockaddr *)&clientaddr,sizeof(clientaddr))<0){
			perror("bind error ");
			exit(0);
		}
		client_len = sizeof(clientaddr);
		if(listen(client_sockfd,1)){
			perror("listen error");
			exit(0);
		}

		server_sockfd = accept(client_sockfd, (struct sockaddr*)&serveraddr, &server_len);
		if(server_sockfd<0){
			perror("Accept error ");
			exit(0);
		}

		if(recv(server_sockfd ,buf, BUFFER_SIZE,MSG_WAITALL)<0){
			perror("receive error ");
			exit(0);
		}
		//parsing
		cout<<buf<<endl;
		cout<<strlen(buf)<<"bytes"<<endl;
		str = buf;
		reader.parse(str,response);
		Json::Value::Members member = response.getMemberNames();
		cout<<response[*member.begin()].size()<<"rows"<<endl;
		memset(buf,0,BUFFER_SIZE);
		//str = buf;
		//cout<<writer.write(response)<<endl;
		close(server_sockfd);
		close(client_sockfd);
	}
}

int main(int argc, char **argv){
	char server_path[1024], client_path[1024];
	int server_sockfd;
	struct sockaddr_un serveraddr;
	int server_len;
	Json::Value message;
	Json::StyledWriter writer;

	strcpy(server_path, "./serverfd");
	strcpy(client_path, "./clientfd");


	//	strcpy(server_path, argv[1]);
	//	strcpy(client_path, argv[2]);

	pthread_t tid;
	pthread_create(&tid, NULL, receive_handler, (void *)client_path );


	server_sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(server_sockfd<0){
		perror("Exit ");
		exit(0);
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, strtok(server_path,"\0"));
	server_len = sizeof(serveraddr);

	char buf[1024];
	const char *temp;
	

	while(true){
		//send
		sleep(1);
		cout<<"Input: ";
		cin>>buf;
		message["sql_ID"] = buf;
		message["client"] = client_path;

		strcpy(buf, writer.write(message).c_str());

		if(sendto(server_sockfd,(void *)&buf, strlen(buf),0,(struct sockaddr *)&serveraddr,server_len)<0){
			perror("send eror ");
			exit(0);
		}
		if(message["sql_ID"]=="quit"){
			pthread_kill(tid,0);
			exit(0);
		}
	}
	close(server_sockfd);
}
