#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "cdata_ioctl.h"


int main(void)
{
	int fd, i;
	pid_t child;
	char str[20];

	strcpy(str, "OH NO! ");

	child = fork();
	printf("Child: %d\n", child);

	fd = open("/dev/cdata", O_RDWR);

	ioctl(fd, IOCTL_SYNC, 1);

	for(i=0; i<100; i++)
		write(fd, (void *)str, strlen(str));
	
	//ioctl(fd, IOCTL_SYNC, 3);
	//ioctl(fd, IOCTL_EMPTY, 2);
	close(fd);

	sleep(1);
}
