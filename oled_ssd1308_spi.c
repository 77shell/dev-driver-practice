/**************************************************************
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  MISC category driver for test OLED-SSD1308
 *
 *
 *  History:     ysh  7-07-2016          Create
 *************************************************************/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/input.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/spi/spi.h>
#include <linux/uaccess.h>
#include "oled_ssd1308_spi.h"


#define __WORKQUEUE

#define PIXEL_X    128
#define PIXEL_Y     64

struct ssd1308_t {
	wait_queue_head_t wait_q;
	struct semaphore sem;
	struct work_struct worker;
	struct timer_list timer;
	struct tasklet_struct tasklet;
	int worker_count;
	int timer_count;
	int tasklet_count;

	struct spi_device *spi;
	struct oled_platform_data_t platform_data;
	u8 *pixel_x;
	u8 *pixel_y;
	u8 data[PIXEL_X];
	/* u8 pixel_buffer[PIXEL_X][PIXEL_Y]; */
};


static void worker_func(struct work_struct *pWork)
{
	struct ssd1308_t *cdata;

	cdata = container_of(pWork, struct ssd1308_t, worker);
	printk(KERN_INFO "%s: %s\n", __func__, cdata->data);
	cdata->worker_count++;
	wake_up_interruptible(&cdata->wait_q);
}


static void timer_func(unsigned long pCdata)
{
	struct ssd1308_t *cdata = (struct ssd1308_t*)pCdata;
	printk(KERN_INFO "%s: %s\n", __func__, cdata->data);
	cdata->timer_count++;
	wake_up_interruptible(&cdata->wait_q);
}


static void tasklet_func(unsigned long pCdata)
{
	struct ssd1308_t *cdata = (struct ssd1308_t*)pCdata;
	printk(KERN_INFO "%s: %s\n", __func__, cdata->data);
	cdata->tasklet_count++;
	wake_up_interruptible(&cdata->wait_q);
}


static ssize_t oled_ssd1308_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct ssd1308_t *cdata;
	int worker_count, timer_count, tasklet_count;
	unsigned long timeout;

	cdata = (struct ssd1308_t*)filp->private_data;
	down_interruptible(&cdata->sem);
	
	worker_count  = cdata->worker_count;
	timer_count   = cdata->timer_count;
	tasklet_count = cdata->tasklet_count;
	copy_from_user(cdata->data, buf, count);

	/*
	 * Schedule worker
	 */
	printk(KERN_INFO "%s: schedule worker\n", __func__);
	schedule_work(&cdata->worker);

	/*
	 * Schedule kernel timer
	 */
	timeout = strstr(cdata->data, "child") != NULL
		/* Child process */
		? 1 * HZ
		
		/* Parent process */
		: 7 * HZ;
	
	printk(KERN_INFO "%s: submit timer, timeout %d\n", __func__, timeout);
	cdata->timer.expires = jiffies + timeout;
	add_timer(&cdata->timer);

	/*
	 * Schedule tasklet 
	 */
	printk(KERN_INFO "%s: schedule tasklet\n", __func__);
	tasklet_schedule(&cdata->tasklet);
	
	/*
	 * Blocking I/O
	 */
REPEAT:
#define EVENTS()					\
	( worker_count != cdata->worker_count		\
	  && timer_count   != cdata->timer_count	\
	  && tasklet_count != cdata->tasklet_count )

	wait_event_interruptible(cdata->wait_q, EVENTS());

	if ( !EVENTS() )
		goto REPEAT;
	
	printk(KERN_INFO "worker_count  : %d %d, %s\n", worker_count, cdata->worker_count, cdata->data);
	printk(KERN_INFO "timer_count   : %d %d, %s\n", timer_count, cdata->timer_count, cdata->data);
	printk(KERN_INFO "tasklet_count : %d %d, %s\n", tasklet_count, cdata->tasklet_count, cdata->data);
	printk(KERN_INFO "%s: complete, %s\n", __func__, cdata->data);
	up(&cdata->sem);
	return count;
}

static long oled_ssd1308_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int oled_ssd1308_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return 0;
}

static int oled_ssd1308_open(struct inode *inode, struct file *filp)
{
	struct ssd1308_t *cdata;
	printk(KERN_INFO "%s\n", __func__);
	
	/* Allocate memory to private data, and set memory to zero */
	cdata = kzalloc(sizeof(struct ssd1308_t), GFP_KERNEL);
	filp->private_data = cdata;

	init_waitqueue_head(&cdata->wait_q);
	sema_init(&cdata->sem, 1);

	/* Init work queue */
	INIT_WORK(&cdata->worker, worker_func);

	/* Init timer */
	init_timer(&cdata->timer);
	cdata->timer.function = timer_func;
	cdata->timer.data = (unsigned long)cdata;

	/* Init tasklet */
	tasklet_init(&cdata->tasklet, tasklet_func, (unsigned long)cdata);
	return 0;
}

static int oled_ssd1308_close(struct inode *inode, struct file *filp)
{
	struct ssd1308_t *cdata;

	cdata = (struct ssd1308_t*)filp->private_data;
	printk(KERN_INFO "%s: %s\n", __func__, cdata->data);
	del_timer_sync(&cdata->timer);
	kfree(filp->private_data);
	return 0;
}

static struct file_operations oled_fops = {
	.owner = THIS_MODULE,
	.write = oled_ssd1308_write,
	.unlocked_ioctl = oled_ssd1308_ioctl,
	.mmap = oled_ssd1308_mmap,
	.open = oled_ssd1308_open,
	.release = oled_ssd1308_close
};


static struct miscdevice oled_ssd1308_miscdev = {
	.minor = 197, /* Refer to miscdev.h */
	.name = "oled-ssd1308",
	.fops = &oled_fops
};


static int oled_ssd1308_probe(struct spi_device *spi)
{
	int ret = 1;
	struct oled_platform_data_t *pData;

#if 0
	if (!spi) {
		printk(KERN_WARNING "%s: spi is null. Device is not accessible\n",
		       __func__);
		return -ENODEV;
	}
#endif

	ret = misc_register(&oled_ssd1308_miscdev);
	
	ret < 0
		? printk(KERN_INFO "%s: Register OLED miscdev failed~\n", __func__)
		: printk(KERN_INFO "%s: Register OLED miscdev successful~\n", __func__);
	
	return ret;
}

static int oled_ssd1308_remove(struct spi_device *spi)
{
	misc_deregister(&oled_ssd1308_miscdev);
	printk(KERN_INFO "%s: Unregister cdata miscdev successful~\n", __func__);
	return 0;
}

static const struct of_device_id oled_ssd1308_dt_ids[] = {
	{ .compatible = "visionox,ssd1308" },
	{}
};

MODULE_DEVICE_TABLE(of, oled_ssd1308_dt_ids);


static struct spi_driver oled_ssd1308_driver = {
	.driver = {
		.name = "oled-ssd1308",
		.owner = THIS_MODULE,
		.of_match_table = oled_ssd1308_dt_ids
	},
	.probe = oled_ssd1308_probe,
	.remove = oled_ssd1308_remove
};


module_spi_driver(oled_ssd1308_driver);


MODULE_LICENSE("GPL");
