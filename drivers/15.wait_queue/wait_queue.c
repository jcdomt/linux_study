#include <linux/module.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <linux/interrupt.h>

#include <linux/wait.h>
#include <linux/sched.h>

#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");

int global_key_gpio = -1;
int global_irq = -1;

static int key_status = 0;

// 等待队列相关 ***************************************************************************************
DECLARE_WAIT_QUEUE_HEAD(key_wait_queue);


// 杂项设备相关 ***************************************************************************************
ssize_t key_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char status = key_status + '0'; // 将状态转换为字符 '0' 或 '1'

    // 启用等待队列
    wait_event_interruptible(key_wait_queue, key_status != 0); // 等待按键状态改变

    if (copy_to_user(buf, &status, 1)) {
        return -EFAULT;
    }
    return 1; // 返回读取的字节数
}

ssize_t key_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char status;
    if (copy_from_user(&status, buf, 1)) {
        return -EFAULT;
    }
    if (status == '0') {
        key_status = 0;
    } else if (status == '1') {
        key_status = 1;
    } else {
        return -EINVAL; // 无效的输入
    }
    return 1; // 返回写入的字节数
}

struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .read = key_read,
    .write = key_write,
};

struct miscdevice key_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "key",
    .fops = &key_fops,
};

// 中断相关 ******************************************************************************************
irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    key_status = 1 - key_status; // 切换按键状态
    printk(KERN_INFO "Key IRQ triggered, status: %d\n", key_status);

    // 在这里唤醒等待队列中的进程
    // 重新判定条件
    wake_up_interruptible(&key_wait_queue); // 唤醒等待队列
    
    return IRQ_HANDLED;
}

// platform 驱动相关 **********************************************************************************
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

    gpio_direction_input(key_gpio);
    gpio_set_debounce(key_gpio, 200); // 设置消抖时间为 200ms

    int irq = gpio_to_irq(key_gpio);
    if (irq < 0) {
        printk(KERN_ERR "Failed to get irq for key-gpio\n");
        gpio_free(key_gpio);
        return irq;
    }
    ret = request_irq(irq, key_irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key_irq", pdev);
    if (ret) {
        printk(KERN_ERR "Failed to request irq for key-gpio\n");
        gpio_free(key_gpio);
        return ret;
    }
    global_key_gpio = key_gpio;
    global_irq = irq;

    // 注册杂项设备
    ret = misc_register(&key_misc_device);
    if (ret) {
        printk(KERN_ERR "Failed to register misc device\n");
        free_irq(irq, pdev);
        gpio_free(key_gpio);
        return ret;
    }

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
    misc_deregister(&key_misc_device);
    return 0;
}

struct platform_driver key_platform_driver = {
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


// 驱动入口 *************************************************************
static int __init wait_queue_init(void)
{
    printk(KERN_INFO "Wait queue module init\n");
    return 0;
}

static void __exit wait_queue_exit(void)
{
    printk(KERN_INFO "Wait queue module exit\n");
}

module_init(wait_queue_init);
module_exit(wait_queue_exit);