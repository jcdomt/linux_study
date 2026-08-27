#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WZJian");
MODULE_DESCRIPTION("Text module");

static dev_t text_dev;
static struct cdev text_cdev;

static int text_open(struct inode *inode, struct file *file);
static int text_release(struct inode *inode, struct file *file);

static ssize_t text_read(struct file *file,
                         char __user *buf,
                         size_t count,
                         loff_t *ppos);

static ssize_t text_write(struct file *file,
                          const char __user *buf,
                          size_t count,
                          loff_t *ppos);

// 外部输入 echo 后，打印日志到内核日志
static ssize_t text_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char kbuf[256];
    if (count > sizeof(kbuf) - 1)
        count = sizeof(kbuf) - 1;
    if (copy_from_user(kbuf, buf, count))
        return 0;
    kbuf[count] = '\0';
    printk(KERN_INFO "Text module received: %s\n", kbuf);

    return (ssize_t)count;
}

// 外部读取 cat 后，从内核日志中读出最近的5条
static ssize_t text_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    // 这里可以实现从内核日志中读取最近的5条日志的逻辑
    // 由于内核日志的读取比较复杂，这里仅做示例
    const char *log = "Recent 5 kernel logs...\n";
    size_t log_len = strlen(log);
    if (*ppos >= log_len)
        return 0;
    if (count > log_len - *ppos)
        count = log_len - *ppos;
    if (copy_to_user(buf, log + *ppos, count))
        return 0;
    *ppos += count;

    return (ssize_t)count;
}

// 打开设备文件时的操作
static int text_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Text module device opened\n");
    return 0;
}

// 关闭设备文件时的操作
static int text_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Text module device closed\n");
    return 0;
}

static const struct file_operations text_fops = {
    .owner = THIS_MODULE,
    .open = text_open,
    .write = text_write,
    .release = text_release,
    .read = text_read,
};

static int __init text_init(void)
{
    printk(KERN_INFO "Kernel Text Module Loaded!\n");
    alloc_chrdev_region(&text_dev, 0, 1, "text_dev");
    printk(KERN_INFO "Text module registered with major: %d, minor: %d\n", MAJOR(text_dev), MINOR(text_dev));
    cdev_init(&text_cdev, &text_fops);
    text_cdev.owner = THIS_MODULE;
    cdev_add(&text_cdev, text_dev, 1);

    // 将设备文件创建在 /dev/text_dev
    /* class_create():
        @param name: 设备类的名称
        @return: 返回创建的设备类结构体指针
    */
    struct class *text_class = class_create(THIS_MODULE, "text_class");
    /* device_create():
        @param class: 设备类结构体指针
        @param parent: 父设备指针
        @param devt: 设备号
        @param drvdata: 驱动数据指针
        @param fmt: 设备名称格式化字符串
        @return: 返回创建的设备结构体指针
    */
    device_create(text_class, NULL, text_dev, NULL, "text_dev");
    return 0;
}

static void __exit text_exit(void)
{
    cdev_del(&text_cdev);
    unregister_chrdev_region(text_dev, 1);
    printk(KERN_INFO "Kernel Text Module Unloaded!\n");
}

module_init(text_init);
module_exit(text_exit);