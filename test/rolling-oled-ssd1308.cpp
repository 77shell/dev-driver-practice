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
#include <stropts.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
#include "../oled_ssd1308_ioctl.h"


#define __SYS_CALL_FSYNC
sig_atomic_t child_exit_status;


static void
print_usage(FILE *stream)
{
	fprintf(stream, "\nUsage: OLED test utility\n");
	fprintf(stream,
		"SYSNOPSIS:\n"
		"   rolling-oled-ssd1308\n"
		"\t-c\tclear screen\n"
		"\t-o\tturn on screen\n"
		"\t-i\tturn off screen\n"
		"\t-f feed-byte\tfeed screen with BYTE\n"
		"\t-r\treset screen\n"
		"\t-t\ttest times\n"
		"\t-u\tflush rate (ms)\n"
		"\t-m feed-byte\ttest mmap()\n"
		);
	
	exit(0);
}

struct Threadpara_t {
	int fd;
	int row;
	unsigned char feed;
	pthread_t th_id;
	pthread_mutex_t mutex;
};


static void*
_thread_test_mmap(void *ptr)
{
	struct Threadpara_t *th = (struct Threadpara_t*)ptr;
	size_t i, j, k, len = 128 * 64 / 8;
	size_t
		row_len = 128,
		row_start = th->row * row_len,
		row_end = row_start + row_len - 1;
	size_t
		grey_area = 0,
		grey_area_width = 8,
		grey_area_start = th->row * grey_area_width,
		grey_area_end;

	unsigned char *map;
	bool quit;
	const useconds_t _1s = 1000000;
	unsigned char a, b;
	
	map = (unsigned char*)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, th->fd, 0);
	if (map == MAP_FAILED) {
		printf("mmap() failed!\n");
		return (void*)-1;
	}

	printf("map : %p, feed : 0x%X\n", map, th->feed);
	k = th->row;
	
	for (quit = false; !quit;)
	{		
		if (pthread_mutex_trylock(&th->mutex) == 0)
			quit = true;

#if 0 /* Bar */
		for (j = 0; j < len; j++) {
			if (j == grey_area) {
				size_t i = 0;
				for (i = 0; i < 64; i++, j++)
					map[j] = 0;
			}
			else {
				map[j] = th->feed;
			}
		}

		grey_area += 64;
		if (grey_area >= len)
			grey_area = 0;
		
#elif 0 /* Wave */

		for (j=row_start; j<=row_end; j++)
			map[j] = 0xff;

		grey_area_start += grey_area_width;
		grey_area_start %= row_len;
		grey_area_end = grey_area_start + grey_area_width;

		j = row_start + grey_area_start;
		k = j + grey_area_width;

		for (; j<k; j++)
			map[j] = 0;
		
#else /* Chess board */
		
		j = row_start;
		k++;
		if (k % 2) {
			a = th->feed;
			b = 0;
		}
		else {
			a = 0;
			b = th->feed;
		}
		
		for (; j<=row_end; j++) {
			for (i=0; i<8; i++, j++)
				map[j] = a;
			
			if (j >= row_end)
				break;
			
			for (i=0; i<8; i++, j++)
				map[j] = b;
		}
#endif

#define LAST_ROW_INDEX   7
#ifdef __SYS_CALL_FSYNC
                if(th->row == LAST_ROW_INDEX)
                        fsync(th->fd);
#else
                if(th->row == LAST_ROW_INDEX)
                        ioctl(th->fd, OLED_FLUSH_PANEL);
#endif
                
		usleep(_1s);
	}
	
	munmap(map, len);
	printf("Thread (0x%lX) was terminated!\n", th->th_id);
	return (void*)0;
}


static const int thread_nr = 8;
static struct Threadpara_t para[thread_nr];

void clean_up_threads(int sig_number, siginfo_t *info, void *p)
{
        int event;
	fprintf(stderr, "%s: Clean up threads\n", __func__);
        
        for (int i=0; i<thread_nr; i++)
                pthread_mutex_unlock(&para[i].mutex);
			
        for (int i=0; i<thread_nr; i++)
                pthread_join(para[i].th_id, (void**)&event);

        for (int i=0; i<thread_nr; i++)
                pthread_mutex_destroy(&para[i].mutex);
        exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
	int
		fd,
		i,
		opt;
	char write_data[20] = { 0 };
	ssize_t ret;
	const char *dev = "/dev/oled-ssd1308";
	pid_t child;
	struct sigaction sigchld_action;
	const useconds_t _1s = 1000000;

	memset(&sigchld_action, 0, sizeof(struct sigaction));
	sigchld_action.sa_sigaction = clean_up_threads;
	sigaction (SIGINT, &sigchld_action, NULL);
	
	if ( (fd = open(dev, O_RDWR)) == -1 ) {
		fprintf(stderr, "Open %s failed~\n", dev);
		exit(EXIT_FAILURE);
	}
	
	fprintf(stderr, "Open %s successful!\n", dev);

        ioctl(fd, OLED_RESET);
        ioctl(fd, OLED_CLEAR);
	ioctl(fd, OLED_ON);
        usleep(_1s);

        unsigned char feed = 0xaa;
        ioctl(fd, OLED_FEED, &feed);
        
	{
                int i, c;
			
                for (i=0; i<thread_nr; i++) {
                        para[i].fd = fd;
                        para[i].row = i;
                        para[i].feed = feed;
                        pthread_mutex_init(&para[i].mutex, NULL);
                        pthread_mutex_lock(&para[i].mutex);
                }
                
                for (i=0; i<thread_nr; i++)
                        pthread_create(&para[i].th_id, NULL, &_thread_test_mmap, (void*)&para[i]);
        }
        while(1) usleep(_1s);
	
	close(fd);
	exit(EXIT_SUCCESS);
}
