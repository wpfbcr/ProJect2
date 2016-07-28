#include<iostream>
#include<string>
#include<string.h>
#include<vector>
#include<hiredis/hiredis.h>
#include"my_sql.h"







int main()
{
	HttpSql::HttpSql sql;
	redisContext *conn=redisConnect("192.168.1.75",8080);
	if(conn->err)
	{
		printf("connect error:%s\n",conn->err);
		exit(2);
	}
	redisReply *reply = (redisReply*)redisCommand(conn, "AUTH 1234");
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	freeReplyObject(reply);
	reply=(redisReply*)redisCommand(conn,"get ID");
	if (reply->type==REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	if(reply->str==NULL)
	{
		cout<<"NULL"<<endl;
		exit(1);
	}
	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(conn, "keys ID*");
	if (reply->type == REDIS_REPLY_ERROR ) { 
    /* Authentication failed */ 
		printf("error\n");
	}
//	if(reply->type==REDIS_REPLY_NIL)
//	{
//		cout<<NULL<<endl;
//		exit(1);
//	}
	int i;
	int len=reply->elements;
	vector<string> table;
	//printf("%d\n",len);
	for(i=0;i<len;i++)
	{
		redisReply *childreply=reply->element[i];
		table.push_back(childreply->str);
		cout<<table[i]<<endl;
	}
	freeReplyObject(reply);
	len-=2;
	vector<string> tmp_table;
	string table_str;
	while(len>=0)
	{
		table_str.empty();
		tmp_table.erase(tmp_table.begin(),tmp_table.end());
		string str;
		str+="hgetall ";
		if(!strncmp(table[len].c_str(),"ID",3))
		{
			len--;
			continue;
		}
		str+=table[len];
		printf("%s\n",str.c_str());
		int i=0;
		reply = (redisReply*)redisCommand(conn,str.c_str());
		if (reply->type == REDIS_REPLY_ERROR) { 
    	/* Authentication failed */ 
			printf("error\n");
		}
		for(i=0;i<reply->elements;i++)
		{
			redisReply*child=reply->element[i];
			tmp_table.push_back(child->str);
		//	printf("str->%s\n",child->str);
		}

		table_str="INSERT INTO test_info";
		table_str+="(name,sex,hobby)VALUES('";
		table_str+=tmp_table[1];
		table_str+="','";
		table_str+=tmp_table[3];
		table_str+="','";
		table_str+=tmp_table[5];
		table_str+="')";
		//cout<<table_str<<endl;
		if(sql.mysql_insert(table_str))
		{}else
		{
			//printf_log();
		}
		cout<<"insert->"<<table_str<<"success"<<endl;
		len--;
		freeReplyObject(reply);
	}
	reply=(redisReply*)redisCommand(conn,"flushdb");
	freeReplyObject(reply);
	return 0;
}





















//}

//	while(len>=0)
//	{
//		str.empty();
//		str+="hgetall ";
//		tmp+=table[len];
//		str+=tmp;
//		reply=(redisReply *)redisCommand(conn,str.c_str());
//		if(reply->type == REDIS_REPLY_ERROR)
//		{
//			cout<<strerror(errno)<<endl;
//			//;cout<<reply->type  defalut
//		}
//		tmp_len=reply->elements;
//		cout<<"tmp_len::"<<tmp_len<<endl;
//		for(int i=0;i<tmp_len;i++)
//		{
//			child=reply->element[i];
//			tmp_table.push_back(child->str);
//			cout<<"tmp_table"<<child->str<<endl;
//			freeReplyObject(child);
//		}
//		string table_str="INSERT INTO test_info";
//		table_str+="(name,sex,hobby)VALUES('";
//		table_str+=tmp_table[1];
//		table_str+="','";
//		table_str+=tmp_table[3];
//		table_str+="','";
//		table_str+=tmp_table[5];
//		table_str+="')";
//		//cout<<table_str<<endl;
//		if(sql.mysql_insert(table_str))
//		{
//		
//		}else
//		{
//			//printf_log();
//		}
//		len--;	

//		freeReplyObject(reply_tmp);
//	}

//}

//	freeReplyObject(reply);
//	return 0;

//}






//int main()

//	HttpSql sql;

//	vector<string> table;
//	string str;
//	redisContext * conn=redisConnect("10.2.12.75",8080);
//	if(conn->err)
//	{	
//		perror("redisConnect");
//		redisFree(conn);
//	}
//
//	redisReply *reply=(redisReply *)redisCommand(conn,"AUTH 1234");
//	if(reply==NULL)
//	{
//		perror("redisCommand");
//		freeReplyObject(reply);
//	}
//	
//	reply=(redisReply *)redisCommand(conn,"keys ID*");
//	 if(reply->type== REDIS_REPLY_ERROR)
//	{
//		exit(1);
//	}
//	
//	redisReply * child;
//	int len=reply->elements;
//	for(int i=0;i<len;i++)
//	{	
//		child=reply->element[i];
//		table.push_back(child->str);
//		cout<<"table"<<child->str<<endl;
//	}
//	freeReplyObject(reply);
//	freeReplyObject(child);
//	string tmp;
//	vector<string> tmp_table;
//	int tmp_len;
//	len-=2;
//	
//	while(len>=0)
//	{
//		string str;
//		str+="hgetall ";
//		str+=table[len];
//		printf("%s\n",str.c_str());
//		int i=0;
//		reply = (redisReply*)redisCommand(conn,str.c_str());
//		if (reply->type == REDIS_REPLY_ERROR) { 
//    	/* Authentication failed */ 
//			printf("error\n");
//		}
//		for(i=0;i<reply->elements;i++)
//		{
//			redisReply *child=reply->element[i];
//			tmp_table.push_back(child->str);
//			printf("str->%s\n",child->str);
//			freeReplyObject(child);
//		}
//	}
//		string table_str="INSERT INTO test_info";
//		table_str+="(name,sex,hobby)VALUES('";
//		table_str+=tmp_table[1];
//		table_str+="','";
//		table_str+=tmp_table[3];
//		table_str+="','";
//		table_str+=tmp_table[5];
//		table_str+="')";
//		//cout<<table_str<<endl;
//		if(sql.mysql_insert(table_str))
//		{
//		
//		}else
//		{
//			//printf_log();
//		}
//		len--;
//	}
//

//	freeReplyObject(reply);


//}

//	while(len>=0)
//	{
//		str.empty();
//		str+="hgetall ";
//		tmp+=table[len];
//		str+=tmp;
//		reply=(redisReply *)redisCommand(conn,str.c_str());
//		if(reply->type == REDIS_REPLY_ERROR)
//		{
//			cout<<strerror(errno)<<endl;
//			//;cout<<reply->type  defalut
//		}
//		tmp_len=reply->elements;
//		cout<<"tmp_len::"<<tmp_len<<endl;
//		for(int i=0;i<tmp_len;i++)
//		{
//			child=reply->element[i];
//			tmp_table.push_back(child->str);
//			cout<<"tmp_table"<<child->str<<endl;
//			freeReplyObject(child);
//		}
//		string table_str="INSERT INTO test_info";
//		table_str+="(name,sex,hobby)VALUES('";
//		table_str+=tmp_table[1];
//		table_str+="','";
//		table_str+=tmp_table[3];
//		table_str+="','";
//		table_str+=tmp_table[5];
//		table_str+="')";
//		//cout<<table_str<<endl;
//		if(sql.mysql_insert(table_str))
//		{
//		
//		}else
//		{
//			//printf_log();
//		}
//		len--;	

//		freeReplyObject(reply_tmp);
//	}

//}
