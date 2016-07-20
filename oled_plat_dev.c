
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
		.bus_num = 1, /* spidev1.x */
		.chip_select = 0
	}
};


static int oled_plat_dev_init(void)
{
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
