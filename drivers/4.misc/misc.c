#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WZJian");
MODULE_DESCRIPTION("This is a misc driver");

static char saved_data[256];

static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    size_t len = strlen(saved_data);

    if (*ppos >= len)
        return 0;

    if (count > len - *ppos)
        count = len - *ppos;

    if (copy_to_user(buf, saved_data + *ppos, count))
        return -EFAULT;

    *ppos += count;
    return count;
}

static ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{  
    if (count > sizeof(saved_data) - 1)
        count = sizeof(saved_data) - 1;

    if (copy_from_user(saved_data, buf, count))
        return -EFAULT;

    saved_data[count] = '\0';
    return count;
}

static const struct  file_operations misc_fops = {
    .owner = THIS_MODULE,
    .read = misc_read,
    .write = misc_write,
}; 

static struct miscdevice misc_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "my_misc_device",
        .fops = &misc_fops,
};
 
static int __init misc_init(void)
{
    printk(KERN_INFO "Misc driver initialized\n");
    
    int ret = misc_register(&misc_dev);
    // 注册失败
    if (ret < 0) {
        printk(KERN_ERR "Failed to register misc device\n");
        return -1;
    }

    printk(KERN_INFO "Misc device registered with minor number %d\n", misc_dev.minor);
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&misc_dev);
    printk(KERN_INFO "Misc driver exited\n");
}

module_init(misc_init);
module_exit(misc_exit);
