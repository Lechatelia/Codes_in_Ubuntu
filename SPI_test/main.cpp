#include<iostream>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>

//#include stdlib.h
//#include stdio.h
//#include string.h
//#include unistd.h

#include <fcntl.h>
//define O_WRONLY and O_RDONLY

using namespace std;

#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"

#define GPIO_Number      "117"

#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio117/direction"

#define GPIO_DIR_OUT      "OUT"
#define GPIO_DIR_IN      "OUT"
#define GPIO_H      "1"
#define GPIO_L      "0"

#define SYSFS_GPIO_VAL          "/sys/class/gpio/gpio117/value"




int main()

{

    int fd;



         //打开端口/sys/class/gpio# echo 117 > export

         fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);

         if(fd == -1)

         {

                   cout<<("ERR: Radio hard reset pin open error.\n");

                   return -1;

         }

         write(fd, GPIO_Number ,sizeof(GPIO_Number));

         close(fd);



         //设置端口方向/sys/class/gpio/gpio117# echo out > direction

         fd = open(SYSFS_GPIO_RST_DIR, O_WRONLY);

         if(fd == -1)

         {

                   cout<<("ERR: Radio hard reset pin direction open error.\n");

                   return -1;

         }

         write(fd, GPIO_DIR_OUT, sizeof(GPIO_DIR_OUT));

         close(fd);



         //输出复位信号: 拉高>100ns

         fd = open(SYSFS_GPIO_VAL, O_RDWR);

         if(fd == -1)

         {

                   cout<<("ERR: Radio hard reset pin value open error.\n");

                   return -1;

         }

         while(1)

         {

                   write(fd, GPIO_H, sizeof(GPIO_H));

                   usleep(1000000);

                   write(fd, GPIO_L, sizeof(GPIO_L));

                   usleep(1000000);

         }

         close(fd);



         cout<<("INFO: Radio hard reset pin value open error.\n");

         return 0;



}

/*int main()
{
    string str1;
    cout<<"input"<<endl;
    cin>>str1;
    cout<<str1<<endl;
}*/
