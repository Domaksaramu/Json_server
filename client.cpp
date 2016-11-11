#include <iostream>
#include <time.h>
#include "client_process.h"
#include "json/json.h"
#define BUFFER_SIZE 655360
using namespace std;

int main(int argc, char **argv){
	client_process *c1;
	c1 = new client_process("./serverfd", "./clientfd1");
	sleep(1);
	c1->send_buffer("q1");
	sleep(1);
	c1->send_buffer("q2");
	sleep(1);
	c1->send_buffer("q3");
	sleep(1);
	c1->send_buffer("q4");
	sleep(1);
	c1->send_buffer("q5");
	sleep(1);
	c1->send_buffer("q6");
	sleep(1);
	c1->send_buffer("q7");
	sleep(1);
	c1->send_buffer("q8");
	sleep(1);
	c1->send_buffer("q9");
	sleep(1);
	c1->send_buffer("q10");
	sleep(3);

	delete c1;
/*

	char server_path[20] = "./serverfd";
	char default_path[20] = "./clientfd";
	char client_path[20];
	client_process *c[30];
	int pid,i, thread_num=30;
	for(i=0;i<thread_num;i++){
		sprintf(client_path,"%s%d",default_path,i);
		pid=fork();
		if(pid==0){
			break;
		}
	}
	if(pid==0){
		c[i]=new client_process(server_path, client_path);
		sleep(1);
		//sprintf(client_path,"%s%d","x",i+1);
		//c[i]->send_buffer(client_path);
		c[i]->send_buffer("q10");
	}*/
	sleep(10);
	//client_process c2("./SERVERDF","./CLIENTFD");
	//client_process c3("./SERVERDF","./CLIENTFD");
	
}
