/*****************************************************************
 *  Copyright (c) 2016 Delta Products
 *
 *  THIS IS UNPUBLISHED PROPRIETARY TRADE SECRET SOURCE CODE OF
 *  Delta Products
 *
 *  The copyright above and this notice must be preserved in all copies of this
 *  source code.  The copyright notice above does not evidence any actual or
 *  intended publication of such source code.  This source code may not be 
 *  copied, disclosed, distributed, demonstrated or licensed except as expressly
 *  authorized by Delta Products.
 ****************************************************************/



/**************************************************************
 *  Name          :  test-oled-ssd1308.c
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  Test driver : oled-ssd1308
 *                 
 *  
 *  History:    ysh   7-28-2016          Create
 *************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "oled_ssd1308_ioctl.h"


sig_atomic_t child_exit_status;


static void
print_usage(FILE *stream)
{
	fprintf(stream, "\nUsage: OLED test utility\n");
	fprintf(stream,
		"SYSNOPSIS:\n"
		"   test-oled-ssd1308\n"
		"\t-c\tclear screen\n"
		"\t-o\tturn on screen\n"
		"\t-i\tturn off screen\n"
		"\t-f\tfeed screen with BYTE\n"
		"\t-r\treset screen\n"
		"\t-t\ttest times\n"
		"\t-u\tflush rate (ms)\n"
		);
	
	exit(0);
}


void clean_up_child_proc(int sig_number, siginfo_t *info, void *p)
{
	/* Clean up the child process */
	int status;
	pid_t child_pid;
	child_pid = wait(&status);

	printf("%s: Child PID %d\n", __func__, child_pid);
	child_exit_status = status;
}


int main(int argc, char *argv[])
{
	int
		fd,
		i,
		opt,
		run = 0;
	char write_data[20] = { 0 };
	ssize_t ret;
	char *dev = "/dev/oled-ssd1308";
	pid_t child;
	struct sigaction sigchld_action;
	const useconds_t _1s = 1000000;

	/*
	 * If child process completes earlier than parent process,
	 * Child process becomes a zombie process, parent process has to
	 * clean up child process by calling wait().
	 */
	memset(&sigchld_action, 0, sizeof(struct sigaction));
	sigchld_action.sa_sigaction = clean_up_child_proc;
	sigaction (SIGCHLD, &sigchld_action, NULL);

#if 0
	child = fork();
	if (child == 0)
		strcpy(write_data, "I'm a child");
	else
		strcpy(write_data, "I'm parent");
#endif
	
	if ( (fd = open(dev, O_RDWR)) == -1 ) {
		fprintf(stderr, "Open %s failed~\n", dev);
		exit(EXIT_FAILURE);
	}
	
	fprintf(stderr, "Open %s successful!\n", dev);

	
	while( (opt = getopt(argc, argv, ":rct:oif:u:")) != -1 )
	{
		switch(opt) {
		case ':':
		case '?': print_usage(stderr); break;
		case 'c': ioctl(fd, OLED_CLEAR); break;
		case 'o': ioctl(fd, OLED_ON); break;
		case 'i': ioctl(fd, OLED_OFF); break;
		case 'r': ioctl(fd, OLED_RESET); break;
			
		case 'f':
		{
			unsigned char feed;
			feed = (unsigned char)atoi(optarg);
			ioctl(fd, OLED_FEED, &feed);
			printf("Feed byte: 0x%X\n", feed);
		}
		break;
			
		case 't':
			run = atoi(optarg);
			printf("Run test %d times\n", run);
			break;

		case 'u':
		{
			unsigned long rate;
			rate = (unsigned long)atoi(optarg);
			ioctl(fd, OLED_FLUSH_RATE, rate);
			printf("Flush rate : %lums\n", rate);
		}
		break;
		}
	}
	
	for(i=0; i<run; i++) {
		unsigned char b = (unsigned char)i;
		ret = write(fd, &b, sizeof(unsigned char));
		printf("%s : %d\n", __func__, i);
		usleep(_1s / 2);
	}

	printf("Press any key to quit!");
	getchar();
	close(fd);
	
	exit(EXIT_SUCCESS);
}
