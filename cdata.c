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
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include "cdata_ioctl.h"


#define CDATA_MAJOR 121
#define BUFSIZE     32

struct cdata_t {
	char *buf;
	unsigned int index;
	wait_queue_head_t write_queue;
	struct work_struct work;
};


static void flush_data(struct work_struct *pWork)
{
	struct cdata_t *cdata;
	int i;

	cdata = container_of(pWork, struct cdata_t, work);
	printk("%s: %s", __func__, cdata->buf);
	cdata->index = 0;
	for (i=0; i<BUFSIZE; i++)
		cdata->buf[i] = 0;
}


static int cdata_open(struct inode *inode, struct file *filp)
{
	struct cdata_t *cdata;
	int i;

	cdata = kmalloc(sizeof(struct cdata_t), GFP_KERNEL);
	filp->private_data = cdata;
	cdata->buf = kmalloc(BUFSIZE, GFP_KERNEL);
	cdata->index = 0;
	for (i=0; i<BUFSIZE; i++)
		cdata->buf[i] = 0;

	init_waitqueue_head(&cdata->write_queue);
	INIT_WORK(&cdata->work, flush_data);
	
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
#else

	        schedule_work(&cdata->work);
		/* 
		 * How does the event be checked? and how long?
		 * 
		 * It was checked after calling wake_up().
		 *
		 */
		wait_event_interruptible(cdata->write_queue, cdata->index + count < BUFSIZE);
#endif
		goto repeat;

		/* Don't return -ENOTTY
		 * It's inappropriate design.
		 */
		// return -ENOTTY;
	}
	
	copy_from_user(&file_buf[data_index], buf, count);
	cdata->index += count;
	printk(KERN_ALERT "%s: write %d-byte \n", __func__, count);
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
		printk(KERN_ALERT "%s empty: arg: %d\n", __func__, arg);
		for (i=0; i<BUFSIZE; i++)
			buf[i] = 0;
		p->index = 0;
		return 0;

	case IOCTL_SYNC:
		printk(KERN_ALERT "%s sync: Buffer content: %s\n", __func__, buf);
		return 0;

#if 0
	case IOCTL_NAME:
		msg = "IOCTL_NAME";
		printk(KERN_ALERT "%s: %s\n", __func__, (char *)arg);
		return 0;
#endif

#if 0
	case IOCTL_WRITE:
		strcpy(p->buf + p->index, (char*)arg);
		p->index += strlen((char*)arg);
		printk(KERN_ALERT "%s, write: %s, index: %d\n", __func__, (char*)p->buf, p->index);
		return 0;
#endif

	default:
		msg = "Unknow command";
		return -ENOTTY;
	}

	return 0;
}


static int cdata_close(struct inode *inode, struct file *filp)
{
	struct cdata_t *p;

	p = filp->private_data;
	printk(KERN_INFO "%s: Free cdata %s\n", __func__, p->buf);
	kfree(p->buf);
	kfree(p);
	return 0;
}


static struct file_operations cdata_fops = {	
	open:		cdata_open,
	write:          cdata_write,
//	compat_ioctl:   cdata_ioctl,
	unlocked_ioctl: cdata_ioctl,
	release:        cdata_close
};

void cdata_init_module(void)
{
	if (register_chrdev(CDATA_MAJOR, "cdata", &cdata_fops)) {
		printk(KERN_ALERT "cdata module: can't registered.\n");
	}
}

void cdata_cleanup_module(void)
{
	unregister_chrdev(CDATA_MAJOR, "cdata");
	printk(KERN_ALERT "cdata module: unregisterd.\n");
}

module_init(cdata_init_module);
module_exit(cdata_cleanup_module);

MODULE_LICENSE("GPL");
