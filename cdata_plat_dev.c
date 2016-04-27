
#include <linux/module.h>
#include <linux/platform_device.h>


static struct resource ldt_resource[] = {
	{
		.flags = IORESOURCE_IO,
		.start = 0x3f8,
		.end = 0x3ff,
	},
	{
		.flags = IORESOURCE_IRQ,
		.start = 4,
		.end = 4,
	},
	{
		.flags = IORESOURCE_MEM,
		.start = 0,
		.end = 0,
	},
};

static void ldt_dev_release(struct device *dev)
{
}

static struct platform_device ldt_platform_device = {
	.name = "cdata_dummy",
	.resource = ldt_resource,
	.num_resources = ARRAY_SIZE(ldt_resource),
	.dev.release = ldt_dev_release
};


static int ldt_plat_dev_init(void)
{
	return platform_device_register(&ldt_platform_device);
}


static void ldt_plat_dev_exit(void)
{
	return platform_device_unregister(&ldt_platform_device);
}


module_init(ldt_plat_dev_init);
module_exit(ldt_plat_dev_exit);

MODULE_LICENSE("GPL");
