#include"my_sql.h"

using namespace std;


int main()
{
	HttpSql sql;
	string str;
	string query_string;
	vector<string> table;
	char buf[1024];
	char name[20];
	char sex[20];
	char hobby[30];

	memset(buf,'\0',sizeof(buf));
	memset(name,'\0',sizeof(name));
	memset(sex,'\0',sizeof(sex));
	memset(hobby,'\0',sizeof(hobby));
	if(getenv("QUERY_STRING"))
	{
		query_string=getenv("QUERY_STRING");
	}else{
		query_string="\0";
	}
	strcpy(buf,query_string.c_str());
	int size=strlen(buf);
	while(size>0)
	{
		if(buf[size]=='&')
		{
			buf[size]='\0';
		}
		else if(buf[size]=='=')
		{
			table.push_back(&buf[size+1]);
		}
		size--;
	}
	strcpy(hobby,table[0].c_str());
	if(hobby[0]!='\0')
	{
		str+="name=\"";
		str+=hobby;
		str+="\"";
	}
	strcpy(sex,table[1].c_str());
	if(sex[0]!='\0')
	{
		if(!str.empty())
		{
			str+=" and ";
		}
		str+="sex=\"";
		str+=sex;
		str+="\"";
	}
	strcpy(name,table[2].c_str());
	if(name[0]!='\0')
	{
		if(!str.empty())
		{
			str+=" and ";
		}
		str+="name=\"";
		str+=name;
		str+="\"";
	}

	sql.mysql_select(str);
	
	
	return 0;
}
