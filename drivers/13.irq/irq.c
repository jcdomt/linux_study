#include <linux/module.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#include <linux/interrupt.h>
#include <linux/of_irq.h>

MODULE_LICENSE("GPL");

/*
test_key {
    compatible = "test,key";
    
    pinctrl-names = "default";
    pinctrl-0 = <&pinctrl_test_key>;
    key-gpios = <&gpio3 RK_PA5 GPIO_ACTIVE_LOW>;
    
    // 如果不使用 gpio_to_irq 而是使用 irq_of_parse_and_map，则需要使用以下配置
    interrupt-parent = <&gpio3>;
    interrupts = <RK_PA5 IRQ_TYPE_EDGE_BOTH>;
    
    status = "okay";
}

&pinctrl {
    pinctrl_test_key:key {
		// RK 的引脚定义是 <Bank Pin Mux Config>
		rockchip,pins = <
			RK_GPIO3 RK_PA5 RK_FUNC_0 &pcfg_pull_none_smt
		>;
	};
}
*/

static int global_key_gpio = -1;
static int global_irq = -1;

// 中断处理函数
irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Key IRQ triggered\n");
    return IRQ_HANDLED;
}

int key_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "Key device probed\n");

    int ret = 0;

    struct device_node *key_device_node = pdev->dev.of_node;

    // 申请 gpio 资源
    int key_gpio = of_get_named_gpio(key_device_node, "key-gpios", 0);
    if (key_gpio < 0) {
        printk(KERN_ERR "Failed to get key-gpio\n");
        return key_gpio;
    }

    global_key_gpio = key_gpio;

    // 设置 gpio 为输入
    ret = gpio_request(key_gpio, "key_gpio");
    if (ret) {
        printk(KERN_ERR "Failed to request key-gpio\n");
        goto func_error;
    }
    ret = gpio_direction_input(key_gpio);
    if (ret) {
        printk(KERN_ERR "Failed to set key-gpio direction\n");
        goto func_error;
    }

    // 将 GPIO 口设置为中断
    int irq = gpio_to_irq(key_gpio);
    if (irq < 0) {
        printk(KERN_ERR "Failed to get key-gpio irq\n");
        ret = irq;
        goto func_error;
    }

    // 也可以使用通用方法
    int common_irq = irq_of_parse_and_map(key_device_node, 0);
    if (common_irq < 0) {
        printk(KERN_ERR "Failed to parse and map irq\n");
        ret = common_irq;
        goto func_error;
    }
    printk(KERN_INFO "Key GPIO: %d, IRQ: %d, Common IRQ: %d\n", key_gpio, irq, common_irq);
    // 两者在理论上应该是相同的

    // 请求中断
    // 设置上升沿和下降沿触发中断
    ret = request_irq(irq, key_irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "key_irq", pdev);
    if (ret) {
        printk(KERN_ERR "Failed to request key-gpio irq\n");
        goto func_error;
    }
    global_irq = irq;

    return 0;

func_error:
    if (global_key_gpio >= 0) {
        gpio_free(global_key_gpio);
    }
    if (global_irq >= 0) {
        free_irq(global_irq, pdev);
    }

    return ret;
}

int key_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "Key device removed\n");

    if (global_irq >= 0) {
        free_irq(global_irq, pdev);
    }
    if (global_key_gpio >= 0) {
        gpio_free(global_key_gpio);
    }

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


static int __init _key_init(void)
{
    printk(KERN_INFO "Key driver init\n");

    return platform_driver_register(&key_platform_driver);
}

static void __exit key_exit(void)
{
    platform_driver_unregister(&key_platform_driver);
}

module_init(_key_init);
module_exit(key_exit);

MODULE_LICENSE("GPL");