#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
//meiyouqudongsuoyibukeyiyong
#define GPIO_U_IOCTL_BASE 'x'
#define GPIOC_OPS   _IOWR(GPIO_U_IOCTL_BASE,0,int)

#define GPIO_SET(no,state) 	( no | (state << 31))
#define GPIO_GET(val)		(val >> 31)


void gpio_set_value(int fd,int gpio_no,int state)
{
	unsigned long val;
	val = (!!state << 31) | gpio_no;

	if(ioctl(fd,GPIOC_OPS,&val) < 0){
		perror("ioctl");
	}
}

int  gpio_get_value(int fd,int gpio_no)
{
	unsigned long val = gpio_no;
	if(ioctl(fd,GPIOC_OPS,&val) < 0){
		perror("ioctl");
	}
	return val;
}
int main(int argc,char **argv)
{
	int gpio;
	gpio = open("/dev/gpio",O_RDWR);
	if(gpio < 0){
		perror("open");
		exit(1);
	}

	int no,state;
	unsigned long val;

	/*no = atoi(argv[2]);
	if(strcmp(argv[1],"in") == 0){
			printf("gpio %d state %d\n",no,gpio_get_value(gpio,no));
	} else {
		state = atoi(argv[3]);
		gpio_set_value(gpio,no,state);
	}*/

    no=1;
    printf("gpio %d state %d\n",no,gpio_get_value(gpio,no));
    while(1)
    {
        usleep(500);
        gpio_set_value(gpio,2,0);
        usleep(500);
        gpio_set_value(gpio,2,1);

    }



	close(gpio);

	return 0;
}
