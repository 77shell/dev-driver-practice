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
 *  Name          :  test-fb-ssd1308.c
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  Test driver : cdata-fb-ssd1308
 *                 
 *  
 *  History:    ysh   7-07-2016          Create
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	char write_data[20];
	ssize_t ret;
	char *dev = "/dev/cdata-fb";

	if ( (fd = open(dev, O_RDWR)) == -1 ) {
		fprintf(stderr, "Open %s failed~\n", dev);
		exit(EXIT_FAILURE);
	}
	
	fprintf(stderr, "Open %s successful!\n", dev);

	ret = write(fd, write_data, sizeof write_data);

	close(fd);
	
	exit(EXIT_SUCCESS);
}
