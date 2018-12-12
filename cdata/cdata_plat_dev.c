
#include <linux/module.h>
#include <linux/platform_device.h>


static struct resource ldt_resource[] = {
};

static void ldt_dev_release(struct device *dev)
{
        printk(KERN_INFO "%s", __func__);
}

static struct platform_device ldt_platform_device = {
	.name = "cdata",
	.resource = ldt_resource,
	.num_resources = ARRAY_SIZE(ldt_resource),
	.dev.release = ldt_dev_release
};


static int ldt_plat_dev_init(void)
{
        printk(KERN_INFO "%s", __func__);
	return platform_device_register(&ldt_platform_device);
}


static void ldt_plat_dev_exit(void)
{
        printk(KERN_INFO "%s", __func__);
	return platform_device_unregister(&ldt_platform_device);
}


module_init(ldt_plat_dev_init);
module_exit(ldt_plat_dev_exit);

MODULE_LICENSE("GPL");
