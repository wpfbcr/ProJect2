#include<stdlib.h>
#include<iostream>
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
#define _SIZE_ 2048
#define _SIZE_MOTHOD_ 20
#define _SIZE_URL_ 1024
struct event_base *base;
using namespace std;
void printf_log(const char *function,int line,char *erro)
{
	printf("%s**%s**%s\n",function,line,erro);
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
int get_line(int fd,char *line,int size)
{
	char ch='a';
	int index=0;
	int _s=0;
	while(ch!='\n'&&index<size-1)
	{
		_s=recv(fd,&ch,1,0);
		if(_s>0)
		{
			if(ch=='\r')
			{
				if(recv(fd,&ch,1,MSG_PEEK))
				{
					if(_s>0&&ch=='\n')
					recv(fd,&ch,1,0);
				}
			}
			line[index++]=ch;
		}
		else{
			line[index++]='\n';
			break;
		}
	}
		line[index]='\0';
		return index;
}


void clear_head(int sock)
{
	char buf[2048];
	int size=-1;
	memset(buf,'\0',sizeof(buf));
	int count=0;
	do{
		 size=-1;
		 memset(buf,'\0',sizeof(buf));
		 size=get_line(sock,buf,2048);

	}while(buf[0]!='\n'&&size>0);
}


void ret_cgi(int sock,char *method,char*path,char*query_string)
{


	printf("method=%s url=%s\n",method,path);
	char content_length[_SIZE_];
	char content_env[_SIZE_];
	char method_env[_SIZE_];
	char query_string_env[_SIZE_];
	char buf[_SIZE_];

	memset(content_env,'\0',_SIZE_);
  	memset(content_length,'\0',_SIZE_);
	memset(method_env,'\0',_SIZE_);
	memset(query_string_env,'\0',_SIZE_);
	memset(buf,'\0',_SIZE_);

	if(!strncasecmp(method,"POST",4))
	{
	while(get_line(sock,buf,_SIZE_)>0)
	{
		if(!strncasecmp(buf,"content-length",14))
		{
			break;
		}
		memset(buf,'\0',_SIZE_);
	}
	
		clear_head(sock);
		strcpy(content_length,buf+15);
		memset(buf,'\0',_SIZE_);
	}else
	{
		char *p=query_string+strlen(query_string);
		while(*p!=' ')
		{
			--p;
		}
		*p='\0';
	}


	sprintf(method_env,"REQUEST_METHOD=%s",method);
	putenv(method_env);
	if(!strcasecmp(method,"get"))
	{
		sprintf(query_string_env,"QUERY_STRING=%s",query_string);
		putenv(query_string_env);
	}
	else{
		sprintf(content_env,"CONTENT_LENGTH=%s",content_length);
		putenv(content_env);
	}
	
	int cgi_fd[2];
	if(pipe(cgi_fd)<0){
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}
	int server_fd[2];
	if(pipe(server_fd)<0){
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}




	pid_t id=0;
	id=fork();
	if(id<0){
	printf_log(__FUNCTION__,__LINE__,strerror(errno));
	exit(3);
		}else if(id==0){//child
		//priutf("137query_string%s\n",query_string);
		printf("path=%s\n",path);
		close(cgi_fd[1]);
		close(server_fd[0]);
		dup2(server_fd[1],1);
		dup2(cgi_fd[0],0);
		execl(path,"vac_cgi",NULL);
	  
		close(cgi_fd[0]);
		close(server_fd[1]);
  	}
  else{
		char buf[_SIZE_];
		memset(buf,'\0',sizeof(buf));
		close(cgi_fd[0]);
		close(server_fd[1]);
		
		if(!strncmp(method,"POST",4))
		{
		char buf[_SIZE_];
		char ch;
		memset(buf,'\0',sizeof(buf));
		int lencgi=atoi(content_length);
		int index=0;
		while(lencgi>0)
		{
			read(sock,&ch,1);
			lencgi--;
			buf[index++]=ch;
		}
			write(cgi_fd[1],buf,strlen(buf));
		}
		memset(buf,'\0',sizeof(buf));
		char ch;
		while(read(server_fd[0],&ch,1)>0)
		{
			send(sock,&ch,1,0);
		}
		close(cgi_fd[1]);
		close(server_fd[0]);
		int status=0;
		waitpid(id,&status,0);

	}

}

void ret_html(int sock,char *mothod,const char *path,int size)
{
	char *buf="HTTP/1.0 200 OK\r\n\r\n";
	ssize_t _size=send(sock,buf,strlen(buf),0);
	int fd=open(path,O_RDONLY);
	_size=sendfile(sock,fd,NULL,size);
	if(_size<0)
	{
		return;
	}
//	char httpbuf[1024];
//	memset(httpbuf,'\0',sizeof(httpbuf));
//	_size=read(sock,httpbuf,sizeof(httpbuf)-1);
//	if(_size>0)
//	{
//		printf("httpbuf_size%d\n",_size);
//		printf("%s\n",httpbuf);
//	}

}




void http_server(int sock,short event,void * arg)
{
	//printf("http_server%d\n",sock);
	int i=0;
	int j=0;
	
	int cgi=0;

	char line[_SIZE_];
	char mothod[_SIZE_MOTHOD_];
	char url[_SIZE_URL_];
	char *query_string;
	
	char path[_SIZE_*2];
	memset(line,'\0',sizeof(line));
	memset(mothod,'\0',sizeof(mothod));
	memset(url,'\0',sizeof(url));
	memset(path,'\0',sizeof(path));

	if(get_line(sock,line,_SIZE_)<=0)
	{
		
		return ;//没数据
	}
	while(line[i]!=' ')
	{
		mothod[j++]=line[i++];
	}
	mothod[j]='\0';
	i++;
	if(!strncasecmp(mothod,"post",4))
	{
		cgi=1;
		query_string='\0';
	}
	j=0;
	while(line[i]!=' ')
	{
		if(line[i]=='?')
		{
			query_string=&line[++i];
			cgi=1;
		
			break;
		}
		url[j++]=line[i++];
	}
	url[j]='\0';
	struct stat st;
	int len=strlen(url);
	//printf("%d\n",len);
	strcpy(path,url);
	sprintf(path,"htdocs%s",url);
	//printf("%s\n",path);
	if(path[strlen(path)-1]=='/')
	{
		//printf("11111\n");
		strcat(path,"/html/index.html");
	}
	printf("%s\n",path);

	if(stat(path,&st)<0)
	{
		printf("stat is null\n");
			//echo_errno(sock,404);
	}else 
		
	{
		if(S_ISDIR(st.st_mode))
		{
			strcpy(path,"./htdocs");
			strcat(path,"/html/index.html");
		}
		else if(st.st_mode&S_IXOTH||st.st_mode&S_IXUSR||st.st_mode&S_IXGRP)
		{
			cgi==1;
			//printf("811111\n");
		
		}
	}
		if(cgi==0)
		{
			printf("mothod=%s,url=%s\n",mothod,url);
			clear_head(sock);
			ret_html(sock,mothod,path,st.st_size);
		
		}else{
			printf("cgimothod=%s,url=%s\n",mothod,url);
			ret_cgi(sock,mothod,path,query_string);
		}
	
		close(sock);
		printf("close %d\n",sock);

	
}




void usage(char *proc)
{
	printf("[%s][ip][port]\n",proc);
	exit(0);
}

int startup(char *ip,int port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	int opt=1;
	if(sock<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}


	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=INADDR_ANY;
	local.sin_port=htons(port);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
		exit(1);
	}

	if(listen(sock,8)<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
		exit(2);
	}

	return sock;
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

	if(!nonblock(sock))
	{
		cout<<"event_init success"<<endl;
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
		exit(1);
	}
		struct event *read_event=new event;

		event_set(read_event,newsock,EV_READ,http_server,(void*)read_event);
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
	int sock=startup(ip,port);
	base=event_base_new();
	if(base)
	{
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
