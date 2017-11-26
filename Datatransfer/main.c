#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define ECHO_PORT 6666  //echo port
#define m_ClientPort 8080
#define DATA_BUF_MAX_LEN 1024
#define remote_server_ip "192.168.1.102"
#define message "hello I'm zhujinguo\nA handsome boy!\n"
int fd_server=-1;
int s_port_fd_client=-1;
int client_connect=0;
int new_fd=-1;
unsigned char RevBuff[1024];
unsigned char RevBuf[1024];
unsigned char SendBuff[1024];
int numbytes,numbytesclient;
fd_set rfds;  //文件描述符集合。用于select函数，可以达到非阻塞的效果
struct timeval tv;
int retval;
fd_set rfds1;  //文件描述符集合。用于select函数，可以达到非阻塞的效果
struct timeval tv1;
int retval1;
struct sockaddr_in remote_addr;  /* remote address information */
struct sockaddr_in src;  //server ip address
struct sockaddr_in client_addr;
socklen_t sin_size;

void server_init()
{
    fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if( fd_server < 0 )
	{
        	printf(" server socket failed\n");
        	//usleep(5*1000);
        	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	}
	else
    {
        printf("success to create server socket\n");
    }
	//struct sockaddr_in src;
	bzero(&src, sizeof(src));
	src.sin_family = AF_INET;
	src.sin_addr.s_addr = INADDR_ANY;//  INADDR_ANY就是inet_addr("0.0.0.0") 全部网口监听
	src.sin_port = htons(ECHO_PORT);  //监听端口
	printf("server ECHO_PORT=%d\n",ECHO_PORT);

	if(bind( fd_server, (struct sockaddr *)&src, sizeof(src)) < 0 )
	{
		printf("Can not bind!\n");
		sleep(1);
        if(fd_server!=-1)close(fd_server);
		fd_server = -1;
		//return ;
	}
	else
    {
        printf("bind socket success\n");
        if (listen(fd_server, 10) < 0 )
        {
            printf("server can not listen InetSocket");
            close(fd_server);
            fd_server = -1;
            //return ;
        }
        else
        {
            printf("listening connect waitting......\n");
        }
    }




}

void* thread_server(void *arg)
{
   printf("thread_server pthread_create success\n");
    server_init();
    while(1)
   {

       int tmp=0;
     //select相关函数的设置

		if(fd_server!=-1)
        {   FD_ZERO(&rfds);  //将文件描述符集清空
            FD_SET(fd_server, &rfds);  //在文件描述符集合中增加服务器文件描述符。其实就是socket的句柄
            tmp = fd_server;
            if(new_fd>0)
            {
                FD_SET(new_fd, &rfds);	  //添加通信句柄
                tmp=tmp > new_fd ? tmp : new_fd;  //较大的句柄
            }
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            retval = select(tmp+1, &rfds, NULL, NULL, &tv);
            if (retval == -1)  //错误
            {
                printf("select() error ");
            }
            else if (retval ==0)    //无监听状态
            {

            }
            else if (retval) //大于0说明 某些文件可读写或出错
            {
                if(FD_ISSET(fd_server, &rfds)) 		 /*判断是否为服务器套接字，是则表示为客户请求连接。*/
                {
                    printf("will to accept.....\n");
                    if ((new_fd = accept(fd_server, (struct sockaddr *)&client_addr, &sin_size)) == -1)
					//接受请求要求，并返回一个socket，与客户端连接 ，之前的继续监听
                        {
                            printf("server accept error\n");
                            //return;
                        }
                    else
                        {
                            printf("server accept a client' connect!\n");
                        }
				FD_SET(new_fd, &rfds);	  //添加通信句柄
				tmp=tmp > new_fd ? tmp : new_fd;
                }
                if(FD_ISSET(new_fd, &rfds))  //通信socket可读
                {
                    memset(RevBuff,0,strlen(RevBuff));
                   if ((numbytes=read(new_fd, &RevBuff[0], (DATA_BUF_MAX_LEN-10))) == -1)   //读取
                    {
                        printf("new_fd read error!\n");
                    }
                    else if( numbytes==0)  ///*客户数据请求完毕，关闭套接字，从集合中清除相应描述符 */
                    {

                        FD_CLR(new_fd,&rfds);
                        if(new_fd!=-1)
                        close(new_fd);   //it's important here
                        new_fd=-1;
                    }
                    else   //读取成功
                    {
                        sleep(1);
                        if(new_fd>0)
                        {
                            if (send(new_fd, &RevBuff[0],numbytes, 0) == -1)	 //向下属客户端发送
                            {
                                printf(" new_fd send failed!");
                            }
                            else
                            {
                                printf("new_fd send: %s\n",&RevBuff[0]);
                            }
                        }

                    }
                }

            }
        }
        else
        {
            server_init();
        }


    }

}

void client_init()
{

	//远程服务器信息
    if ((s_port_fd_client = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("s_port_fd_client socket Error!");
		//exit(1);
	}
	 else printf("success to create client socket\n");

    bzero(&remote_addr,sizeof(remote_addr)); /* zero the rest of the struct */
	remote_addr.sin_family = AF_INET; /* host byte order */
	remote_addr.sin_port=htons(m_ClientPort);

	inet_aton(remote_server_ip,&remote_addr.sin_addr);
}

void connect_task()
{
    if(client_connect==0)
      {
        if(connect(s_port_fd_client,(struct sockaddr *)&remote_addr, sizeof(struct sockaddr))== -1)   //连接失败
        {
            printf("client connect remote server failed\n");
            sleep(2);
            client_connect=0;
            close(s_port_fd_client);  //it's very important here
            client_init();
        }
         else
         {
            client_connect=1;
            printf("s_port_fd_client connect success\n");
            stpcpy(SendBuff,message);
            if (send(s_port_fd_client, &SendBuff[0],strlen(SendBuff), 0) == -1)		 //向远程服务器发送
            {
                printf("first write out error\n");
            }
            else
            {
                printf("first client out %s\n",&SendBuff[0]);
               // client_connect=1;
            }
         }
      }
}
void* thread_client(void *arg)
{
    printf("thread_client pthread_create success\n");
    client_init();
    connect_task();
    while(1)
   {
       if(client_connect==0)
       {
           connect_task();
       }
     else
     {
              if(s_port_fd_client>0)
       {
        FD_ZERO(&rfds1);  //将文件描述符集清空
		FD_SET(s_port_fd_client, &rfds1);  //在文件描述符集合中增加服务器文件描述符。其实就是socket的句柄
        tv1.tv_sec = 5;
		tv1.tv_usec = 0;
		retval1 = select(s_port_fd_client+1, &rfds1, NULL, NULL, &tv1);
        if (retval1 == -1)  //错误
		{
			printf("select1() error ");
		}
		else if (retval1 ==0)    //无监听状态
		{

		}
		else if (retval1)
        {
                    if(FD_ISSET(s_port_fd_client, &rfds1)) //与远程服务器连接的客户端的socket可读
			{
			     memset(RevBuf,0,strlen(RevBuf));
				if ((numbytesclient=read(s_port_fd_client, &RevBuf[0], (DATA_BUF_MAX_LEN-10))) == -1)
				{
					printf("DT s_port_fd_client Disconnect\n");
					printf("s_port_fd_client=%d\n",s_port_fd_client);
				}
				else if( numbytesclient==0)  //套接字关闭，从集合中清楚相应描述符
				{
				    if(client_connect!=0)
                    {
                       printf("DT Disconnect\n");
					   FD_CLR(s_port_fd_client,&rfds1);
					   if(s_port_fd_client!=-1)close(s_port_fd_client);
					   s_port_fd_client=-1;
					   client_connect=0;
                    }

				}
				else
				{
                    if((s_port_fd_client>0)&&(client_connect!=0)) //客户端已创建成功
					{   sleep(1);
						if (send(s_port_fd_client, &RevBuf[0],numbytesclient, 0) == -1)		 //向远程服务器发送
						{
							printf("client write error !\n");
						}
						else
                        {
                            printf("client send: %s\n",&RevBuf[0]);
                        }
					}

				}
			}
        }

       }
       else
       {
           client_init();
       }
     }


   }
}

int main ()
{
    pthread_t tid;
    int status=0;
    status=pthread_create(&tid,NULL,thread_server,NULL);
    if(status!=0)
        {
            printf("thread_server pthread_create error\n");
        }

    status=pthread_create(&tid,NULL,thread_client,NULL);
    if(status!=0)
        {
            printf("thread_client pthread_create error\n");
        }
   while(1)
   {

   }
}
