#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>   //struct ifreq
#include <errno.h>  /*错误号定义*/
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>   /*文件控制定义*/
#include <termios.h>   /*PPSIX 终端控制定义*/
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <unistd.h> /*UNIX 标准函数定义*/

#define DATA_BUF_MAX_LEN  4096 //数据缓冲的长度

int s_port_fd; //serial number
bool OpenSerial_Flag;//串口打开是否成功
unsigned char RevBuf1[DATA_BUF_MAX_LEN];
unsigned char RevBuf2[DATA_BUF_MAX_LEN];

bool OpenSerial(int iPort,int ibaud,int iparity)//打开串口
{
	 tcflag_t baudRate,parity=0;
	struct termios opt;
	char cSerialName[20];

	if(s_port_fd!=0)
	{
		close(s_port_fd);
		s_port_fd = 0;
	}
	//sprintf(cSerialName, "/dev/ttySP%d", iPort - 1);
	sprintf(cSerialName, "/dev/ttymxc%d", iPort - 1);
	printf("open serila name:%s \n", cSerialName);

	s_port_fd = open(cSerialName, O_RDWR | O_NOCTTY  | O_NDELAY);
	if(s_port_fd < 0)
	{
		printf("open serila Error:%s ", cSerialName);
		return 0;
	}

	tcgetattr(s_port_fd, &opt);      //获取选项

	switch(ibaud)
	{
		case 115200:
			baudRate=B115200; break;
		case 38400:
			baudRate=B38400; break;
		case 19200:
			baudRate=B19200; break;
		case 9600:
			baudRate=B9600; break;
		case 4800:
			baudRate=B4800; break;
		case 2400:
			baudRate=B2400; break;
		default:
			baudRate=B19200;
	}
	cfsetispeed(&opt, baudRate);//设置波特率
	cfsetospeed(&opt, baudRate);

	/*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
	switch(iparity)
	{
		case 0://无校验
			opt.c_cflag &= ~PARENB;
			opt.c_cflag &= ~CSTOPB;
			opt.c_cflag &= ~CSIZE;
			//opt.c_cflag |= CS8;
			break;
		case 2://偶校验
			opt.c_cflag |= PARENB;
			opt.c_cflag &= ~PARODD;
			opt.c_cflag &= ~CSTOPB;
			opt.c_cflag &= ~CSIZE;
			break;
		case 1://奇校验
			opt.c_cflag |= PARENB;
			opt.c_cflag |= PARODD;
			opt.c_cflag &= ~CSTOPB;
			opt.c_cflag &= ~CSIZE;
			break;
		default:
			opt.c_cflag &= ~PARENB;
			opt.c_cflag &= ~CSTOPB;
			opt.c_cflag &= ~CSIZE;
			opt.c_cflag |= CS8;//默认无校验
	}
	opt.c_cflag   |=   CS8;//8位
	if (tcsetattr(s_port_fd,   TCSANOW,   &opt)<0)
	{
		return   0;
	}
	tcflush(s_port_fd,TCIOFLUSH);
	//CSysLog::Info("open serila cSerialName=%s,ibaud = %d,iparity=%d,s_port_fd=%d", cSerialName,ibaud,iparity,s_port_fd);
	printf("open serila cSerialName=%s,ibaud = %d,iparity=%d,s_port_fd=%d", cSerialName,ibaud,iparity,s_port_fd);
	return 1;

}

int main()
{
    fd_set rfds;
	struct timeval tv;
	int retval;
	int numbytesclient;


	while(1)
	{
            if(OpenSerial_Flag==0)//串口打开错误
		{
			if(OpenSerial(3,115200,0))
			{
				OpenSerial_Flag = 1;
			}
			else
			{
				OpenSerial_Flag = 0;
			}
			usleep(1000);
			continue;
		}
    FD_ZERO(&rfds);
	FD_SET(s_port_fd, &rfds);  //将串口添加监听
	tv.tv_sec = 5;
    tv.tv_usec = 0;
    retval = select(s_port_fd+1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
    {
        printf("select() error");
    }
    else if (retval ==0)
    {
        printf("serial: time out");
    }
    else if(retval)
    {
        if ((numbytesclient=read(s_port_fd, &RevBuf2[0], (DATA_BUF_MAX_LEN-10))) == -1)
        {
            printf("receive error\n ");
            FD_CLR(s_port_fd,&rfds);
            if(s_port_fd!=-1)close(s_port_fd);
            s_port_fd=-1;
        }
        else if( numbytesclient==0)
        {

            printf(" s_port_fd Disconnect\n");
            FD_CLR(s_port_fd,&rfds);
            if(s_port_fd!=-1)close(s_port_fd);
            s_port_fd=-1;
        }
        else
        {
            printf("%s",RevBuf2);
            if(s_port_fd>0)
            {
                if (write(s_port_fd, &RevBuf2[0],numbytesclient) == -1)
                {
                    fprintf(stderr," s_port_fd write port failed!\n");
                }
                else
                {
                    fprintf(stderr," s_port_fd send what it received!\n");
                }
            }
        }
    }

	}


}
