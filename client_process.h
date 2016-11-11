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
#include <time.h>
#include "json/json.h"

#define BUFFER_SIZE 100000000

class client_process{
	public:
		char server_path[104];
		char client_path[104];
		char *buffer;
		int server_sockfd;
	    struct sockaddr_un serveraddr;
	    int server_len;
		timespec begin;
		timespec end;
		uint64_t diff;

		int cli_sockfd, srv_sockfd;
		struct sockaddr_un cliaddr,srvaddr;
		socklen_t cli_len,srv_len; 
		Json::Value response;
		int recv_cnt;
		Json::Reader reader;

		pthread_t tid;
////////// 수정할것

		client_process(const char *serverfd, const char *clientfd);
		static void* receive_handler(void *arg);
		int send_buffer(const char* temp);
		~client_process();

};
