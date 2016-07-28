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


sig_atomic_t child_exit_status;

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
	int fd, i;
	char write_data[20] = { 0 };
	ssize_t ret;
	char *dev = "/dev/oled-ssd1308";
	pid_t child;
	struct sigaction sigchld_action;

	/*
	 * If child process completes earlier than parent process,
	 * Child process becomes a zombie process, parent process has to
	 * clean up child process by calling wait().
	 */
	memset(&sigchld_action, 0, sizeof(struct sigaction));
	sigchld_action.sa_sigaction = clean_up_child_proc;
	sigaction (SIGCHLD, &sigchld_action, NULL);

	child = fork();
	if (child == 0)
		strcpy(write_data, "I'm a child");
	else
		strcpy(write_data, "I'm parent");
	
	if ( (fd = open(dev, O_RDWR)) == -1 ) {
		fprintf(stderr, "Open %s failed~\n", dev);
		exit(EXIT_FAILURE);
	}
	
	fprintf(stderr, "Open %s successful!\n", dev);

	for(i=0; i<100; i++) {
		ret = write(fd, write_data, sizeof write_data);
		printf("%s : %d\n", write_data, ret);
	}
	
	close(fd);
	
	exit(EXIT_SUCCESS);
}
