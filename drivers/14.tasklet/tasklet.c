#include <linux/module.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <linux/interrupt.h>

#include <linux/delay.h>


static int global_key_gpio = -1;
static int global_irq = -1;

struct tasklet_struct key_tasklet;

irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Key IRQ triggered\n");

    // 触发 tasklet
    tasklet_schedule(&key_tasklet);

    printk(KERN_INFO "Key IRQ handled\n");

    return IRQ_HANDLED;
}

// tasklet 处理函数
void key_tasklet_handler(unsigned long data)
{
    struct platform_device *pdev = (struct platform_device *)data;
    printk(KERN_INFO "Tasklet executed for device: %s\n", dev_name(&pdev->dev));
    ssleep(1); // 模拟耗时操作
    // 事实上，可以是写文件、发送信号等操作
    printk(KERN_INFO "Tasklet completed for device: %s\n", dev_name(&pdev->dev));
}


int key_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "Key device probed\n");

    struct device_node *node = pdev->dev.of_node;
    if (!node) {
        printk(KERN_ERR "Failed to get device node\n");
        return -ENODEV;
    }

    // 获取 GPIO 资源
    int key_gpio = of_get_named_gpio(node, "key-gpios", 0);
    if (key_gpio < 0) {
        printk(KERN_ERR "Failed to get key-gpio\n");
        return key_gpio;
    }
    int ret = gpio_request(key_gpio, "key_gpio");
    if (ret) {
        printk(KERN_ERR "Failed to request key-gpio\n");
        return ret;
    }
    ret = gpio_direction_input(key_gpio);
    if (ret) {
        printk(KERN_ERR "Failed to set key-gpio direction\n");
        gpio_free(key_gpio);
        return ret;
    }

    global_key_gpio = key_gpio;

    gpio_direction_input(key_gpio);
    gpio_set_debounce(key_gpio, 200); // 设置消抖时间为 200ms

    int irq = gpio_to_irq(key_gpio);
    if (irq < 0) {
        printk(KERN_ERR "Failed to get key-gpio irq\n");
        gpio_free(key_gpio);
        return irq;
    }
    ret = request_irq(irq, key_irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key_irq", pdev);
    if (ret) {
        printk(KERN_ERR "Failed to request key-gpio irq\n");
        gpio_free(key_gpio);
        return ret;
    }

    global_irq = irq;

    // 初始化 tasklet
    tasklet_init(&key_tasklet, key_tasklet_handler, (unsigned long)pdev);

    return 0;
}

int key_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "Key device removed\n");
    if (global_irq >= 0) {
        free_irq(global_irq, pdev);
        global_irq = -1;
    }
    if (global_key_gpio >= 0) {
        gpio_free(global_key_gpio);
        global_key_gpio = -1;
    }
    return 0;
}

struct platform_driver tasklet_platform_driver = {
    .probe = key_probe,
    .remove = key_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "key",
        .of_match_table = (const struct of_device_id []) {
            {.compatible = "test,key"},
            {}
        },
    }
};

static int __init _tasklet_init(void)
{
    printk(KERN_INFO "Tasklet module init\n");
    return platform_driver_register(&tasklet_platform_driver);
}

static void __exit tasklet_exit(void)
{
    printk(KERN_INFO "Tasklet module exit\n");
    platform_driver_unregister(&tasklet_platform_driver);
    tasklet_kill(&key_tasklet); // 确保 tasklet 已经完成
}

module_init(_tasklet_init);
module_exit(tasklet_exit);

MODULE_LICENSE("GPL");