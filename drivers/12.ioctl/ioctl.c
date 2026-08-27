#include <linux/module.h>

#include <linux/ioctl.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");

#define CMD_TEST_O _IO('T', 0)
#define CMD_TEST_I _IOW('T', 1, int) 
#define CMD_TEST_R _IOR('T', 2, int)

// 使用最开始的 misc_device 来尝试 ioctl

static long ioctl_handler(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case CMD_TEST_O:
            printk(KERN_INFO "CMD_TEST_O received\n");
            break;
        case CMD_TEST_I:
            printk(KERN_INFO "CMD_TEST_I received with value: %ld\n", arg);
            break;
        case CMD_TEST_R:
            {
                int value = 123; // 假设我们要返回的值
                if (copy_to_user((int __user *)arg, &value, sizeof(value))) {
                    return -EFAULT;
                }
                printk(KERN_INFO "CMD_TEST_R received, returning value: %d\n", value);
            }
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations ioctl_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = ioctl_handler, // 这里可以实现 ioctl 的处理函数
};

static struct miscdevice ioctl_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "ioctl_test",
    .fops = &ioctl_fops,
};

static int __init ioctl_init(void)
{
    printk(KERN_INFO "IOCTL module loaded\n");

    // 创建 misc 设备
    int ret = misc_register(&ioctl_misc_device);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register misc device\n");
        return ret;
    }

    return 0;
}

static void __exit ioctl_exit(void)
{
    printk(KERN_INFO "IOCTL module unloaded\n");

    // 注销 misc 设备
    misc_deregister(&ioctl_misc_device);
}

module_init(ioctl_init);
module_exit(ioctl_exit);