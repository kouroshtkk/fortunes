#include <dirent.h>

#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#define PORT "4242"

char *read_from_file(FILE *file)
{
  int max = 4096;
  char *ptr=malloc(max);
  ssize_t n,rc;
  char c;
  for(n = 1; n< max;++n)
    {
      rc = fread(&c,1,1,file);
      if(rc==1)
	{
	  ptr[n-1]=c;
	  if(c == '%'&&n!=1)
	    {
	      break;
	    }
	}
      else if(rc == 0)
	{
	  if(n==1)
	    {
	      return NULL;
	    }
	  else
	    break;
	}
      else
	{
	  return NULL;
	}
    }
  ptr[n] = '\0';
  return ptr;


  
}
char *get_rnd_file_name(DIR* dir)
{
  srand(time(NULL));
  int filecount=0;
  struct dirent *entry;
  while((entry = readdir(dir)))
    {
      if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)       
	continue;
      
      filecount++;
    }
  if(filecount==0)
    {
      closedir(dir);
      return NULL;
    }
  int index = 0;
  char *filename=NULL;
  int rand_num = rand() % filecount;
  rewinddir(dir);
  while((entry = readdir(dir)))
    {
      if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)       
	continue;
      if(rand_num==index)
	{
	  filename=strdup(entry->d_name);
	  break;
	}
      index++;
    }
  closedir(dir);
  return filename;
}
void handle_client(int fd)
{
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  printf("waiting to recvfrom\n");
  uint8_t buf[100];
  int numbytes = recvfrom(fd,buf,sizeof(buf)-1,0,(struct sockaddr *)&client_addr,&len);
  if(numbytes == -1)
    {
      perror("recv");
      return;
    }
  if(numbytes>=0)
    {
      DIR *dir = opendir("./fortunes");
      if(dir == NULL)
	{
	  perror("open");
	  return;
	}
      char *filename = get_rnd_file_name(dir);
      if(filename==NULL)
	{
	  printf("file error");
	  return;
	}
      char path[1024];
      snprintf(path,sizeof(path),"./fortunes/%s",filename);
      FILE *file = fopen(path,"r");
      if(file==NULL)
	{
	  perror("fopen");
	  return;
	}
      char *msg = read_from_file(file);
      uint32_t len_msg = strlen(msg);
      int numbytes_send = sendto(fd,msg,len_msg,0,(struct sockaddr *)&client_addr,len);
      if(numbytes_send == -1)
	{
	  perror("send msg");
	  return;
	}
      free(msg);
    }
  
}
int server_init()
{
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  if(getaddrinfo(NULL,PORT,&hints,&servinfo)!=0)
    {
      printf("getaddrinfo error\n");
      return -1;
    }
  for(p = servinfo;p!=NULL;p=p->ai_next)
    {
      if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
	{
	  perror("socket");
	  continue;
	}
      if(bind(sockfd,p->ai_addr,p->ai_addrlen) == -1)
	{
	  close(sockfd);
	  perror("bind");
	  continue;
	}
      break;
    }
  if(p==NULL)
    {
      printf("failed to bind\n");
      return -1;
    }
  freeaddrinfo(servinfo);
  return sockfd;
}
int main()
{
  int sockfd = server_init();
  if(sockfd < 0)
    {
      printf("initialization failed\n");
      return -1;
    }
  while(1)
    {
      handle_client(sockfd);
    }
  
}
