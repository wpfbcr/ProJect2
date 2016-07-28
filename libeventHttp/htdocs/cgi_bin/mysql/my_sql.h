#pragma once
#include<iostream>
#include<mysql.h>
#include<errno.h>
#include<stdlib.h>
#include<string>
#include<vector>
#include<string.h>
using namespace std;


class HttpSql{
	public:
		HttpSql();
		bool mysql_insert(string _str);
		bool mysql_select(string str);
		bool mysql_delete(string _str);
		bool mysql_modify(string str);
		bool mysql_start();
		bool mysql_commit();
		bool mysql_rollback();
		bool mysql_begin();
//		~HttpSql()
//		{
//			if(mysql!=NULL)
//			mysql_close(mysql);
//			if(res!=NULL)
//			{
//			mysql_free_result(res);
//			}
//		}	
//protected:
		bool mysql_op(string str);
	private:	
		MYSQL* mysql;
		MYSQL_RES *res;
};


