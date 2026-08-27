#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/miscdevice.h>

unsigned int *vir_gpio_dr;

static ssize_t beep_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char kbuf[255];
    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;
    
    if (kbuf[0] == '1' || kbuf[0] == 1) {
        *vir_gpio_dr |= (1 << 5); // 设置 GPIO5 输出高电平
    } else if (kbuf[0] == '0' || kbuf[0] == 0) {
        *vir_gpio_dr &= ~(1 << 5); // 设置 GPIO5 输出低电平
    } else {
        return -EINVAL; // 无效参数
    }

    return count;
}

static const struct file_operations beep_fops = {
    .owner = THIS_MODULE,
    .write = beep_write,
};

static struct miscdevice beep_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "beep",
    .fops = &beep_fops,
};

static int beep_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "Beep device probed\n");


    printk(KERN_INFO "Beep device resource start: %pa\n", &pdev->resource[0].start);
    printk(KERN_INFO "Beep device resource end: %pa\n", &pdev->resource[0].end);

    struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        printk(KERN_ERR "Failed to get resource\n");
        return -EBUSY;
    }
    printk(KERN_INFO "Beep device resource: %pa\n", &res->start);

    // 登记设备的资源
    if (request_mem_region(res->start, res->end - res->start + 1, pdev->name)) {
        printk(KERN_ERR "Failed to request memory region\n");
        goto err_release_resource;
        return -EBUSY;
    }

    // 注册杂项设备
    vir_gpio_dr = ioremap(res->start, res->end - res->start + 1);

    int ret = misc_register(&beep_misc_device);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register misc device\n");
        goto err_release_resource;
    }


    return 0;

err_release_resource:
    release_mem_region(res->start, res->end - res->start + 1);
    return -EBUSY;
}

static int beep_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "Beep device removed\n");
    iounmap(vir_gpio_dr);
    return 0;
}

struct platform_driver beep_driver = {
    .probe = beep_probe,
    .remove = beep_remove,

    .driver = {
        // 和 device.c 中的 name 一致
        .name = "beep",
        .owner = THIS_MODULE,
    },
};

static int __init beep_driver_init(void)
{
    return platform_driver_register(&beep_driver);
}

static void __exit beep_driver_exit(void)
{
    platform_driver_unregister(&beep_driver);
}

module_init(beep_driver_init);
module_exit(beep_driver_exit);

MODULE_LICENSE("GPL");