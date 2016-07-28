#include<iostream>
#include<string>
#include<vector>
#include<string.h>
#include"my_sql.h"
#include<stdio.h>
using namespace std;



int main()
{
	HttpSql sql;
	vector<string> table;
	string query_string;
	char *method=(char *)malloc(10);
	string content_length;
	char buf[2048];
	memset(buf,'\0',2048);
	if(getenv("REQUEST_METHOD"))
	{
		method=getenv("REQUEST_METHOD");
	}
	cout<<"method"<<method<<endl;
	if(!strncasecmp(method,"get",3))
	{
		cout<<"get"<<endl;
		if(getenv("QUERY_STRING"))
		{
			query_string=getenv("QUERY_STRING");
		}
		else{
				query_string="\0";
		}
	}
	else
	{
		cout<<"post"<<endl;
		if(getenv("CONTENT_LENGTH"))
		{
			content_length=getenv("CONTENT_LENGTH");
		int len=atoi(content_length.c_str());
		char ch;
		int index=0;
		while(len>0&&read(0,&ch,1)>0)
		{
			buf[index++]=ch;
			len--;
		}
		}
		else{
			buf[0]='\0';
		}
		query_string=buf;
	}
	memset(buf,'\0',sizeof(buf));
	strcpy(buf,query_string.c_str());

	int size=query_string.size();
	int i=0;
	while(size>=0)
	{
		if(buf[size]=='=')
		{
			table.push_back(&buf[size+1]);
		}
		if(buf[size]=='&')
		{
			buf[size]='\0';
		}

		size--;
	}
//	for(int i=0;i<table.size();i++)
//	{
//		cout<<table[i]<<endl;
//	}
	string table_str="INSERT INTO test_info";
	table_str+="(name,sex,hobby)VALUES('";
	table_str+=table[2];
	table_str+="','";
	table_str+=table[1];
	table_str+="','";
	table_str+=table[0];
	table_str+="')";
	//cout<<table_str<<endl;
	if(sql.mysql_insert(table_str))
	{
		cout<<"insert success"<<endl;
	}else
	{
		cout<<"insert failure"<<endl;
	}
	return 0;
}



