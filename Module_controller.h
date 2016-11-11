#pragma once
#include "json/json.h"
#include <cstring>
#include "Database_access_object.h"
//#include "thread_control.h"
#include <string>

class Thread_pool;

class Module_controller
{
	public:
		//////////////////functions
		Module_controller(const char* name,const char* host, const char* user, const char* passwd);
		//stored_data 인자로 추가
		
		Json::Value find_unexpected_data(Json::Value request);
		//stored data와 pre_defined_querys에도 없는 data	
		
		Json::Value request_data(Json::Value not_found);
		//Stored data에 없는 data 요청하는 함수
		
		Json::Value find_pre_defined_query(Json::Value request);
		//

		void init_pre_defined_query();

		void init_stored_data(); 

		void push_request(Json::Value request);
		void push_request(std::string request);
		void pop_request();

		~Module_controller();

		
		////////////////variables
		char db_name[100];
		char db_host[100];
		char db_user[100];
		char db_passwd[100];


		int client_list;
		Json::Value pre_defined_querys; 
		Json::Value stored_data;
		Database_access_object *DAO;
		Thread_pool *t_pool;
};
