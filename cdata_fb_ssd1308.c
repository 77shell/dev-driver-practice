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


#define __PLATFORM_DRIVER_HELPER_MACRO

#define __WORKQUEUE

#define PIXEL_X    128
#define PIXEL_Y     64

struct cdata_fb_t {
	wait_queue_head_t wait_q;
	struct work_struct worker;
	struct timer_list timer;
	u8 pixel_buffer[PIXEL_X][PIXEL_Y];
};


static void worker_func(struct work_struct *pWork)
{
	struct cdata_fb_t *cdata;

	cdata = container_of(pWork, struct cdata_fb_t, worker);
	printk(KERN_INFO "%s\n", __func__);
}


static void timer_func(unsigned long pCdata)
{
	struct cdata_fb_t *cdata = (struct cdata_fb_t*)pCdata;
	printk(KERN_INFO "%s\n", __func__);
}


static ssize_t cdata_fb_ssd1308_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct cdata_fb_t *cdata = (struct cdata_fb_t*)filp->private_data;

	printk(KERN_INFO "%s: schedule worker\n", __func__);
	schedule_work(&cdata->worker);

	
#define TIMEOUT_VALUE   (2 * HZ)
	printk(KERN_INFO "%s: submit timer\n", __func__);
	cdata->timer.expires = jiffies + TIMEOUT_VALUE;
	add_timer(&cdata->timer);
	
	return 0;
}

static long cdata_fb_ssd1308_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int cdata_fb_ssd1308_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return 0;
}

static int cdata_fb_ssd1308_open(struct inode *inode, struct file *filp)
{
	struct cdata_fb_t *cdata;
	printk(KERN_INFO "%s\n", __func__);
	
	/* Allocate memory to private data, and set memory to zero */
	cdata = kzalloc(sizeof(struct cdata_fb_t), GFP_KERNEL);
	filp->private_data = cdata;

	init_waitqueue_head(&cdata->wait_q);

	INIT_WORK(&cdata->worker, worker_func);

	/* Init timer */
	init_timer(&cdata->timer);
	cdata->timer.function = timer_func;
	cdata->timer.data = (unsigned long)cdata;
	return 0;
}

static int cdata_fb_ssd1308_close(struct inode *inode, struct file *filp)
{
	struct cdata_fb_t *cdata;
	printk(KERN_INFO "%s\n", __func__);
	
	cdata = (struct cdata_fb_t*)flip->private_data;	
	del_timer(cdata->timer);
	kfree(filp->private_data);
	return 0;
}


static struct file_operations cdata_fb_fops = {
	.owner = THIS_MODULE,
	.write = cdata_fb_ssd1308_write,
	.unlocked_ioctl = cdata_fb_ssd1308_ioctl,
	.mmap = cdata_fb_ssd1308_mmap,
	.open = cdata_fb_ssd1308_open,
	.release = cdata_fb_ssd1308_close
};

static struct miscdevice cdata_fb_miscdev = {
	.minor = 198, /* Refer to miscdev.h */
	.name = "cdata-fb",
	.fops = &cdata_fb_fops
};

static int cdata_fb_plat_probe(struct platform_device *plat_dev)
{
	int ret;

	ret = misc_register(&cdata_fb_miscdev);
	ret < 0
		? printk(KERN_INFO "%s: Register cdata miscdev failed~\n", __func__)
		: printk(KERN_INFO "%s: Register cdata miscdev successful~\n", __func__);
	
	return ret;
}

static int cdata_fb_plat_remove(struct platform_device *plat_dev)
{
	misc_deregister(&cdata_fb_miscdev);
	printk(KERN_INFO "%s: Unregister cdata miscdev successful~\n", __func__);
	return 0;
}

static struct platform_driver cdata_fb_plat_driver = {
	.driver = {
		.name = "cdata-fb",
		.owner = THIS_MODULE
	},
	.probe = cdata_fb_plat_probe,
	.remove = cdata_fb_plat_remove
};


#ifdef __PLATFORM_DRIVER_HELPER_MACRO
int __init cdata_fb_ssd1308_init_module(void)
{
	int ret;
	printk(KERN_INFO "%s: Register cdata-fb platform driver successful\n", __func__);
	ret = platform_driver_register(&cdata_fb_plat_driver);
	return ret;
}

void __exit cdata_fb_ssd1308_cleanup(void)
{
	printk(KERN_INFO "%s: Unregister cdata-fb platform driver successful\n", __func__);
	platform_driver_unregister(&cdata_fb_plat_driver);
}
#endif

#ifdef __PLATFORM_DRIVER_HELPER_MACRO
module_init(cdata_fb_ssd1308_init_module);
module_exit(cdata_fb_ssd1308_cleanup);
#else
module_platform_driver(cdata_fb_plat_driver);
#endif

MODULE_LICENSE("GPL");
