
#include <linux/device.h>
#include "oled_ssd1308_spi.h"

extern struct oled_platform_data_t OLED;


static ssize_t reverse_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%ld\n", OLED.reverse_pixel);
}


static ssize_t reverse_store(struct class *class, struct class_attribute *attr,
			const char *buf, size_t count)
{
	long l;
	int ret;

	ret = kstrtol(buf, 0, &l);
	if (ret != 0 || l > 1 || l < 0)
		return 0;
	
	printk(KERN_INFO "%s: reverse %ld\n", __func__, l);
	OLED.reverse_pixel = l;
	oled_flush();
	return 2;
}


ssize_t resolution_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
        return sprintf(buf, "%d\n", OLED.pixel_x);
}


ssize_t resolution_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
        return sprintf(buf, "%d\n", OLED.pixel_y);
}


static ssize_t rotate_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", OLED.rotate);
}


static ssize_t rotate_store(struct class *class, struct class_attribute *attr,
			const char *buf, size_t count)
{
	long l;
	int ret;

        if(strlen(buf) == 0)
                return 0;
        
	ret = kstrtol(buf, 0, &l);
	if (ret != 0 || l > 360 || l < 0) {
                printk(KERN_WARNING "%s: invalid value(%ld) for rotatation\n", __func__, l);
		return 0;
        }
	
	printk(KERN_INFO "%s: rotate %ld degree, count:%d\n", __func__, l, count);
	OLED.rotate = l;
	oled_flush();
	return count;
}


//static CLASS_ATTR(reverse, 0555, reverse_show, reverse_store);
static CLASS_ATTR_RW(reverse);
static CLASS_ATTR_RO(resolution_x);
static CLASS_ATTR_RO(resolution_y);
static CLASS_ATTR_RW(rotate);
static struct class *oled_class;


void oled_ssd1308_create_class_attr()
{
        oled_class = class_create(THIS_MODULE, "oled");
        if (IS_ERR(oled_class)) {
                pr_warn("%s: Unable to create OLED class; errno = %ld\n",
                        __func__, PTR_ERR(oled_class));
        }
        else {
                int error = 0;
                error  = class_create_file(oled_class, &class_attr_reverse);
                error += class_create_file(oled_class, &class_attr_resolution_x);
                error += class_create_file(oled_class, &class_attr_resolution_y);
                error += class_create_file(oled_class, &class_attr_rotate);
                if(error)
                        printk(KERN_WARNING "%s: create attributes error!\n", __func__);
        }
}


void oled_ssd1308_destroy_class_attr()
{
	class_remove_file(oled_class, &class_attr_reverse);
        class_remove_file(oled_class, &class_attr_resolution_x);
        class_remove_file(oled_class, &class_attr_resolution_y);
        class_remove_file(oled_class, &class_attr_rotate);
	class_destroy(oled_class);
}
