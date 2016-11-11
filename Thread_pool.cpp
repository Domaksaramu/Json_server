#pragma once
#include "Thread_pool.h"
#include "Module_controller.h"
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

using namespace std;
Thread_pool::Thread_pool(Json::Value& stored_data, Module_controller *m_controller){
	this->stored_data = stored_data;
	this->m_controller = m_controller;
	init_Threads();
	buf = new char[BUFFER_SIZE];
}

Thread_pool::~Thread_pool(){
	while(!request_queue.empty())
		request_queue.pop();
	for(int i=0;i<THREAD_NUM;i++)
		pthread_kill(t_ids[i],0);
}
pthread_mutex_t Thread_pool::q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Thread_pool::q_cond = PTHREAD_COND_INITIALIZER;


void Thread_pool::init_Threads(){
	for(int i=0;i<THREAD_NUM;i++){
		pthread_create(&t_ids[i],NULL,Thread_pool::thread_handler,(void*)this);
	}
}

bool Thread_pool::load_request(Json::Value &response){
	pthread_mutex_lock(&q_lock);
	while(request_queue.empty())
		pthread_cond_wait(&q_cond, &q_lock);
	response = request_queue.front();
	request_queue.pop();
	pthread_mutex_unlock(&q_lock);
	return true;
}
void Thread_pool::assign_request(Json::Value message){
	Json::StyledWriter writer;
	pthread_mutex_lock(&q_lock);
	//cout<<"Assign request:"<<endl;
	//cout<<writer.write(message)<<endl;
	request_queue.push(message);
	pthread_mutex_unlock(&q_lock);
	pthread_cond_signal(&q_cond);
}
void* Thread_pool::thread_handler(void* pArg){
	Json::Value temp, response;
	Json::StyledWriter writer;
	Thread_pool *p = (Thread_pool*)pArg;

	//cout<<"execute thread"<<endl;
	while(true){
		p->load_request(temp);
		//cout<<"load request:"<<endl;
		response[temp["sql_ID"].asString()] = Json::Value( p->stored_data[temp["sql_ID"].asString()]);
		if(response[temp["sql_ID"].asString()].empty()){
			response[temp["sql_ID"].asString()]=p->m_controller->find_unexpected_data(temp["sql_ID"]);
			//cout<<"empty"<<endl;
		}
		memset(p->buf,0,BUFFER_SIZE);
		p->send_response(temp,response);
	//	response.clear();
	}	
}
void Thread_pool::send_response(Json::Value message, Json::Value &response){
	char client_path[1024];
	int client_sockfd;
	struct sockaddr_un clientaddr;
	socklen_t client_len;
	Json::StyledWriter writer;
	//memset(buf,0,BUFFER_SIZE);

	const char *temp;
	
	strcpy(client_path, message["client"].asString().c_str());
	strcpy(buf,writer.write(response).c_str());

	client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(client_sockfd<0){
		perror("socket errer ");
		exit(0);
	}
	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sun_family = AF_UNIX;
	strcpy(clientaddr.sun_path, client_path);
	client_len = sizeof(clientaddr);

	if(connect(client_sockfd,(struct sockaddr *)&clientaddr, client_len)<0){
		perror("connect error ");
		return;
	}

	int buffer_length = strlen(buf);
	if(buffer_length==0){
		strcpy(buf,"null");
		buffer_length = 5;
	}
	int len;
	len=send(client_sockfd, buf, buffer_length,0);
	cout<<"send "<<len<<endl;
	response.clear();
	close(client_sockfd);
}
