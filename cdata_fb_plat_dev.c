/**************************************************************
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  OLED-SSD1308
 *
 *
 *  History:     ysh  7-08-2016          Create
 *************************************************************/


#include <linux/module.h>
#include <linux/platform_device.h>

static struct resource cdata_fb_resource[] = {
};

static void cdata_fb_dev_release(struct device *dev)
{
}

static struct platform_device cdata_fb_plat_device = {
	.name = "cdata-fb",
	.resource = cdata_fb_resource,
	.num_resources = ARRAY_SIZE(cdata_fb_resource),
	.dev.release = cdata_fb_dev_release
};


static int cdata_fb_plat_init(void)
{
	return platform_device_register(&cdata_fb_plat_device);
}


static void cdata_fb_plat_exit(void)
{
	return platform_device_unregister(&cdata_fb_plat_device);
}


module_init(cdata_fb_plat_init);
module_exit(cdata_fb_plat_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Yang");
