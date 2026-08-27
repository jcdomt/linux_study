#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WZJian");
MODULE_DESCRIPTION("This is a bee driver");

static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

static struct file_operations misc_fops = {
    .owner = THIS_MODULE,
    .read = misc_read,
    .write = misc_write,
};

static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    const char *message = "Hello from the bee driver!\n";
    size_t len = strlen(message);

    if (*ppos >= len)
        return 0;

    if (count > len - *ppos)
        count = len - *ppos;

    if (copy_to_user(buf, message + *ppos, count))
        return -EFAULT;

    *ppos += count;
    return count;
}

static ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char message[256];

    if (count > sizeof(message) - 1)
        count = sizeof(message) - 1;

    if (copy_from_user(message, buf, count))
        return -EFAULT;

    message[count] = '\0';
    printk(KERN_INFO "Received from user: %s\n", message);
    return count;
}

static struct misc_device_t *bee_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "my_bee",
    .fops = &misc_fops,
};

void static bee_init(void)
{
    int ret = misc_register(bee_dev);
    if (ret) {
        printk(KERN_ERR "Failed to register bee device\n");
        return;
    }
    printk(KERN_INFO "Bee driver initialized\n");
}

void static bee_exit(void)
{
    printk(KERN_INFO "Bee driver exited\n");
}

MODULE_INIT(bee_init);
MODULE_EXIT(bee_exit);