#include"my_sql.h"



//int main()
//{
//	HttpSql sql;
//	string str="";
//	string query_string;
//	vector<string> table;
//	char buf[1024];
//	char name[20];
//	char sex[20];
//	char hobby[30];
//	char which[30];
//
//	memset(buf,'\0',sizeof(buf));
//	memset(which,'\0',sizeof(which));
//	memset(name,'\0',sizeof(name));
//	memset(sex,'\0',sizeof(sex));
//	memset(hobby,'\0',sizeof(hobby));
//	if(getenv("QUERY_STRING"))
//	{
//		query_string=getenv("QUERY_STRING");
//	}else{
//		query_string="\0";
//	}
//	strcpy(buf,query_string.c_str());
//	int size=strlen(buf);
//	while(size>0)
//	{
//		if(buf[size]=='&')
//		{
//			buf[size]='\0';
//		}
//		else if(buf[size]=='=')
//		{
//			table.push_back(&buf[size+1]);
//		}
//		size--;
//	}
//	
//	
//
//	strcpy(hobby,table[0].c_str());
//	if(hobby[0]!='\0')
//	{
//		str+="name=\"";
//		str+=hobby;
//		str+="\"";
//	}
//
//	strcpy(sex,table[1].c_str());
//	if(sex[0]!='\0')
//	{
//		if(!str.empty())
//		{
//			str+=',';
//		}
//		str+="sex=\"";
//		str+=sex;
//		str+="\"";
//	}
//
//	strcpy(name,table[2].c_str());
//	if(name[0]!='\0')
//	{
//		if(!str.empty())
//		{
//			str+=',';
//		}
//		str+="name=\"";
//		str+=name;
//		str+="\"";
//	}
//
//	strcpy(which,table[3].c_str());
//	if(which[0]!='\0')
//	{
//		if(!str.empty())
//		{
//			str+=" where ";
//		}else{
//			cout<<"please write which name you want to modify"<<endl;
//			return 0;
//		}
//		str+="name=\"";
//		str+=which;
//		str+="\"";
//	}
//
//	if(sql.mysql_modify(str)<0)
//	{
//		cout<<"modify failure"<<endl;
//	}
//	else{
//		cout<<"modify success";
//	}
//
//	return 0;
//}
//
	
int main()
{
	HttpSql sql;
	if(!sql.mysql_start())
	{
		return -1;
	}
	char buf1[1024];
	char buf2[1024];
	memset(buf1,'\0',sizeof(buf1));
	memset(buf2,'\0',sizeof(buf2));
	strcpy(buf1,"update test_info set name=\"wangmazi\" where name=\"zhangsan\"");
	strcpy(buf2,"update test_info set name=\"wangmazi\" where name=\"lisi\"");
	string str1(buf1);
	string str2(buf2);
	if(!sql.mysql_begin())
	{
		return -1;
	}
	cout<<"asdfas1"<<endl;
	int ret1=sql.mysql_op(str1);
	int ret2=sql.mysql_op(str2);
	cout<<"asdfas"<<endl;
	//ret1=false;
	if(ret1&&ret2)
	{
		if(sql.mysql_commit())
		{
			cout<<"modify success"<<endl;
		}
		else{
			cout<<"modify failure"<<endl;
		}
	}else
	{
		if(sql.mysql_rollback())
		{
			cout<<"modify failure roolback success"<<endl;
		}
		else{
			cout<<"boom!!!1"<<endl;
		}
	}
	

}
