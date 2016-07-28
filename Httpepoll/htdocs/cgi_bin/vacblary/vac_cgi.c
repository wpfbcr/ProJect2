#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include"bigdata.h"
#define _SIZE_ 1024
#define _DATA_ 20
#define _SUM_ 40

void bigdata(char buf[],char *query_string)
{
	char *data[_DATA_];
	int data1=0;
	int data2=0;
	int int_sum=0;
	char *p=query_string+strlen(query_string);
	int index=0;
	while(p!=query_string)
	{
		--p;
		if(*p=='&')
		{
			*p='\0';
		}
		if(*p=='=')
		{
			++p;
			data[index++]=p;
			--p;
		}
	}
	int i=0;
	data1=atoi(data[0]);
	data2=atoi(data[1]);
	int_sum=data1+data2;
	sprintf(buf,"%d",int_sum);
}
int main()
{
	char *query_string=(char *)malloc(_SIZE_);
	char buf[_SIZE_];
	char *method=(char *)malloc(_SIZE_);
	char *content_length=(char *)malloc(_SIZE_);
	
	memset(buf,'\0',_SIZE_);
	memset(query_string,'\0',_SIZE_);
	memset(method,'\0',_SIZE_);
	memset(content_length,'\0',_SIZE_);


	if(getenv("REQUEST_METHOD"))
	{
		method=getenv("REQUEST_METHOD");
	}else{
		method="\0";
	}


	if(!strncmp(method,"GET",3))
	{
		if(getenv("QUERY_STRING"))
		{
			query_string=getenv("QUERY_STRING");
		}else{
			query_string="\0";
		}
	}
	else{
	}
	if(getenv("CONTENT_LENGTH"))
	{
		content_length=getenv("CONTENT_LENGTH");
		int index=0;
		char ch;
		int len=atoi(content_length);
		while(len>0&&read(0,&ch,1)>0){
			query_string[index++]=ch;
			len--;
		}
	}
	bigdata(buf,query_string);
	printf("sum=%s\n",buf);
	
//	printf("hello\n");
//	char *content=(char *)malloc(10);
//
//	if(getenv("CONTENT_LENGTH"))
//	{
//		content=getenv("CONTENT_LENGTH");
//		printf("contend%s\n",content);
//	}
//	int len=atoi(content);
//	printf("len%d\n",len);
//	char ch='a';
//	char buf[_SIZE_];
//	memset(buf,'\0',sizeof(buf));
//	ssize_t _size=-1;
//	int index=0;
//	while(len>0)
//	{
//		_size=read(0,&ch,1);
//		printf("c=%c\n",ch);
//		if(_size>0)
//		{
//			buf[index++]=ch;
//		}
//		len--;
//	}
//	printf("buf=%s\n",buf);
//	return 0;
}
