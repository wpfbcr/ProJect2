#include"httpd.h"
#include<pthread.h>
#define _CGI_ "./htdocs/cgi_bin/vac_cgi"
#define _BUF_SIZE_ 1024*10


void printf_log(const char *function,const int line,const char *file)
{
	int fd=open("./log/daily.log",O_CREAT|O_WRONLY,0644 );
	if(fd<0)
	{
		perror("open");
	}
	close(1);
	int newfd=dup2(fd,1);
	close(fd);
	printf("%s**%d**%s\n",function,line,file);
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
	 sock=-1;
	if(sock<0)
	{
		printf_log(__func__,__LINE__,__FILE__);
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

void ret_error(int errno)
{
	;
}
void ret_cgi(int sock,char *method,char*path,char*query_string)//动态
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

	sprintf(method_env,"REQUEST_METHOD=%s",method);//配置环境变量
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
		waitpid(id);

	}

}

void ret_html(int sock,char *mothod,const char *path,int size)//静态
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
