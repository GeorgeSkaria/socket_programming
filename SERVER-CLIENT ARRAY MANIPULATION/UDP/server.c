#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
int main()
{
 int sock_s;
 socklen_t s_len;
 int arr[20];
 sock_s=socket(AF_INET,SOCK_DGRAM,0);
 if(sock_s<0)
 {
 printf("Socket creation failed\n");
 exit(0);
 }
 struct sockaddr_in server,client;
 s_len=sizeof(server);
 server.sin_family=AF_INET;
 server.sin_port=htons(8080);
 server.sin_addr.s_addr=inet_addr("127.0.0.1");
 int server_b= bind(sock_s,(struct sockaddr*)&server,s_len);
 
 if(server_b<0)
 {
 printf("Bind Failed\n");
 exit(0);
 }
 printf("Bind Successfull\n");
 ssize_t rec= recvfrom(sock_s,arr,sizeof(arr),0,(struct sockaddr*)&server,&s_len);
 if(rec <0)
 {
 printf("Receive failed\n");
 exit(0);
 }
 printf("Array received\n");
 int len,max,min,sum,avg,i;
 len=arr[0];
 max=arr[1];
 min=arr[1];
 sum=0;
 for(i=1;i<=len;i++)
 {
 if(arr[i]>=max)
 max=arr[i];
 if(arr[i]<=min)
 min=arr[i];
 sum=sum+arr[i]; 
 }
 
 avg=sum/(len);
 int arr2[10];
 arr2[0]=max;
 arr2[1]=min;
 arr2[2]=sum;
 arr2[3]=avg;
 
 ssize_t sen=sendto(sock_s,arr2,sizeof(arr2),0,(struct sockaddr*)&server,s_len);
 if(sen< 0)
 {
 printf("Send failed\n");
 exit(0);
 }
 printf("Array Sent after manipulation\n");
 close(sock_s); 
}