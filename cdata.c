/**************************************************************
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  Demostrate device driver
 *
 *
 *  History:     ysh  4-23-2016          Create
 *************************************************************/


#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/input.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include "cdata_ioctl.h"


#define CDATA_MAJOR 121
#define BUFSIZE     32
#define TIMEOUT_VALUE (1*HZ)

#define __WORK_QUEUE
// #define __TIMER

// #define __MKNOD
#define __PLAT_DRIVER


#if defined(__WORK_QUEUE) && defined(__TIMER)
#error "Wrong configuration, either on __WORK_QUEUE or __TIMER"
#endif


struct cdata_t {
	char *buf;
	unsigned int index;
	wait_queue_head_t write_queue;
	struct work_struct work;
	struct timer_list timer;
};


static void wq_flush_data(struct work_struct *pWork)
{
	struct cdata_t *cdata;
	int i;

	cdata = container_of(pWork, struct cdata_t, work);
	printk(KERN_INFO "%s: %s", __func__, cdata->buf);
	cdata->index = 0;
	for (i=0; i<BUFSIZE; i++)
		cdata->buf[i] = 0;
	wake_up(&cdata->write_queue);
}


#ifdef __TIMER
static void timer_handle(unsigned long data)
{
	struct cdata_t *cdata = (struct cdata_t*)data;
        int i;

	printk(KERN_INFO "%s: %s", __func__, cdata->buf);
	cdata->index = 0;
	for (i=0; i<BUFSIZE; i++)
		cdata->buf[i] = 0;
	wake_up(&cdata->write_queue);
}
#endif /* __TIMER */


static int cdata_open(struct inode *inode, struct file *filp)
{
	struct cdata_t *cdata;
	int i;

	/* Allocate memory to private_data */
	cdata = kmalloc(sizeof(struct cdata_t), GFP_KERNEL);
	filp->private_data = cdata;
	cdata->buf = kmalloc(BUFSIZE, GFP_KERNEL);

	/* Init cdata_t content */
	{
		cdata->index = 0;
		for (i=0; i<BUFSIZE; i++)
			cdata->buf[i] = 0;
	}

	init_waitqueue_head(&cdata->write_queue);
        
#ifdef __WORK_QUEUE
        
        printk(KERN_INFO "%s: Apply work queue\n", __func__);
	INIT_WORK(&cdata->work, wq_flush_data);

#elif defined(__TIMER)

	/* Init timer */
	{
                printk(KERN_ALERT "%s: Apply timer\n", __func__);
		struct timer_list *pTimer;
		pTimer = &cdata->timer;
		init_timer(pTimer);
		pTimer->function = timer_handle;
		pTimer->data = (unsigned long)cdata;
	}
#endif
	
	printk(KERN_ALERT "%s: cdata in open: filp = %p\n", __func__, filp);
	return 0;
}


static ssize_t cdata_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct cdata_t *cdata;
	char *file_buf;
	unsigned int data_index;

	cdata = filp->private_data;
	file_buf = cdata->buf;
        
repeat:
	data_index = cdata->index;

	if (data_index + count >= BUFSIZE) {
#if 0
		/*
		 * Traditional way of doing process scheduling
		 *    1. Change current process state
		 *    2. Put to wait-queue
		 *    3. Call scheduler
		 */
		current->state = TASK_UNINTERRUPTIBLE;
		add_wait_queue();
		schedule();
#endif

#if 0
		/*
		 * Upon three statements can be replaced by following statement.
		 * But this is not an atomic operaton, so it could result some problems.
		 */
		interrupt_sleep_on(&cdata->write_queue);
#endif

#ifdef __WORK_QUEUE
		
	        schedule_work(&cdata->work);
		
#elif defined __TIMER
		
		cdata->timer.expires = jiffies + TIMEOUT_VALUE;
		add_timer(&cdata->timer);
#endif
		/* 
		 * How does the event be checked? and how long?
		 * 
		 * It was checked after calling wake_up().
		 *
		 */
		wait_event_interruptible(cdata->write_queue, cdata->index + count < BUFSIZE);

		goto repeat;

		/* Don't return -ENOTTY
		 * It's inappropriate design.
		 */
		// return -ENOTTY;
	}
	
	if (copy_from_user(&file_buf[data_index], buf, count))
		return 0;
	
	cdata->index += count;
	printk(KERN_INFO "%s: write %d-byte, buffer-index: %d\n", __func__, count, cdata->index);
	return count;
}


static long cdata_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	char *msg, *buf;
	struct cdata_t *p;
	unsigned int i, index;

	p = (struct cdata_t *)filp->private_data;
	buf = p->buf;
	index = p->index;

	switch (cmd) {
	case IOCTL_EMPTY:
		printk(KERN_ALERT "%s: empty: arg: %ld\n", __func__, arg);
		for (i=0; i<BUFSIZE; i++)
			buf[i] = 0;
		p->index = 0;
		return 0;

	case IOCTL_SYNC:
		printk(KERN_ALERT "%s sync: Buffer content: %s\n", __func__, buf);
		return 0;

	default:
		msg = "Unknow command";
		return -ENOTTY;
	}

	return 0;
}


static int cdata_close(struct inode *inode, struct file *filp)
{
	struct cdata_t *p;

#ifdef __TIMER
	int ret;
#endif
	
	p = filp->private_data;

#ifdef __TIMER
	ret = del_timer_sync(&p->timer);
	if (ret == 0)
		printk(KERN_INFO "%s: timer has already been removed.\n", __func__);
	else if (ret > 0)
		printk(KERN_INFO "%s: timer has been removed before closing.\n", __func__);
#endif
	
	printk(KERN_INFO "%s: Free cdata %s\n", __func__, p->buf);

	kfree(p->buf);
	kfree(p);
	return 0;
}


static struct file_operations cdata_fops = {
        owner:          THIS_MODULE,
	open:		cdata_open,
	write:          cdata_write,
//	compat_ioctl:   cdata_ioctl,
	unlocked_ioctl: cdata_ioctl,
	release:        cdata_close
};


#if !defined(__MKNOD) || defined(__PLAT_DRIVER)
static struct miscdevice cdata_miscdev = {
	.minor = 199, /* Refer to miscdevice.h */
	.name = "cdata.0",
	.fops = &cdata_fops
	/* .nodename = "cdata" */
};
#endif


#ifdef __PLAT_DRIVER
static int cdata_plat_probe(struct platform_device *dev)
{
	int ret;
	
	ret = misc_register(&cdata_miscdev);
	if(ret < 0) {
		printk(KERN_ALERT "%s: registering failed\n", __func__);
		return -1;
	}
	
	printk(KERN_ALERT "%s: register MISC successful\n", __func__);
	return 0;
}
#endif


#ifdef __PLAT_DRIVER
static int cdata_plat_remove(struct platform_device *dev)
{
	misc_deregister(&cdata_miscdev);
	printk(KERN_ALERT "%s: cdata was unregisted.\n", __func__);
	return 0;
}
#endif


#ifdef __PLAT_DRIVER
static struct platform_driver cdata_plat_driver = {
	.driver = {
		.name = "cdata",
		.owner = THIS_MODULE
	},
	.probe = cdata_plat_probe,
	.remove = cdata_plat_remove
};
#endif


int __init cdata_init_module(void)
{
#ifdef __MKNOD
        
	if (register_chrdev(CDATA_MAJOR, "cdata", &cdata_fops)) {
		printk(KERN_ALERT "%s: cdata module: can't registered.\n", __func__);
	}

#elif defined( __PLAT_DRIVER)
        
	platform_driver_register(&cdata_plat_driver);
	printk(KERN_INFO "%s: register platform driver successful\n", __func__);

#else
        
	int ret;
	
	ret = misc_register(&cdata_miscdev);
	if(ret < 0)
		printk(KERN_ALERT "%s: registering failed\n", __func__);
	
	printk(KERN_ALERT "%s: register MISC successful\n", __func__);
#endif
        
	return 0;
}


void __exit cdata_cleanup_module(void)
{
#ifdef __MKNOD
	unregister_chrdev(CDATA_MAJOR, "cdata");
	printk(KERN_ALERT "cdata module: unregisterd.\n");

#elif defined(__PLAT_DRIVER)
        
	platform_driver_unregister(&cdata_plat_driver);
	printk(KERN_ALERT "%s: platform driver was unregisted.\n", __func__);
#else
        
	misc_deregister(&cdata_miscdev);
	printk(KERN_ALERT "%s: MISC cdata was unregisted.\n", __func__);
#endif
}


module_init(cdata_init_module);
module_exit(cdata_cleanup_module);

MODULE_LICENSE("GPL");
