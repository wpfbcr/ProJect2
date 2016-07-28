#include<hiredis/hiredis.h>
#define _SIZE_ 1024
#include<iostream>
#include<string>
#include<vector>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
using namespace std;



int main()
{
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
	string namebuf;
	string sexbuf;
	string hobbybuf;
	redisContext *conn=redisConnect("192.168.1.75",8080);
	if(conn->err)
	{
		printf("connect error:%s\n",conn->err);
	}
	redisReply *reply = (redisReply*)redisCommand(conn, "AUTH 1234");
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	freeReplyObject(reply);
	reply=(redisReply*)redisCommand(conn,"INCR ID");
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	freeReplyObject(reply);
	reply=(redisReply*)redisCommand(conn,"GET ID");
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	
	string id=reply->str;
	
	freeReplyObject(reply);
	namebuf+="hset ";
	namebuf+="ID";
	namebuf+=id;
	namebuf+=" name ";
	namebuf+=table[2];
	
	cout<<"name::"<<namebuf<<endl;
	sexbuf+="hset ";
	sexbuf+="ID";
	sexbuf+=id;
	sexbuf+=" sex ";
	sexbuf+=table[1];
	
	
	cout<<"sex::"<<sexbuf<<endl;
	hobbybuf+="hset ";
	hobbybuf+="ID";
	hobbybuf+=id;
	hobbybuf+=" hobby ";
	hobbybuf+=table[0];
	
	cout<<"hobby::"<<hobbybuf<<endl;

	reply=(redisReply*)redisCommand(conn,namebuf.c_str());
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	freeReplyObject(reply);

	reply=(redisReply*)redisCommand(conn,sexbuf.c_str());
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	freeReplyObject(reply);
	
	reply=(redisReply*)redisCommand(conn,hobbybuf.c_str());
	if (reply->type == REDIS_REPLY_ERROR) { 
    /* Authentication failed */ 
		printf("error\n");
	}
	freeReplyObject(reply);
	redisFree(conn);
	return 0;
}



//
//	redisContext *conn=redisConnect("10.2.12.75",8080);
//	if(conn->err)
//	{
//		printf("connect error:%s\n",conn->err);
//	}
//	redisReply *reply = (redisReply*)redisCommand(conn, "AUTH 1234");
//	if (reply->type == REDIS_REPLY_ERROR) { 
//    /* Authentication failed */ 
//		printf("error\n");
//	}
//	freeReplyObject(reply);
//
//
//	reply = (redisReply*)redisCommand(conn, "keys *");
//	if (reply->type == REDIS_REPLY_ERROR) { 
//    /* Authentication failed */ 
//		printf("error\n");
//	}
//	int i;
//	int len=reply->elements;
//	vector<string> table;
//	printf("%d\n",len);
//	for(i=0;i<reply->elements;i++)
//	{
//		redisReply *childreply=reply->element[i];
//		table.push_back(childreply->str);
//		cout<<table[i]<<endl;
//	}
//	freeReplyObject(reply);
//	len--;
//	vector<string> tmp_table;
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
//		}
//		len--;
//	}
//
//
//	freeReplyObject(reply);
//
//
//}
