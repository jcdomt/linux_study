#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");

static int major_num, minor_num;

module_param(major_num, int, S_IRUGO);
module_param(minor_num, int, S_IRUGO);

static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "my_char_device opened\n");
    return 0;
} 

struct cdev my_cdev = {
    .owner = THIS_MODULE,
};
struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
};
struct class *my_class;
dev_t dev = 0;

static int __init char_init(void) {
    if (major_num) {
        dev = MKDEV(major_num, minor_num);
        if (!register_chrdev_region(dev, 1, "my_char_device")) {
            return -1;
        }
    } else {
        // 不存在主设备号，尝试使用动态分配
        // 动态分配主设备号，次设备号从 0 开始，申请一个设备号
        if (alloc_chrdev_region(&dev, 0, 1, "my_char_device") < 0) {
            return -1;
        }
    }

    printk(KERN_INFO "Char driver initialized with major: %d, minor: %d\n", MAJOR(dev), MINOR(dev));

    // 开始正式注册字符设备
    cdev_init(&my_cdev, &my_fops); // 这里可以传入 file_operations
    cdev_add(&my_cdev, dev, 1);

    // 创建设备节点
    my_class =  class_create(THIS_MODULE, "my_char_class");
    device_create(my_class, NULL, dev, NULL, "my_char_device");

    return 0;
}

static void __exit char_exit(void) {
    unregister_chrdev_region(MKDEV(major_num, minor_num), 1);
    cdev_del(&my_cdev);
    class_destroy(my_class);
    device_destroy(my_class, dev);
}

module_init(char_init);
module_exit(char_exit);