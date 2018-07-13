#include<iostream>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include<string>

//#include stdlib.h
//#include stdio.h
//#include string.h
#include <unistd.h>
#include <fcntl.h>
#include<time.h>
//define O_WRONLY and O_RDONLY

using namespace std;

#define GPIO_EXPORT           "/sys/class/gpio/export"
#define GPIO_Number_1      "1"
#define GPIO_Number_2      "2"
#define GPIO_Number_3      "3"
#define GPIO_Number_4      "4"

#define GPIO_DIR_1          "/sys/class/gpio/gpio1/direction"
#define GPIO_DIR_2          "/sys/class/gpio/gpio2/direction"
#define GPIO_DIR_3          "/sys/class/gpio/gpio3/direction"
#define GPIO_DIR_4          "/sys/class/gpio/gpio4/direction"

#define GPIO_VALUE_1          "/sys/class/gpio/gpio1/value"
#define GPIO_VALUE_2          "/sys/class/gpio/gpio2/value"
#define GPIO_VALUE_3          "/sys/class/gpio/gpio3/value"
#define GPIO_VALUE_4          "/sys/class/gpio/gpio4/value"

#define GPIO_DIR_OUT      "out"
#define GPIO_DIR_IN      "in"

#define GPIO_H      "1"
#define GPIO_L      "0"




int GPIO_export(const char* export_name,const char* gpio_name)
{
         int fd = open(export_name, O_WRONLY);

         if(fd == -1)

         {

                   std::cout<<("ERR:export GPIO error.\n");

                   return -1;

         }

         int re= write(fd, gpio_name ,sizeof(gpio_name));

         close(fd);
         return re;
}

int GPIO_set_direction(const char* gpio_dir_name,const char* gpio_value_name, const char* dir)
{

         int fd = open(gpio_dir_name, O_WRONLY);

         if(fd == -1)

         {

                   std::cout<<("ERR: set pin direction open error.\n");

                   return -1;

         }
        close(fd);
        int fd_for_value=open(gpio_value_name, O_RDWR);
        return fd_for_value;
}

int GPIO_Set_level(int fd_for_value,const char* level)
{

          int re=write(fd_for_value, level, sizeof(level));
          return re;
}

int GPIO_read_level(int fd_for_value,char* level)
{

          int level_state=read(fd_for_value, level, sizeof(level));
          return level_state;

}



int main()

{
    int fd_value_1=0;
    int fd_value_2=0;
    int fd_value_3=0;
    int fd_value_4=0;



         //打开端口/sys/class/gpio# echo 117 > export

         for(int i=0;i<4;i++)
         {
            GPIO_export(GPIO_EXPORT,GPIO_Number_1);
            GPIO_export(GPIO_EXPORT,GPIO_Number_2);
            GPIO_export(GPIO_EXPORT,GPIO_Number_3);
            GPIO_export(GPIO_EXPORT,GPIO_Number_4);
         }




         //设置端口方向/sys/class/gpio/gpio117# echo out > direction

         for(int i=0;i<4;i++)
         {
            fd_value_1=GPIO_set_direction(GPIO_DIR_1,GPIO_VALUE_1,GPIO_DIR_IN);
            fd_value_2=GPIO_set_direction(GPIO_DIR_2,GPIO_VALUE_2,GPIO_DIR_OUT);
            fd_value_3=GPIO_set_direction(GPIO_DIR_3,GPIO_VALUE_3,GPIO_DIR_OUT);
            fd_value_4=GPIO_set_direction(GPIO_DIR_4,GPIO_VALUE_4,GPIO_DIR_OUT);
         }



         //输出复位信号: 拉高>100ns

        struct timespec delay_time;
        delay_time.tv_sec=0;
        delay_time.tv_nsec=500000;
         while(1)

         {
        //GPIO_Set_level(GPIO_VALUE_1,GPIO_H)
            GPIO_Set_level(fd_value_2,GPIO_H);
            GPIO_Set_level(fd_value_3,GPIO_H);
            GPIO_Set_level(fd_value_4,GPIO_L);
            //usleep(500);
            if(nanosleep(& delay_time,NULL)!=0)
                std::cout<<"123"<<std::endl;
            GPIO_Set_level(fd_value_2,GPIO_L);
            GPIO_Set_level(fd_value_3,GPIO_L);
            GPIO_Set_level(fd_value_4,GPIO_H);
            //usleep(500);
            if(nanosleep(& delay_time,NULL)!=0)
                std::cout<<"123"<<std::endl;
         }





         return 0;



}
