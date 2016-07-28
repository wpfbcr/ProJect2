#include<iostream>
#include<stdlib.h>
#include<event.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#define _BUF_SIZE_ 1024
#define _METHOD_SIZE_ 1024
#define _URL_SIZE_ 1024
#define EVLOOP_NO_EXIT_ON_EMPTY 0x04

using  namespace std;
struct event_base *base;

void printf_log(const char *function,int line,char *error)
{
	int fd=open("./log/start.log",O_APPEND);
	close(1);
	dup2(fd,1);
	close(fd);
	printf("%s,%d,%s\n",function,line,error);
}

bool nonblock(int sock)
{
	int fl=fcntl(sock,F_GETFL);
	if(fcntl(sock,F_SETFL,fl|O_NONBLOCK)<0)
	{
		return false;
	}
	return true;
}

int Listen(char *ip,int port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		cout<<strerror(errno)<<endl;
	}
	struct sockaddr_in local;
	local.sin_addr.s_addr=inet_addr(ip);
	local.sin_port=htons(port);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		cout<<strerror(errno)<<endl;
	}

	if(listen(sock,64)<0)
	{
		cout<<strerror(errno)<<endl;
	}
	return sock;
}
	
	int get_line(int sock,char *buf,int size)
	{
		printf("getline,sock=%d\n",sock);
		char ch='a';
		int index=0;
		int _s=0;
		while(ch!='\n'&&index<size-1)
		{
			_s=recv(sock,&ch,1,0);
			if(_s>0)
			{
				if(ch=='\r')
				
				{
					if(recv(sock,&ch,1,MSG_PEEK))
					{
						if(_s>0&&ch=='\n')
						recv(sock,&ch,1,0);
					}
				}
				buf[index++]=ch;
			}
			else{
				buf[index++]='\n';
				break;
			}
		}
		buf[index]='\0';
		return index;
	}
	void clear_head(int sock)
	{
		char buf[2048];
		int size=1;
		do{
		memset(buf,'\0',sizeof(buf));
		size=-1;
		size=get_line(sock,buf,2048);
		}while(buf[0]!='\n'&&size>0);
	}

	void ret_html(int sock,char *method,const char *path,int size)
	{
		char buf[_BUF_SIZE_];
		memset(buf,'\0',sizeof(buf));
		strcpy(buf,"HTTP/1.0 200 ok\r\n\r\n");
		send(sock,buf,strlen(buf),0);
		int fd=open(path,O_RDONLY);

		ssize_t _size=sendfile(sock,fd,NULL,size);
		if(_size<0)
		{
			return;
		}
	}
	
	void ret_cgi(int sock,char *method,char *path,char *query_string)
	{
		printf("method%s\n",method);
		char method_env[1024];
		char query_string_env[1024];
		char buf[1024];
		char content_length[1024];
		char length_env[1024];
		memset(content_length,'\0',sizeof(content_length));
		memset(length_env,'\0',sizeof(length_env));
		memset(method_env,'\0',sizeof(method_env));
		memset(query_string_env,'\0',sizeof(query_string_env));
		memset(buf,'\0',sizeof(buf));
		if(!strncasecmp(method,"POST",4))
		{
			while(get_line(sock,buf,sizeof(buf))>0)
			{
				if(!strncasecmp(buf,"CONTENT-LENGTH",14))
				{
					break;
				}
				memset(buf,'\0',sizeof(buf));
			}
			strcpy(content_length,buf+15);
			memset(buf,'\0',sizeof(buf));
			sprintf(length_env,"CONTENT_LENGTH=%s",content_length);
			putenv(length_env);
			clear_head(sock);
		}else
		{
			char *p=query_string+strlen(query_string);
			while(*p!=' ')
			{
				--p;
			}
			*p='\0';
			sprintf(query_string_env,"QUERY_STRING=%s",query_string);
			putenv(query_string_env);
		}
		sprintf(method_env,"REQUEST_METHOD=%s",method);
		putenv(method_env);

		//fork ready
		int cgi_fd[2];
		int server_fd[2];
		if(pipe(cgi_fd)<0)
		{

			printf_log(__FUNCTION__,__LINE__,strerror(errno));
			return ;
		}
		if(pipe(server_fd)<0)
		{
			printf_log(__FUNCTION__,__LINE__,strerror(errno));
			return ;
		}
		pid_t id;
		id=fork();
		if(id<0)
		{
			printf_log(__FUNCTION__,__LINE__,strerror(errno));
		}else if(id==0)//child cgi 
		{	
			cout<<"id==0"<<path<<endl;
			close(server_fd[0]);
			close(cgi_fd[1]);
			dup2(server_fd[1],1);
			dup2(cgi_fd[0],0);
			
			execl(path,"vac_cgi",NULL);

			close(server_fd[1]);
			close(cgi_fd[0]);
		}else //parent sock
		{
			close(server_fd[1]);
			close(cgi_fd[0]);
			if(!strncasecmp(method,"POST",4))
			{
				int index=0;
				char buf[1024];
				memset(buf,'\0',sizeof(buf));
				char ch;
				int len=atoi(content_length);
			//	printf("len=%d\n",len);
				while(len>0)
				{
					read(sock,&ch,1);
					//cout<<"ch="<<ch<<endl;
					buf[index++]=ch;
					len--;
				}
				//printf("readsockbuf%s\n",buf);
				write(cgi_fd[1],buf,strlen(buf));
			}
			char ch;
			while(read(server_fd[0],&ch,1)>0)
			{
				send(sock,&ch,1,0);
			}

			close(server_fd[0]);
			close(cgi_fd[1]);
			int status;
			waitpid(id,&status,0);
		}
	}
	void readevent(int sock,short ievent,void*arg)
	{
		int cgi=0;
		char buf_line[_BUF_SIZE_];
		char url[_URL_SIZE_];
		char method[_METHOD_SIZE_];
		char *query_string;
		char path[2*_BUF_SIZE_];
		memset(buf_line,'\0',sizeof(buf_line));
		memset(url,'\0',sizeof(url));
		memset(method,'\0',sizeof(method));
		memset(path,'\0',sizeof(path));
		if(get_line(sock,buf_line,_BUF_SIZE_)<0)
		{
			return ;//没有数据
		}
		int i=0;
		int j=0;
		int len=strlen(buf_line);
		printf("line=%s\n",buf_line);
		while(buf_line[i]!=' ')
		{
			method[j++]=buf_line[i++];
		}
		//buf_line[i]='\0';
		method[j]='\0';
		i++;
		j=0;
		while(i<len&&buf_line[i]!=' ')
		{
			if(buf_line[i]=='?')
			{
				cgi=1;
				query_string=&buf_line[++i];
				break;
			}
			url[j++]=buf_line[i++];
		}
		url[j]='\0';
	
		if(!strncasecmp(method,"POST",4))
		{
			cgi=1;
			query_string='\0';
		}
		struct stat st;
		//int len=strlen(url);
		strcpy(path,url);
		sprintf(path,"htdocs%s",url);

		if(path[strlen(path)-1]=='/')
		{
			strcat(path,"/html/index.html");
		}
		if(stat(path,&st)<0)
		{
			cout<<"stat is null"<<endl;
			//ret errno
			//ret_errno()
		}
		else
		{
			if(S_ISDIR(st.st_mode))
			{
				memset(path,'\0',sizeof(path));
				strcpy(path,"./htdocs");
				strcat(path,"/html/index.html");
			}else if(st.st_mode&S_IXOTH||st.st_mode&S_IXUSR||st.st_mode&S_IXGRP)
			{
				cgi=1;
			}
		}
		if(cgi==0)
		{
			cout<<"method="<<method<<endl;
			cout<<"path="<<path<<endl;
			clear_head(sock);
			ret_html(sock,method,path,st.st_size);
	//		struct event *del_event=(struct event*)arg;
	//		event_del(del_event);
			close(sock);
	//		delete(del_event);
			cout<<"client close"<<endl;
		}else
		{
			ret_cgi(sock,method,path,query_string);
	//		struct event *del_event=(struct event*)arg;
	//		event_del(del_event);
			close(sock);
	//		delete(del_event);
			cout<<"client close"<<endl;
		}
//		int size;
//		char buf[1024];
//		memset(buf,'\0',sizeof(buf));
//		size=read(sock,buf,sizeof(buf)-1);
//		if(size<=0)
//		{
//		}
//		buf[size]='\0';
//		cout<<"client:::"<<buf;
//		struct event *write_event=new event;
//		event_set(write_event,sock,EV_WRITE|EV_PERSIST,writeevent,(void*)write_event);
//		event_base_set(base,write_event);
//		event_add(write_event,NULL);

	}
	void accept_handle(int sock,short ievent,void *arg)
{

		cout<<"handle"<<endl;
		struct sockaddr_in client;
		socklen_t client_len =sizeof(client);

		int newsock=accept(sock,(struct sockaddr*)&client,&client_len);
		if(newsock<0)
		{
			cout<<strerror(errno)<<endl;
		}
		cout<<"newsock="<<newsock<<endl;
		cout<<"a listen[ip]"<<inet_ntoa(client.sin_addr)<<"[port]"<<client.sin_port<<endl;

		struct event *read_event=new event;

		event_set(read_event,newsock,EV_READ,readevent,(void*)read_event);
		event_base_set(base,read_event);
		event_add(read_event,NULL);
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		cout<<"["<<argv[0]<<"]"<<"[ip][port]"<<endl;
	}
	int port = atoi(argv[2]);
	char *ip=argv[1];
	int sock=Listen(ip,port);
	base=event_base_new();
	if(base)
	{
		cout<<"event_init success"<<endl;
	}
	if(!nonblock(sock))
	{
		cout<<"event_init success"<<endl;
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
		exit(1);
	}

		cout<<"event_init success"<<endl;
	int opt=1;
		if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
		exit(1);
	}
	struct event ev;
	event_set(&ev,sock,EV_READ|EV_PERSIST,accept_handle,(void*)&ev);
	event_base_set(base,&ev);
	event_add(&ev,NULL);
	cout<<"event_init success"<<endl;
//	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
	event_base_dispatch(base);
	return 0;
}
