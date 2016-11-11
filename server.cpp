#include <iostream>
#include <mysql++/mysql++.h>
#include "Module_controller.h"
#include "Thread_pool.h"
#define DB_NAME "world"
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASSWD "hwoml8075"

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


using namespace std;

int main(int argc, char** argv){
	//bind
	//listen
	//acept
	Module_controller *module_controller;
	module_controller = new Module_controller(DB_NAME,DB_HOST,DB_USER,DB_PASSWD);
	Json::Value temp;
	Json::StyledWriter writer;



	char server_path[1024];
	int server_sockfd;
	struct sockaddr_un serveraddr;
	int server_len; 
	Json::Value request;
	char buf[1024];
	Json::Reader reader;
	string str;


	strcpy(server_path,"./serverfd");
	//strcpy(server_path,argv[1]);
	
	server_sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(server_sockfd<0){
		perror("socket error ");
		exit(0);
	}   
	unlink(server_path);

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path,strtok(server_path,"\0"));
	if(bind(server_sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
		perror("bind error ");
		exit(0);
	}   
	server_len = sizeof(serveraddr);


	while(true){
		memset(buf,0,1024);
		if(recvfrom(server_sockfd ,(void *)&buf, sizeof(buf),0,(struct sockaddr *)&serveraddr, (socklen_t *)&server_len)<0){
			perror("receive error ");
			exit(0);
		}
		//parsing
		str = buf;

		cout<<"receive"<<endl;
		cout<<buf<<endl;
		reader.parse(str,request);
		if(request["sql_ID"].asString()=="quit"){
			delete module_controller;
			close(server_sockfd);
			exit(0);
		}
		module_controller->t_pool->assign_request(request);
		//memset(buf,0,sizeof(buf));
//		cout<<writer.write(request)<<endl;
	}
	//	module_controller->t_pool->assign_request(temp[i]);

}
