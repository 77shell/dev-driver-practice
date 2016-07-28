/**************************************************************
 *  Author        :  
 *                    ..  ..
 *                   / | / |            ~~
 *                  /  |/  |    /| \ /  ~
 *                 /   | ~ |   /.|  x  ~
 *                / ~      |~ /  l / L
 *
 *  Description :  
 *
 *
 *  History:     ysh  7-17-2016          Create
 *************************************************************/


#ifndef __OLED_SSD1308_SPI_H
#define __OLED_SSD1308_SPI_H


#include <linux/types.h>


struct oled_platform_data_t {
	struct spi_device *spi;
	unsigned int pixel_x;
	unsigned int pixel_y;
	unsigned int page_nr;
	unsigned reset_pin;
	unsigned ad_pin;
	unsigned led1_pin;
	unsigned led2_pin;
	u8 *fb;
};


void oled_reset(void);
void oled_off(void);
void oled_paint(u8 byte);
void oled_init(const struct oled_platform_data_t *);

#endif /* __OLED_SSD1308_SPI_H */
