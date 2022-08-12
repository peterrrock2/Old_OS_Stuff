#include <stdio.h>
#include <stdlib.h> // for malloc
#include <unistd.h> // for read() write() lseek()
#include <fcntl.h> // for open()
#include <errno.h>
#include <string.h>

int main()
{
	int fd=0;

	char nums[10]="1234567890";
	char *buff=malloc(1000);
	fd=open("mytext.txt", O_RDWR);
	
	if(fd < 0){
		printf("error opening file.\n");
		printf("error %s\n", strerror(errno));
		return -1;
	}
	//remeber to check for return values for all of your functions below for best practice
	write(fd,nums,sizeof(nums));
	
	lseek(fd,0,SEEK_SET);
	
	read(fd,buff,10);
	printf("buff: %s\n", buff);
	lseek(fd,5,SEEK_SET);
	memset(buff,0,sizeof(buff));
	read(fd,buff,10);
	printf("buff: %s\n", buff);
	free(buff);
	close(fd);
	
	return 0;
}