
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include "oled_ssd1308_spi.h"


struct oled_platform_data_t oled_platform_data = {
	.pixel_x = 128,
	.pixel_y = 64
};

struct spi_board_info oled_board_info[] = {
	{
		.modalias = "oled-ssd1308",
		.platform_data = &oled_platform_data,
		.mode = SPI_MODE_0,
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 0
	}
};

static struct resource oled_resource[1];

static void oled_dev_release(struct device *dev)
{
}

static struct platform_device oled_platform_device = {
	.name = "oled-ssd1308",
	.resource = oled_resource,
	.num_resources = ARRAY_SIZE(oled_resource),
	.dev.release = oled_dev_release
};


static int oled_plat_dev_init(void)
{
	//return platform_device_register(&oled_platform_device);
	//return spi_register_board_info(oled_board_info, ARRAY_SIZE(oled_board_info));
	struct spi_master *master;
	struct spi_device *spi_dev;

	printk(KERN_WARNING "%s\n", __func__);
	master = spi_busnum_to_master(oled_board_info[0].bus_num);
	spi_dev = spi_new_device(master, oled_board_info);
	if (!spi_dev)
		printk(KERN_WARNING "%s: spi_new_device() failed~\n", __func__);
}


static void oled_plat_dev_exit(void)
{
	//return platform_device_unregister(&oled_platform_device);
}


module_init(oled_plat_dev_init);
module_exit(oled_plat_dev_exit);

MODULE_LICENSE("GPL");
