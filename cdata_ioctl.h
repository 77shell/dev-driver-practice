#ifndef _CDATA_IOCTL
#define _CDATA_IOCTL

#include <linux/ioctl.h>

#define IOCTL_EMPTY  _IO(0xaa, 0)
#define IOCTL_SYNC   _IO(0xaa, 1)
#define IOCTL_NAME   _IOW(0xaa, 2, char*)
#define IOCTL_WRITE  _IOR(0xaa, 3, char*)

#endif
