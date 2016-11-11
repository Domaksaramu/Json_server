#pragma once
#include <iostream>
#include "Module_controller.h"
#include "Thread_pool.h"

using namespace std;

Module_controller::Module_controller(const char* name, const char* host, const char* user, const char* passwd)
{
	//////// database 정보
	strcpy(db_name,name);
	strcpy(db_host,host);
	strcpy(db_user,user);
	strcpy(db_passwd,passwd);

	DAO = new Database_access_object(0,&stored_data,db_name,db_host,db_user,db_passwd);

	init_pre_defined_query();
	init_stored_data();

	t_pool= new Thread_pool(stored_data,this);
	
}

Json::Value Module_controller::find_unexpected_data(Json::Value request)
//stored data와 pre_defined에도 없는 예기치 못한 data 찾기
{
	Json::Value init_query;
	init_query["sql_ID"] = "init";
	init_query["sql"] = "select * from query";
	Json::Value query_result = DAO->execute_query(init_query);

	Json::Value::Members member = query_result.getMemberNames();	
	Json::StyledWriter writer;
	for(Json::Value::Members::iterator it = member.begin();it!=member.end();++it){
		if(pre_defined_querys[*it].empty()){
			pre_defined_querys[*it] = query_result[*it]["sql"];
			request_data(query_result[*it]["sql_ID"]);	
		}
	}
	Json::Value result;
	result = stored_data[request.asString()];
	return result;
}


Json::Value Module_controller::request_data(Json::Value request)
//Stored data에 없는 data 요청하는 함수
{
		
	Json::Value sql_info =  find_pre_defined_query(request);
	
	Json::Value query_result = DAO->execute_query(sql_info);
	Json::StyledWriter writer;
//	if(!query_result.empty())
		stored_data[sql_info["sql_ID"].asString()]=query_result;

	return query_result;
}
Json::Value Module_controller::find_pre_defined_query(Json::Value request)
{
	Json::Value result;
	if(!request.empty()){
		result["sql_ID"] = request;
		if(!pre_defined_querys[request.asString()].isNull()){
			result["sql"] = pre_defined_querys[request.asString()];
		}
	}
	return result;
}
void Module_controller::init_pre_defined_query()
{
	Json::Value init_query;
	init_query["sql_ID"] = "init";
	init_query["sql"] = "select * from query";
	Json::Value result = DAO->execute_query(init_query);

	Json::Value::Members member = result.getMemberNames();	
	Json::StyledWriter writer;
	for(Json::Value::Members::iterator it = member.begin();it!=member.end();++it){//field name 가져오는법 1, vector 쓰는법 
		pre_defined_querys[*it]=result[*it]["sql"];
	}
	cout<<writer.write(pre_defined_querys)<<endl;
}
void Module_controller::init_stored_data()
{
	for(Json::ValueIterator itr=pre_defined_querys.begin(); itr != pre_defined_querys.end(); itr++){
		Json::Value request = itr.key().asString();//field name 가져오는법 2
		request_data(request);				
	}
}

Module_controller::~Module_controller()
{
	stored_data.clear();
	delete t_pool;
	delete DAO;
}

