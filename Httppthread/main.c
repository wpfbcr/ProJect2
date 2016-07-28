#include"httpd.h"


void* http_server(void *newsock)
{
	int sock=(int)newsock;
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
	strcpy(path,url);
	sprintf(path,"htdocs%s",url);
	if(path[strlen(path)-1]=='/')
	{
		strcat(path,"/html/index.html");
	}
	printf("%s\n",path);

	if(stat(path,&st)<0)
	{
		printf("stat is null\n");
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
	
	struct sockaddr_in client;
	int client_len=sizeof(client);
	
	int newsock=-1;
	while(1)
	{
		newsock=accept(listen_sock,(struct sockaddr*)&client,&client_len);
		if(newsock<0)
		{
			printf_log(__FUNCTION__,__LINE__,strerror(errno));
		}
		printf("a client listen%d[ip]::%s,[port]::%d\n",newsock,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		pthread_t id=-1;
		pthread_create(&id,NULL,http_server,(void*)newsock);
		pthread_detach(id);

	}
	
}

