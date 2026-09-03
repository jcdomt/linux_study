// 使用内核定时器进行按键消抖

#include <linux/module.h>
MODULE_LICENSE("GPL");

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <linux/interrupt.h>

#include <linux/timer.h>

static int global_key_gpio = -1;
static int global_key_irq = -1;


void key_shake_timer_handler(struct timer_list *t);

struct timer_data {
    struct timer_list timer;
    int gpio;
    int irq;
    struct platform_device *pdev;
};

void key_shake_timer_handler(struct timer_list *t)
{
    printk(KERN_INFO "Key shake timer handler called\n");
    // 从 timer_list 中获取我们的数据结构
    struct timer_data *data = from_timer(data, t, timer);
    // 继续读取 GPIO 状态，判断按键是否仍然按下
    int key_state = gpio_get_value(data->gpio);
    if (key_state == 0) {
        printk(KERN_INFO "Key is still pressed after debounce\n");
        // 这里可以触发按键按下的事件，比如发送输入事件
    } else {
        printk(KERN_INFO "Key is released after debounce\n");  
    }
}

irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Key IRQ handler called for IRQ %d\n", irq);

    // 我们在这里进行消抖，如果 20ms 后按键状态仍然为按下，则认为按键有效
    // 使用内核定时器
    struct timer_data *data = (struct timer_data *)dev_id;
    mod_timer(&data->timer, jiffies + msecs_to_jiffies(20));
    return IRQ_HANDLED;
}

int key_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "Key driver probe function called\n");
    
    struct device_node *node = pdev->dev.of_node;
    if (!node) {
        printk(KERN_ERR "Failed to get device node\n");
        return -ENODEV;
    }

    // 申请 GPIO 资源
    int key_gpio = of_get_named_gpio(node, "key-gpios", 0);
    if (key_gpio < 0) {
        printk(KERN_ERR "Failed to get key GPIO from device tree\n");
        return -EINVAL;
    }
    int ret = gpio_request(key_gpio, "key_gpio");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", key_gpio);
        return -EIO;
    }
    global_key_gpio = key_gpio;

    // 配置 GPIO 为输入模式
    ret = gpio_direction_input(key_gpio);
    if (ret) {
        printk(KERN_ERR "Failed to set GPIO %d as input\n", key_gpio);
        goto gpio_free;
    }
    ret = gpio_set_debounce(key_gpio, 200); // 设置消抖时间为 200ms
    if (ret) {
        printk(KERN_ERR "Failed to set debounce for GPIO %d\n", key_gpio);
        goto gpio_free;
    }

    // 从 GPIO 申请中断
    int irq = gpio_to_irq(key_gpio);
    if (irq < 0) {
        printk(KERN_ERR "Failed to get IRQ for GPIO %d\n", key_gpio);
        goto gpio_free;
    }

    // 组成 timer_data 结构体
    // 以设备的名义申请一块内存，内核会在设备移除时自动释放
    struct timer_data *data = devm_kzalloc(&pdev->dev, sizeof(struct timer_data), GFP_KERNEL);
    if (!data) {
        printk(KERN_ERR "Failed to allocate memory for timer_data\n");
        goto gpio_free;
    }
    data->gpio = key_gpio;
    data->irq = irq;
    data->pdev = pdev;
    timer_setup(&data->timer, key_shake_timer_handler, 0);
    platform_set_drvdata(pdev, data);

    ret = request_irq(irq, key_irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key_irq", data);
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", irq);
        goto irq_free;
    }
    global_key_irq = irq;

    return 0;
irq_free:
    free_irq(global_key_irq, data);
    global_key_irq = -1;
gpio_free:
    gpio_free(key_gpio);
    global_key_gpio = -1;
    return ret;
}

int key_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "Key driver remove function called\n");
    struct timer_data *data = dev_get_drvdata(&pdev->dev);
    del_timer_sync(&data->timer);

    if (data) {
        if (global_key_irq >= 0) {
            free_irq(global_key_irq, data);
            global_key_irq = -1;
        }
        if (global_key_gpio >= 0) {
            gpio_free(global_key_gpio);
            global_key_gpio = -1;
        }
    }

    return 0;
}

struct platform_driver key_driver = {
    .probe = key_probe,
    .remove = key_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "key_driver",
        .of_match_table = (const struct of_device_id[]) {
            { .compatible = "test, key" },
            { /* sentinel */ }
        },
    },
};

static int __init key_driver_init(void)
{
    return platform_driver_register(&key_driver);
}

static void __exit key_driver_exit(void)
{
    platform_driver_unregister(&key_driver);
}

module_init(key_driver_init);
module_exit(key_driver_exit);