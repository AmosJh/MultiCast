#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int MCAST_PORT = 8888; 
const int MCAST_INTERVAL = 5; /*发送间隔时间*/
const char* MCAST_ADDR = "224.0.0.88"; /*一个局部连接多播地址，路由器不进行转发*/ 
const char* MCAST_DATA = "BROADCAST TEST DATA";  /*多播发送的数据*/

int main(int argc, char const *argv[])
{
    struct sockaddr_in mcast_addr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd == -1) 
    { 
        perror("socket()"); 
        return -1; 
    }

    memset(&mcast_addr, 0, sizeof(mcast_addr)); /*初始化IP多播地址为0*/ 
    mcast_addr.sin_family = AF_INET;            /*设置协议族类行为AF*/ 
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR); /*设置多播IP地址*/ 
    mcast_addr.sin_port = htons(MCAST_PORT);            /*设置多播端口*/

    while(1) { 
        int n = sendto( fd,         /*套接字描述符*/ 
                        MCAST_DATA, /*数据*/ 
                        sizeof(MCAST_DATA), /*长度*/ 
                        0, 
                        (struct sockaddr*)&mcast_addr, 
                        sizeof(mcast_addr)) ; 
        if( n < 0) 
        { 
            perror("sendto()"); 
            return -2; 
        } 
          
        sleep(MCAST_INTERVAL); /*等待一段时间*/ 
    }

    return 0;
}
