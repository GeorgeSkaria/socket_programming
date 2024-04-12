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
 int sp,b,l,new_sock;
 struct sockaddr_in address;
 char* msg = "Msg from server";
 ssize_t valread;
 char buffer[1024] = {0};
 socklen_t addrlen = sizeof(address);
 sp = socket(AF_INET,SOCK_STREAM,0);
 if(sp<0)
 { printf("Socket Creation Failed\n");
 exit(0);
 }
 else
 printf("Socket descriptor value = %d\n",sp);
 
 address.sin_family=AF_INET;
 address.sin_port=htons(8088);
 address.sin_addr.s_addr = inet_addr("127.0.0.1");
 b=bind(sp,(struct sockaddr*)&address,sizeof(address));
 if(b<0)
 {
 printf("Bind Unsuccessful\n");
 exit(0);
 }
 printf("Bind Successful\n");
 
 l=listen(sp,5);
 if(l<0)
 {
 printf("Listen failed\n");
 exit(0);
 }
 new_sock=accept(sp,(struct sockaddr*)&address,&addrlen);
 
 if(new_sock<=0){
 printf("Accept Error\n");
 exit(0);
 }
 
 valread=read(new_sock,buffer,1024-1); //sub 1 for null terminator at end;
 printf("%s\n",buffer);
 send(new_sock,msg,strlen(msg),0);
 printf("Message sent\n");
 
 close(new_sock);
 close(sp);
 
 return 0; 
 
 
}