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
#include <linux/gpio.h>
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

	struct oled_platform_data_t platform_data;
};


struct gpio_t {
	unsigned pin;
	unsigned long flag;
	char *label;
};

struct oled_platform_data_t OLED;
static struct gpio_t *oled_gpios = NULL;
static ssize_t oled_gpio_nr = 0;
static int oled_init_gpios(void);
static void oled_free_gpios(void);


static int oled_init_gpios()
{	
	ssize_t i;
	int err;
	struct gpio_t gpios[] = {
		{ OLED.reset_pin, GPIOF_DIR_OUT, "OLED-Reset-Pin" },
		{ OLED.ad_pin,    GPIOF_DIR_OUT, "OLED-AD-Pin"    },
		{ OLED.led1_pin,  GPIOF_DIR_OUT, "LED1-Pin"       },
		{ OLED.led2_pin,  GPIOF_DIR_OUT, "LED2-Pin"       }
	};
	
	oled_gpio_nr = ARRAY_SIZE(gpios);
	oled_gpios = kzalloc(sizeof gpios, GFP_KERNEL);
	memcpy(oled_gpios, gpios, sizeof gpios);
	
	for (i=0; i<oled_gpio_nr; i++) {
		err = gpio_request_one(gpios[i].pin,
				       gpios[i].flag, gpios[i].label);
		if (err)
			printk(KERN_WARNING "%s: Request GPIO-%d failed~\n",
		       __func__, gpios[i].pin);
	}

	gpio_set_value(OLED.led1_pin, 0);
	gpio_set_value(OLED.led2_pin, 0);
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}


static void oled_free_gpios()
{	
	ssize_t i;
	ssize_t gpio_nr = oled_gpio_nr;
	struct gpio_t *gpios = oled_gpios;
	
	gpio_set_value(OLED.led1_pin, 1);
	gpio_set_value(OLED.led2_pin, 1);
	
	for (i=0; i<gpio_nr; i++)
		gpio_free(gpios[i].pin);
	
	kfree(oled_gpios);
	printk(KERN_INFO "%s\n", __func__);
}


static void worker_func(struct work_struct *pWork)
{
	struct ssd1308_t *ssd;
	ssd = container_of(pWork, struct ssd1308_t, worker);
	
#if 0
	printk(KERN_INFO "%s: %s\n", __func__, cdata->data);
	cdata->worker_count++;
#endif
	wake_up_interruptible(&ssd->wait_q);
}


static void timer_func(unsigned long pSSD)
{
	struct ssd1308_t *ssd = (struct ssd1308_t*)pSSD;
	ssd->timer_count++;
	oled_paint((u8)(ssd->timer_count));
	wake_up_interruptible(&ssd->wait_q);
}


static void tasklet_func(unsigned long pSSD)
{
	struct ssd1308_t *ssd = (struct ssd1308_t*)pSSD;
	ssd->tasklet_count++;
	wake_up_interruptible(&ssd->wait_q);
}


static ssize_t oled_ssd1308_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct ssd1308_t *ssd;
	int worker_count, timer_count, tasklet_count;
	unsigned long timeout;

	ssd = (struct ssd1308_t*)filp->private_data;
	down_interruptible(&ssd->sem);
	
	worker_count  = ssd->worker_count;
	timer_count   = ssd->timer_count;
	tasklet_count = ssd->tasklet_count;
	//copy_from_user(ssd->data, buf, count);

	/*
	 * Schedule worker
	 */
	printk(KERN_INFO "%s: schedule worker\n", __func__);
	schedule_work(&ssd->worker);

	/*
	 * Schedule kernel timer
	 */
#if 0
	timeout = strstr(ssd->data, "child") != NULL
		/* Child process */
		? 1 * HZ
		
		/* Parent process */
		: 7 * HZ;
#else
	timeout = 1 * HZ;
#endif
	
	printk(KERN_INFO "%s: submit timer, timeout %d\n", __func__, timeout);
	ssd->timer.expires = jiffies + timeout;
	add_timer(&ssd->timer);

	/*
	 * Schedule tasklet 
	 */
	printk(KERN_INFO "%s: schedule tasklet\n", __func__);
	tasklet_schedule(&ssd->tasklet);
	
	/*
	 * Blocking I/O
	 */
REPEAT:
#define EVENTS()					\
	( worker_count != ssd->worker_count		\
	  && timer_count   != ssd->timer_count	\
	  && tasklet_count != ssd->tasklet_count )

	wait_event_interruptible(ssd->wait_q, EVENTS());

	if ( !EVENTS() )
		goto REPEAT;
	
	printk(KERN_INFO "worker_count  : %d %d\n", worker_count, ssd->worker_count);
	printk(KERN_INFO "timer_count   : %d %d\n", timer_count, ssd->timer_count);
	printk(KERN_INFO "tasklet_count : %d %d\n", tasklet_count, ssd->tasklet_count);
	up(&ssd->sem);
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
	struct ssd1308_t *ssd;
	printk(KERN_INFO "%s\n", __func__);
	
	/* Allocate memory to private data, and set memory to zero */
	ssd = kzalloc(sizeof(struct ssd1308_t), GFP_KERNEL);
	filp->private_data = ssd;

	init_waitqueue_head(&ssd->wait_q);
	sema_init(&ssd->sem, 1);

	/* Init work queue */
	INIT_WORK(&ssd->worker, worker_func);

	/* Init timer */
	init_timer(&ssd->timer);
	ssd->timer.function = timer_func;
	ssd->timer.data = (unsigned long)ssd;

	/* Init tasklet */
	tasklet_init(&ssd->tasklet, tasklet_func, (unsigned long)ssd);
	return 0;
}

static int oled_ssd1308_close(struct inode *inode, struct file *filp)
{
	struct ssd1308_t *ssd;

	ssd = (struct ssd1308_t*)filp->private_data;
	del_timer_sync(&ssd->timer);
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
	int ret;
	unsigned int x, y;
	struct oled_platform_data_t *pData;

	if (!spi) {
		printk(KERN_WARNING "%s: spi is null. Device is not accessible\n",
		       __func__);
		return -ENODEV;
	}

	spi->bits_per_word = 8;
	ret = spi_setup(spi);
	if (ret)
		printk(KERN_WARNING  "%s: spi_setup() failed~\n", __func__);

	pData = (struct oled_platform_data_t*)spi->dev.platform_data;
	OLED = *pData;
	printk(KERN_INFO "%s: %dx, %dy, %dreset, %dad, %p-spi\n",
	       __func__,
	       pData->pixel_x, pData->pixel_y,
	       pData->reset_pin, pData->ad_pin, spi);

	OLED.spi = spi;
	printk(KERN_WARNING  "%s: spi_new_device(), 0x%x successful~\n",
	       __func__, (unsigned long)OLED.spi);

	/* Allocate memory */
	x = pData->pixel_x;
	y = pData->pixel_y / pData->page_nr;
	OLED.fb = kzalloc(x * y, GFP_KERNEL);

	oled_init_gpios();
	
	printk(KERN_INFO "%s: oled_init()\n", __func__);
	oled_init(&OLED);

	printk(KERN_INFO "%s: oled_paint()\n", __func__);
	oled_paint(0xaa);

	ret = misc_register(&oled_ssd1308_miscdev);
	ret < 0
		? printk(KERN_INFO "%s: Register OLED miscdev failed~\n", __func__)
		: printk(KERN_INFO "%s: Register OLED miscdev successful~\n", __func__);
	
	return ret;
}

static int oled_ssd1308_remove(struct spi_device *spi)
{
	oled_off();
	oled_free_gpios();
	kfree(OLED.fb);
	misc_deregister(&oled_ssd1308_miscdev);
	printk(KERN_INFO "%s: Unregister ssd miscdev successful~\n", __func__);
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
