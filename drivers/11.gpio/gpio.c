#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");

static unsigned global_beep_gpio; // 全局变量，用于存储 GPIO 号

const struct of_device_id beep_of_match[] = {
    {.compatible = "test"},
    {}
};

static ssize_t beep_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    // 这里假设你有一个写操作函数，用于控制 GPIO
    // 例如，根据用户传入的数据来设置 GPIO 的高低电平
    char kbuf[255];
    if (copy_from_user(kbuf, buf, sizeof(kbuf))) {
        return -EFAULT;
    }

    if (kbuf[0] == '1') {
        gpio_set_value(global_beep_gpio, 1); // 设置为高电平
    } else if (kbuf[0] == '0') {
        gpio_set_value(global_beep_gpio, 0); // 设置为低电平
    } else {
        return -EINVAL; // 无效参数
    }

    return count;
}

struct file_operations beep_fops = {
    .owner = THIS_MODULE,
    .write = beep_write,
};

struct miscdevice beep_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "beep",
    .fops = &beep_fops,
};

static int beep_probe(struct platform_device *pdev)
{
    int func_ret = 0;

    printk(KERN_INFO "Beep device probed\n");

    // 获取设备节点
    struct device_node *test_device_node = pdev->dev.of_node;
    if (test_device_node == NULL) {
        printk(KERN_ERR "Failed to get device node\n");
        func_ret = -ENODEV;
        goto error_exit;
    }

    // 从设备节点中获取 GPIO
    int beep_gpio = of_get_named_gpio(test_device_node, "beep-gpios", 0);
    if (beep_gpio < 0) {
        printk(KERN_ERR "Failed to get beep-gpio\n");
        func_ret = beep_gpio;
        goto error_exit;
    }
    global_beep_gpio = beep_gpio;

    // 请求 GPIO 资源
    int ret = gpio_request(beep_gpio, "beep");
    if (ret) {
        printk(KERN_ERR "Failed to request beep_gpio\n");
        func_ret = ret;
        goto error_exit;
    }

    // 使用 GPIO
    gpio_direction_output(beep_gpio, 0); // 设置为输出，初始值为低电平
    gpio_set_value(beep_gpio, 1); // 设置为高电平

    // 注册杂项设备等用于操作 GPIO
    ret = misc_register(&beep_misc_device);
    if (ret) {
        printk(KERN_ERR "Failed to register beep misc device\n");
        func_ret = ret;
        goto error_exit;
    }
    // ...

    return 0;

error_exit:
    misc_deregister(&beep_misc_device);
    gpio_free(beep_gpio);
    global_beep_gpio = 0;

    return func_ret;
}

static int beep_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "Beep device removed\n");

    // 注销杂项设备
    misc_deregister(&beep_misc_device);

    // 释放 GPIO 资源
    gpio_free(global_beep_gpio);
    global_beep_gpio = 0;

    return 0;
}

struct platform_driver beep_driver = {
    .probe = beep_probe,
    .remove = beep_remove,
    .driver = {
        .name = "beep",
        .of_match_table = beep_of_match,
    },
};

module_platform_driver(beep_driver);