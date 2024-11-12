#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
	int fd = open("/dev/dht11",O_RDONLY);
	int fd2 = open("/dev/oled_ssd1306",O_WRONLY);
	if(fd<0||fd2<0){
		printf("cannot open\n");
		return -1;
	}


	unsigned char data[5];
	char s[50];



	ssize_t r = read(fd,data,sizeof(data));

	sprintf(s,"hum: %d,%d, tem: %d,%d",data[0],data[1],data[2],data[3]);

	ssize_t w = write(fd2,s,sizeof(s));

	close(fd);
	close(fd2);
	return 0;
}


