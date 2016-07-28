#include<iostream>
#include<string>
#include<vector>
#include<string.h>
#include"my_sql.h"
using namespace std;
int main()
{
	HttpSql de;
	string str="DELETE FROM test_info WHERE name=";
	str+='"';
	//cout<<str<<endl;
	string del;
	string _str;
	char *buf=(char *)malloc(64);
	memset(buf,'\0',sizeof(buf));
	if(getenv("QUERY_STRING"))
	{
		del=getenv("QUERY_STRING");
	}
	strcpy(buf,del.c_str());
	int size=del.size();
	while(size>0)
	{
		if(buf[size]=='=')
		{
			_str=&buf[size+1];
		}
		size--;
	}
	_str+='"';
	str+=_str.c_str();
	//cout<<str<<endl;
	if(de.mysql_delete(str)<0)
	{
		cout<<"delete failure"<<endl;
	}else
	{
		cout<<"delete success"<<endl;
	}
	return 0;
}
