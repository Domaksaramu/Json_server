#pragma once
#include "Database_access_object.h"
#include <iostream>
#include <exception>
using namespace std;
Database_access_object::Database_access_object (pthread_t id, Json::Value *stored_data, char* db_name, char* db_host, char* db_user, char* db_passwd)
{
	if(!mysqlpp_conn.connect(db_name, db_host, db_user, db_passwd)){
		std::cout<<"Cannot access Database"<<std::endl;
	}
}
Json::Value Database_access_object::execute_query(Json::Value sql_info)
{
	string sql = sql_info["sql"].asString();
	mysqlpp::Query query =  mysqlpp_conn.query(sql);
	
	mysqlpp::StoreQueryResult res;
  
	Json::Value res_fields;
	Json::Value res_data; 
	try{
		res = query.store();
	}
	catch(exception& e){
		return res_data;
	}

	mysqlpp::Field field;
	Json::Value row;
	size_t num_rows, num_fields, j;
	int i, k;
   	string primary_key;
	int primary_count = 0;
	if(res){
		num_rows = res.num_rows();
		num_fields = res.num_fields();
		
		for(i=0;i<num_fields;i++){
			field= res.fetch_field();
			res_fields.append(field.name());
			if(field.primary_key()){
				primary_key = field.name();
				primary_count++;				
			}
		}
		if(primary_count==1){
			for(j= 0; j<num_rows;j++){
				for(k = 0;k<num_fields;k++){
					row[res_fields[k].asString()] = (string)res[j][k];
				}

				res_data[row[primary_key].asString()] = row;
			}
		}
		else{
			for(j= 0; j<num_rows;j++){
				for(k = 0;k<num_fields;k++){
					row[res_fields[k].asString()] = (string)res[j][k];
				}

				res_data.append(row);
			}
		}
	}
	return res_data;	
}
Database_access_object::~Database_access_object(){
	mysqlpp_conn.disconnect();
}
//void Database_access_object::store_data(Json::Value json_result){
//}
