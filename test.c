

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "cdata_ioctl.h"


int main(void)
{
	int fd, i;
	pid_t child;
	char str[20];
	useconds_t us;
	const useconds_t _100ms = 100000;

	strcpy(str, "OH NO! ");

	child = fork();
	printf("Child: %d\n", child);

	if ( (fd = open("/dev/cdata-misc", O_RDWR)) == -1) {
		fprintf(stderr, "Open /dev/cdata-misc failed~\n");
		exit(EXIT_FAILURE);
	}

	ioctl(fd, IOCTL_SYNC, 1);

	for(i=0; i<100; i++) {
		write(fd, (void *)str, strlen(str));
		usleep(_100ms);
	}
	
	//ioctl(fd, IOCTL_SYNC, 3);
	//ioctl(fd, IOCTL_EMPTY, 2);
	close(fd);

	exit(EXIT_SUCCESS);
}
