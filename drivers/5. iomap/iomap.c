#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WZJian");
MODULE_DESCRIPTION("This is a iomap driver");

void static iomap_init(void)
{
    printk(KERN_INFO "Iomap driver initialized\n");
}

void static iomap_exit(void)
{
    printk(KERN_INFO "Iomap driver exited\n");
}

MODULE_INIT(iomap_init);
MODULE_EXIT(iomap_exit);