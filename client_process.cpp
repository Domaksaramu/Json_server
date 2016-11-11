#pragma once
#include "client_process.h"
#include <fstream>
#include <cstdio>
#include <signal.h>
using namespace std;

client_process::client_process(const char *serverfd, const char *clientfd){
	strcpy(server_path,serverfd);
	strcpy(client_path,clientfd);
	
	pthread_create(&tid, NULL, receive_handler, (void *)this );
	cout<<"client process is created"<<endl;

	buffer = new char[BUFFER_SIZE];
	server_sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(server_sockfd<0){
		perror("Exit ");
		exit(0);
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, strtok(server_path,"\0"));
	server_len = sizeof(serveraddr);

	cout<<client_path<<endl;
	cout<<server_path<<endl;
}
int client_process::send_buffer(const char *temp){
	int result;
	char buf[100];
	Json::Value message;
	Json::StyledWriter writer;
	
	clock_gettime(CLOCK_REALTIME,&begin);
	strcpy(buf,temp);
	
	cout<<"send "<<buf<<endl;
	message["sql_ID"] = buf;
	message["client"] = client_path;
	strcpy(buf, writer.write(message).c_str());

	if(result = sendto(server_sockfd,(void *)&buf, strlen(buf),0,(struct sockaddr *)&serveraddr,server_len)<0){
		perror("send eror ");
	}
	if(message["sql_ID"]=="quit"){
		close(server_sockfd);
		pthread_kill(tid,0);
		exit(0);
	}
	return result;
}
client_process::~client_process(){
	close(server_sockfd);
	pthread_kill(tid,0);
	cout<<"client process is deleted"<<endl;
}


void* client_process::receive_handler(void *arg){
	client_process *cli = (client_process*)arg;
	char *cli_path = cli->client_path;
	cli->recv_cnt = 0;
	string str;	
	ofstream fout;
	int len;
	while(1){
		memset(cli->buffer,0,BUFFER_SIZE);
		cli->cli_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
		if(cli->cli_sockfd<0){
			perror("socket error ");
			exit(0);
		}
		unlink(cli_path);

		bzero(&cli->cliaddr, sizeof(cli->cliaddr));
		cli->cliaddr.sun_family = AF_UNIX;
		strcpy(cli->cliaddr.sun_path,strtok(cli_path,"\0"));
		if(bind(cli->cli_sockfd,(struct sockaddr *)&cli->cliaddr,sizeof(cli->cliaddr))<0){
			perror("bind error ");
			exit(0);
		}
		cli->cli_len = sizeof(cli->cliaddr);
		if(listen(cli->cli_sockfd,1)){
			perror("listen error");
			exit(0);
		}
		cli->srv_sockfd = accept(cli->cli_sockfd, (struct sockaddr*)&cli->srvaddr, &cli->srv_len);
		if(cli->srv_sockfd<0){
			perror("Accept error ");
			exit(0);
		}

		if(len = recv(cli->srv_sockfd ,cli->buffer, BUFFER_SIZE,MSG_WAITALL)<0){
			perror("receive error ");
			exit(0);
		}
		
		str = cli->buffer;
		cli->reader.parse(str,cli->response);
		clock_gettime(CLOCK_REALTIME,&(cli->end));
		cli->diff = 1000000000L*(cli->end.tv_sec-cli->begin.tv_sec)+cli->end.tv_nsec-cli->begin.tv_nsec;
		long double tt = (long double)cli->diff/1000000;
	
		cout.precision(4);
		int len = strlen(cli->buffer);
//		cout<<"read "<<len<<"bytes"<<endl;
//		if(len<10)
//			cout<<cli->buffer<<endl;

		cout<<fixed<<tt<<endl;
		close(cli->srv_sockfd);
		close(cli->cli_sockfd);
		cli->response.clear();
	}
}

