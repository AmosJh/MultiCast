#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const int MCAST_PORT = 8888; 
const char* MCAST_ADDR = "224.0.0.88";  /*一个局部连接多播地址，路由器不进行转发*/ 
const int MCAST_INTERVAL = 5;           /*发送间隔时间*/ 
const int BUFF_SIZE = 256;              /*接收缓冲区大小*/ 

int main(int argc, char const *argv[])
{
    int fd; /*套接字文件描述符*/ 
    struct sockaddr_in local_addr; /*本地地址*/ 
    int err = -1; 
      
    fd = socket(AF_INET, SOCK_DGRAM, 0); /*建立套接字*/ 
    if (fd == -1) 
    { 
        perror("socket()"); 
        return -1; 
    }

    memset(&local_addr, 0, sizeof(local_addr)); 
    local_addr.sin_family = AF_INET; 
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    local_addr.sin_port = htons(MCAST_PORT);

    err = bind(fd ,(struct sockaddr*)&local_addr, sizeof(local_addr)) ; 
    if(err < 0) 
    { 
        perror("bind()"); 
        return -2; 
    } 
      
    /*设置回环许可*/ 
    int loop = 1; 
    err = setsockopt(fd ,IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)); 
    if(err < 0) 
    { 
        perror("setsockopt():IP_MULTICAST_LOOP"); 
        return -3; 
    }

    /*加入广播组*/
    struct ip_mreq mreq;  
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);  /*广播地址*/ 
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);      /*网络接口为默认*/ 
    
    /*将本机加入广播组*/ 
    err = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)); 
    if (err < 0) 
    { 
        perror("setsockopt():IP_ADD_MEMBERSHIP"); 
        return -4; 
    } 
      
    int times = 0; 
    socklen_t addr_len = 0; 
    char buff[BUFF_SIZE]; 
    int n = 0;

    /*循环接收广播组的消息，5次后退出*/ 
    for(times = 0; times<5; times++) 
    { 
        addr_len = sizeof(local_addr); 
        memset(buff, 0, BUFF_SIZE); /*清空接收缓冲区*/ 
        /*接收数据*/ 
        n = recvfrom(fd , buff, BUFF_SIZE, 0,(struct sockaddr*)&local_addr, &addr_len); 
        if( n== -1) 
        { 
            perror("recvfrom()"); 
        } 
        /*打印信息*/ 
        printf("Recv %dst message from server:%s\n", times, buff); 
        sleep(MCAST_INTERVAL); 
    } 
      
    /*退出广播组*/ 
    err = setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof 
    (mreq)); 
          
    close(fd);

    return 0;
}


