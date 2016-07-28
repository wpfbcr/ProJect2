#include"httpd.h"


void* http_server(void *newsock)
{
	int opt=1;
	int sock=(int)newsock;
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}
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
	if(!strcmp(path,".htdocs/html/udp_client"))
	{
			ret_html(sock,mothod,path,st.st_size);
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



int main(int argc,char *argv[])
{
	if(argc!=3){
		usage(argv[0]);
	}

	int port=atoi(argv[2]);
	char *ip=argv[1];
	int opt=1;
	int listen_sock=startup(ip,port);
	
	if(setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}
	
	pool_init(30);
	int epfd=epoll_create(128);
	
	if(epfd<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}

	struct epoll_event ev;
	ev.data.fd=listen_sock;
	ev.events=EPOLLIN;//暂时设置为阻塞进行测试。

	if(epoll_ctl(epfd,EPOLL_CTL_ADD,listen_sock,&ev)<0)
	{
		printf_log(__FUNCTION__,__LINE__,strerror(errno));
	}

	struct epoll_event epfds[_MAX_EPFD_];

	int fd=-1;
	int i;
	for(i=0;i<=_MAX_EPFD_;i++)
	{
		epfds[i].data.fd=fd;
	}

	int timeout=100000000;
	int fdlen=0;

	while(1)
	{
		switch(fdlen=epoll_wait(epfd,epfds,_MAX_EPFD_,timeout))
		{

			case -1:
			{
				printf_log(__FUNCTION__,__LINE__,strerror(errno));
			}
			case 0:
			{
				//donoting;
			}
			default:
			{
				struct sockaddr_in client;
				int client_len=sizeof(client);
				int i=0;
				for(i=0;i<fdlen;i++)
				{
					int retfd=epfds[i].data.fd;
					if(retfd==listen_sock&&(epfds[i].events&EPOLLIN))
					{
						int recvfd=accept(retfd,(struct sockaddr*)&client,&client_len);
						if(recv<0)
						{
							continue;
						}
						else
						{
							//线程池获取线程，调用某一个线程进行使用。
							pthread_t id;
							add_work(http_server,(void*)recvfd);
						}
					}
				}
			}
		}
	}

pool_destroy();
return 0;
}	

