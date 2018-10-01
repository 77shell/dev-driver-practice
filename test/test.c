

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "../cdata_ioctl.h"


static void
print_usage()
{
        puts("test -d /dev/cdata.0");
}


int main(int argc, char *argv[])
{
	int fd, i, opt;
	pid_t child;
	char str[20];
        char *dev = "/dev/cdata.0";
	useconds_t us;
	const useconds_t _100ms = 100000;
	const useconds_t _10ms = 10000;
        const useconds_t _1ms = 1000;

        while( (opt = getopt(argc, argv, "d:")) != -1 )
        {
                switch(opt) {
                case ':':
                case '?':
                        print_usage();
                        exit(EXIT_SUCCESS);

                case 'd':
                        dev = optarg;
                        printf("Device: %s\n", dev);
                        break;
                }
        }

	strcpy(str, "______");

	if ( (fd = open(dev, O_RDWR)) == -1) {
		fprintf(stderr, "Open %s failed~\n", dev);
		exit(EXIT_FAILURE);
	}

	child = fork();
	printf("Child: %d\n", child);

        if(child)
                strcpy(str, "******");
        
	//ioctl(fd, IOCTL_SYNC, 1);

	for(i=0; i<100; i++) {
		write(fd, (void *)str, strlen(str));
		//usleep(_100ms);
	}

        printf("complete %d\n", child);
	//ioctl(fd, IOCTL_SYNC, 3);
	//ioctl(fd, IOCTL_EMPTY, 2);
	close(fd);

	exit(EXIT_SUCCESS);
}
