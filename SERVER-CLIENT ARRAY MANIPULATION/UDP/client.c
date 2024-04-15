#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
int main()
{
 int sock_c;
 socklen_t s_len;
 sock_c=socket(AF_INET,SOCK_DGRAM,0);
 if(sock_c<0)
 {
 printf("Socket creation failed\n");
 exit(0);
 }
 printf("Socket created\n");
 struct sockaddr_in server;
 server.sin_family=AF_INET;
 server.sin_port=htons(8080);
 server.sin_addr.s_addr=inet_addr("127.0.0.1");
 s_len=sizeof(server);
int arr[20],i;
 printf("Enter number of elements:\t");
 scanf("%d",&arr[0]);
 printf("Enter the numbers\n");
 for(i=0;i<arr[0];i++)
 {
 scanf("%d",&arr[i+1]);
 } 
 ssize_t sen=sendto(sock_c,&arr,(arr[0]+1)*sizeof(int),0,(struct sockaddr*)&server,s_len);
 if(sen<0)
 {
 printf("Send failed\n");
 exit(0);
 } 
 printf("Send successfully\n");
 int arr2[10];
 ssize_t rec=recvfrom(sock_c,arr2,sizeof(arr2),0, (struct sockaddr *)&server, &s_len);
 if(rec<0)
 {
 printf("receive Failed\n");
 exit(0);
 }
 printf("Array recieved\n");
 int min,max,sum,avg;
 max=arr2[0];
 min=arr2[1];
 sum=arr2[2];
 avg=arr2[3];
 printf("maximum = %d\n",max);
 printf("minimum = %d\n",min);
 printf("sum = %d\n",sum);
 printf("average = %d\n",avg);
 close(sock_c);
}