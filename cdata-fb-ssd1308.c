/**************************************************************
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  OLED-SSD1308 driver, MISC category
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

static int cdata_fb_plat_probe(struct platform_device *);
static int cdata_fb_plat_remove(struct platform_device *);


static ssize_t cdata_fb_ssd1308_write(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
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


static int cdata_fb_ssd1308_close(struct inode *inode, struct file *filp)
{
	return 0;
}


static struct file_operations cdata_fb_fops = {
	.owner = THIS_MODULE,
	.write = cdata_fb_ssd1308_write,
	.unlocked_ioctl = cdata_fb_ssd1308_ioctl,
	.mmap = cdata_fb_ssd1308_mmap,
	.open = cdata_fb_ssd1308_open,
	.released = cdata_fb_ssd1308_close
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
		? printk(KERN_INFO "Register cdata_fb_miscdev failed~\n")
		: printk(KERN_INFO "Register cdata_fb_miscdev successful~\n");
	
	return ret;
}

static int cdata_fb_plat_remove(struct platform_device *plat_dev)
{
	platform_device_unregister(&cdata_fb_miscdev);
	printk(KERN_INFO "Unregister cdata_fb_miscdev successful~\n");
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

int __init cdata_fb_ssd1308_init_module(void)
{
	int ret;
	printk(KERN_INFO "Register cdata-fb driver successful\n");
	ret = platform_driver_register(&cdata_fb_plat_driver);
	return ret;
}

void __exit cdata_fb_ssd1308_cleanup(void)
{
	printk(KERN_INFO "Unregister cdata-fb driver successful\n");
	platform_driver_unregister(&cdata_fb_plat_driver);
}


module_init(cdata_fb_ssd1308_init_module);
module_exit(cdata_fb_ssd1308_cleanup);
MODULE_LICENSE("GPL");
