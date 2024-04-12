#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
int main()
{
 int cp,con;
 struct sockaddr_in serv_adr;
 char* msg = "Msg from client";
 ssize_t valread;
 char buffer[1024] = {0};
 socklen_t addrlen = sizeof(serv_adr);
 cp = socket(AF_INET,SOCK_STREAM,0);
 if(cp<0)
 { printf("Socket Creation Failed\n");
 exit(0);
 }
 else
 printf("Socket descriptor value = %d\n",cp);
 
 serv_adr.sin_family=AF_INET;
 serv_adr.sin_port=htons(8088);
 //serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
 
 con=connect(cp,(struct sockaddr*)&serv_adr,addrlen);
 
 if(con<0){
 printf("Connect Error\n");
 exit(0);
 }
 
 
 send(cp,msg,strlen(msg),0);
 printf("Message sent\n");
 valread=read(cp,buffer,1024-1); //sub 1 for null terminator at end;
 printf("Message read is %s",buffer);
 
 
 
 
 close(cp);
 
 return 0; 
 
 
} 